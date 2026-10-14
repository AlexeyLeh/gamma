/*!
  \file   bto.cpp
  \author Aleksey Lekhtin
  \brief  Базовое уравление БТО
  \date   10.05.2023
*/

#include <iostream>
#include <ostream>

#include <QApplication>
#include <QtDebug>
#include <QMessageBox>
#include <QThread>
#include <NodeBA.h>

#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "dbg.h"
#include "diagn_gui.h"
#include "gamma.h"
#include "main.h"
#include "mainwindow.h"
#include "node.h"
#include "NodeBA.h"
#include "utils.h"

static constexpr int BtoPollPeriodFast  = 100;     // ms
static constexpr int BtoPollPeriod      = 3000;    // ms
static constexpr int BtoPingPeriod      = 10000;   // ms

BTO::BTO(APP *m, BTO_GUI *gui) :
    m_app(m),
    m_gui(gui),
    m_gamma_module_pn(),
    m_module_powering_step(GAMMA_STATUS::NOT_POWERED),
    m_settings_bto_autoconnect(false),
    m_settings_gamma_autopower(false)
{
    qRegisterMetaType<byte>("byte");

    read_settings(m_app->qSettings());
    m_gui->set_backgr(this);


    connect(comm(), &COMM::signal_comm_ethernet_connected,
            this, &BTO::slot_comm_ethernet_connected, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_ethernet_disconnected,
            this, &BTO::slot_comm_ethernet_disconnected, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_ethernet_canceled,
            this, &BTO::slot_comm_ethernet_canceled, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_connection_error,
            this, &BTO::slot_comm_connection_error, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_not_responded,
            this, &BTO::slot_comm_not_responded, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_requests_completed,
            this, &BTO::slot_comm_requests_completed, Qt::QueuedConnection);

    m_timer_bto_polling.setSingleShot(true);
    connect(&m_timer_bto_polling, &QTimer::timeout, this, &BTO::slot_bto_polling);

    connect(comm(), &COMM::signal_comm_NodeBA_responded, this, [this](byte cmd, byte ext) {
        m_gui->update_NodeBA_fields(cmd, ext);
    });

    connect(comm(), &COMM::signal_comm_NodeE4_responded, this, [this](byte cmd, byte ext) {
        m_gui->update_nodeE4_fields(cmd, ext);
    });

    connect(comm(), &COMM::signal_comm_incorrect_resonse,
            this, &BTO::slot_comm_incorrect_resonse, Qt::QueuedConnection);
}

//---------------------------------------------------------
BTO::~BTO()
{
    DBG(4);
    stop_bto_timer();
}

//---------------------------------------------------------
QString BTO::part_number() const
{
    return m_part_number;
}

//---------------------------------------------------------
void BTO::set_part_number(const QString &text)
{
    m_part_number = text;
}

//---------------------------------------------------------
void BTO::set_type(NODECODE num)
{
    m_bto_type = num;
    nodeBA()->set_code(num);
    nodeBA()->create_bto_requests();
    nodeBA()->create_gamma_requests();
}

//---------------------------------------------------------
BTO::MODULE_KIND BTO::gamma_module_variant() const
{
    return m_gamma_module_kind;
}

//---------------------------------------------------------
void BTO::set_gamma_module_kind(const BTO::MODULE_KIND &kind)
{
    m_gamma_module_kind = kind;
}

//---------------------------------------------------------
QString BTO::gamma_module_pn() const
{
    return m_gamma_module_pn;
}

//---------------------------------------------------------
void BTO::set_gamma_module_pn(const QString &pn)
{
    m_gamma_module_pn = pn;
}

//---------------------------------------------------------
bool BTO::gamma_module_found() const
{
    return m_module_powering_step == GAMMA_STATUS::FOUND; // 1
}

//---------------------------------------------------------
void BTO::push_upload_settings_button()
{
    DBG(3);
    if (type() == NodeBA::CODExBA) {
        comm()->push_requests(
                    std::list<Adam::Frame> {
                        nodeBA()->WriteRequiredVoltage(m_settings_voltage_limit),
                        nodeBA()->ReadRequiredVoltage()
                    });
    } else if (type() == NodeBA::CODEx7A) {
        comm()->push_requests(
                    std::list<Adam::Frame> {
                        nodeBA()->WriteRequiredVoltage(m_settings_voltage_limit),
                        nodeBA()->WriteRequiredAmperage(m_settings_amperage_limit),
                        nodeBA()->ReadRequiredVoltage(),
                        nodeBA()->ReadAmperage(),
                    });
    }
}

//---------------------------------------------------------
bool BTO::push_gamma_power_button(bool on)
{
    DBG(1, QString("\nmodule power going to be %1").arg(on ? "ON" : "OFF"));
    stop_bto_timer();
    mw()->delay(200);

    std::list<Adam::Frame> frames;
    if (on)
    {
        gui()->set_enabled_gamma_power_button(false);
        bool ok = make_module_poweron_request(frames);
        if (!ok) {
            gui()->set_enabled_gamma_power_button(true);
            gui()->set_gamma_power_button_status(false);
            start_bto_timer();
            return false;
        }
        gui()->progress_bar_message_show(tr("Powering the module"));
        gui()->update_gamma_status(GAMMA_STATUS::SEARCH);
        gui()->set_led_icon_active();
        gui()->set_enabled_gamma_power_button(true);
    }
    else
    {
        gui()->set_enabled_gamma_power_button(false);
        gui()->set_gamma_power_button_status(false);
        gui()->update_gamma_status(BTO::GAMMA_STATUS::NOT_POWERED);
        //gui()->clear_nodeE4_fields();
        make_module_poweroff_request(frames);
    }

    comm()->push_requests(frames);
    DBGNL(2);
    return true;
}

//---------------------------------------------------------
bool BTO::make_module_poweron_request(std::list<Adam::Frame> &frames)
{
    // 1. Проверка - включен-ли уже источник питания?
    if (nodeBA()->power_input_switch() == NodeBA::PowerInputSwitch::ON)
    {
        DBG(1, QString("%1 %2")
            .arg(QString::fromStdString(nodeBA()->code_name()))
            .arg("Input Switch already powered"));
        return false;
    }

    // 2. Проверка - есть-ли уже напряжение на выходе БТО?
    // (возможно есть постороннее напряжение на выходе)
    if (nodeBA()->output_voltage() > NodeBA::THRESHOLD_4_5V)
    {
        auto msg = QString("[%1]\n%2 %3%4")
                .arg(QString::fromStdString(nodeBA()->code_name()))
                .arg(tr("Seems the output under extraneous voltage"))
                .arg(nodeBA()->output_voltage(), 6, 'f', 2, QChar(' '))
                .arg(tr("V"));
        DBG(1, msg);
        emit mw()->signal_message(QMessageBox::Warning, {}, UTILS::split_message(msg));
        return false;
    }

    // 3. Выбор источника питания для выдачи выходного напряжения
    // полное отключение выхода напряжения
    auto out_sw = NodeBA::PowerOutputType::OFF;
    DBG(2, QString("[%1] going to '%2'")
        .arg(QString::fromStdString(nodeBA()->code_name()))
        .arg(NodeBA::power_output_name(out_sw).c_str()));
    frames.emplace_back(nodeBA()->WritePowerOutputSwitch(out_sw));

    // переключение на нужный вариант выхода напряжения
    out_sw = gui()->get_power_output_switch_from_settings();
    DBG(2, QString("[%1] going to '%2'")
        .arg(QString::fromStdString(nodeBA()->code_name()))
        .arg(NodeBA::power_output_name(out_sw).c_str()));
    frames.emplace_back(nodeBA()->WritePowerOutputSwitch(out_sw));

    // 4. Подать питание (если выбрано батарейное, то источник питания не подключается)
    out_sw = nodeBA()->power_output_type();
    if (out_sw == NodeBA::PowerOutputType::OFF || out_sw == NodeBA::PowerOutputType::INT)
    {
        auto in_sw = NodeBA::PowerInputSwitch::ON; // подключить источник питания
        DBG(2, QString("[%1] going to '%2'")
            .arg(QString::fromStdString(nodeBA()->code_name()))
            .arg(NodeBA::input_switch_name(in_sw).c_str()));
        frames.emplace_back(nodeBA()->WritePowerInputSwitch(in_sw));
    }

    return true;
}

//---------------------------------------------------------
void BTO::make_module_poweroff_request(std::list<Adam::Frame> &frames)
{
    // Отключение выхода напряжения
    auto out_sw = NodeBA::PowerOutputType::OFF;
    DBG(1, QString("[%1] going to %2")
        .arg(QString::fromStdString(nodeBA()->code_name()))
        .arg(NodeBA::power_output_name(out_sw).c_str()));
    frames.emplace_back(nodeBA()->WritePowerOutputSwitch(out_sw));

    // Отсоединение источника питания
    auto in_sw = NodeBA::PowerInputSwitch::OFF;
    DBG(1, QString("[%1] going to %2")
        .arg(QString::fromStdString(nodeBA()->code_name()))
        .arg(NodeBA::input_switch_name(in_sw).c_str()));
    frames.emplace_back(nodeBA()->WritePowerInputSwitch(in_sw));

    // считать сразу новые значения
    frames.splice(frames.end(),
                  std::list<Adam::Frame>{
                      nodeBA()->ReadPowerOutputSwitch(),
                      nodeBA()->ReadPowerInputSwitch(),
                      nodeBA()->ReadOutputVoltage(),
                  });

    gui()->clear_NodeE4_PWR_fields();
}

//---------------------------------------------------------
void BTO::connect_to() const
{
    comm()->connect_to();
}

//---------------------------------------------------------
void BTO::disconnect_from()
{
    DBG(3);
    m_initial_start = 0;
    stop_bto_timer();
    if (!comm()->isConnected()) // QAbstractSocket::UnconnectedState or comport not open
    {
        DBG(3, "already disconnected");
        gui()->disconnected_event();
        return;
    }
    comm()->disconnect_from();
}

//---------------------------------------------------------
void BTO::cancel_connection()
{
    comm()->cancel_connection();
}

//---------------------------------------------------------
void BTO::start_bto_timer(int interval)
{
    if (m_timer_bto_polling.isActive()) {
        return;
    }
    DBGF(3, "timer bto polling start(%d)", interval);
    m_timer_bto_polling.start(interval);
}

//---------------------------------------------------------
void BTO::stop_bto_timer()
{
    DBG(2);
    m_timer_bto_polling.stop();
    comm()->clear_requests();
}

//---------------------------------------------------------
void BTO::ping()
{
    if (m_timer_bto_polling.isActive()) {
        return;
    }
    // просто опрос БТО, чтобы не отключал сетевое соединение
    DBG(2, "ping only...");
    if (comm()->push_request(nodeBA()->ReadPartNumber()) != 0) {
        gui()->set_led_icon_active();
    }
}

//---------------------------------------------------------
void BTO::slot_bto_polling()
{
    if (mw()->shuttingdown()) {
        DBG(1, "shuttingdown...");
        return;
    }

    if (!comm()->isConnected()) {
        DBG(1, "(!) no connection - skip");
        m_initial_start = 0;
        stop_bto_timer();
        return;
    }

    if (mw()->current_tab() != MW::PAGES::BTO_PAGE) {
        ping();
        return;
    }

    DBG(2);
    gui()->set_led_icon_active();
    mw()->showstatus(tr("polling..."), 5000);

    if (m_module_powering_step == GAMMA_STATUS::FOUND) {
        if (m_next_request_for_gamma) {
            m_next_request_for_gamma = !m_next_request_for_gamma;
            std::list<Adam::Frame> frames = nodeBA()->gamma_requests();
            comm()->push_requests(frames);
            return;
        }
    }

    std::list<Adam::Frame> frames = nodeBA()->bto_requests();
    if (comm()->push_requests(frames) == 0) {
        gui()->set_led_icon_inactive();
    }

    if (m_module_powering_step == GAMMA_STATUS::FOUND) {
        m_next_request_for_gamma = true;
    }
}

//---------------------------------------------------------
void BTO::slot_comm_ethernet_connected()
{
    DBG(3);
    gui()->connected_event(comm()->name());
    gui()->progress_bar_timer.stop();
    gui()->progress_bar_connection_set_max();
    mw()->delay(500);
    gui()->progress_bar_connection_settext_only(tr("The Service Unit powering..."));
    gui()->set_led_icon_active();
    m_initial_start = 1;
    std::list<Adam::Frame> frames = nodeBA()->bto_requests();
    frames.emplace_front(nodeBA()->ReadPartNumber());
    if (comm()->push_requests(frames) == 0) {
        gui()->set_led_icon_inactive();
    }
}

//---------------------------------------------------------
void BTO::slot_comm_ethernet_disconnected()
{
    DBG(1, comm()->name());
    m_initial_start = 0;
    stop_bto_timer();
    gui()->disconnected_event(comm()->error(), comm()->errorString());
}

//---------------------------------------------------------
void BTO::slot_comm_ethernet_canceled()
{
    DBG(1, comm()->name());
    m_initial_start = 0;
    stop_bto_timer();
    gui()->disconnected_event(0, tr("Canceled"));
}

//---------------------------------------------------------
int BTO::gamma_search_procedure()
{
    stop_bto_timer();

    if (m_module_powering_step == GAMMA_STATUS::SEARCH) {
        // поиск модуля - ожидание ответа после включения его питания
        gui()->update_gamma_status(GAMMA_STATUS::SEARCH);
        m_module_powering_step = GAMMA_STATUS::NOT_POWERED_YET;
        auto frames = std::list<Adam::Frame>{
                nodeBA()->ReadPowerOutputSwitch(),
                nodeBA()->ReadPowerInputSwitch(),
                nodeBA()->ReadOutputVoltage(),
                nodeBA()->ReadAmperage(),
        };
        comm()->push_requests(frames);
        return 1;
    }

    if (m_module_powering_step == GAMMA_STATUS::NOT_POWERED_YET) {
        // проверить подачу питания на модуль
        bool ok = nodeBA()->power_output_type() != NodeBA::PowerOutputType::OFF;
        ok &= nodeBA()->power_input_switch() == NodeBA::PowerInputSwitch::ON;
        if (!ok) {
            gui()->update_gamma_status(GAMMA_STATUS::NOT_POWERED);
            push_gamma_power_button(false);
            return 1;
        }
        gui()->update_gamma_status(GAMMA_STATUS::READPARTNUMBER);
    }

    if (m_module_powering_step == GAMMA_STATUS::READPARTNUMBER) {
        // считать номер модуля - если ответит - найден.
        gui()->update_gamma_status(GAMMA_STATUS::ALMOST_FOUND);
        comm()->push_request(NodeE4::ReadPartNumber());
        return 1;
    }

    if (m_module_powering_step == GAMMA_STATUS::ALMOST_FOUND) {
        gui()->update_gamma_status(GAMMA_STATUS::ALMOST_FOUND2);
        std::list<Adam::Frame> frames = nodeBA()->gamma_requests();
        comm()->push_requests(frames);
        return 1;
    }

    if (m_module_powering_step == GAMMA_STATUS::ALMOST_FOUND2) {
        gui()->update_gamma_status(GAMMA_STATUS::FOUND);
        return 2;
    }

    if (m_module_powering_step == GAMMA_STATUS::NOT_POWERED) {
        gui()->set_enabled_gamma_power_button(true);
        gui()->set_gamma_power_button_status(false);
    }

    return 0;
}

//---------------------------------------------------------
void BTO::slot_comm_requests_completed(int nodecode)
{
    if (mw()->current_tab() != MW::PAGES::BTO_PAGE) {
        DBG(3);
        start_bto_timer(BtoPingPeriod);
        return;
    }
    mw()->clearstatus();

    if (!comm()->isConnected()) {
        DBGF(3, "node: %02X", nodecode);
        gui()->requests_completed(nodecode);
        gui()->progress_bar_connection_close();
        gui()->progress_bar_message_close();
        gui()->update_gamma_status(GAMMA_STATUS::NOT_FOUND);
        gamma_search_procedure();
        DBGNL(2);
        return;
    }

    if (DBG::debug_level >= 3) {
        QString s_found;
        QDebug(&s_found) << m_module_powering_step;
        DBG(3, QString("input sw: %1, output sw: %2, output V:%3, gamma: %4")
            .arg(NodeBA::input_switch_name(nodeBA()->power_input_switch()).c_str())
            .arg(NodeBA::power_output_name(nodeBA()->power_output_type()).c_str())
            .arg(nodeBA()->output_voltage(), 0, 'f', 3, QChar('0'))
            .arg(s_found));
    }

    if (m_initial_start == 1) {
        bool ok = nodeBA()->power_output_type() != NodeBA::PowerOutputType::OFF;
        ok &= nodeBA()->power_input_switch() == NodeBA::PowerInputSwitch::ON;
        if (!ok) {
            m_initial_start = 0;
            gui()->progress_bar_connection_close();
            gui()->progress_bar_message_close();
            start_bto_timer();
            return;
        }
        m_initial_start = 0;
        gui()->progress_bar_connection_settext_only(tr("Powering the module"));
        gui()->set_gamma_power_button_status(true);
        gui()->update_gamma_status(GAMMA_STATUS::SEARCH);
    }

    int ret = gamma_search_procedure();
    if (ret != 0) {
        // 0 - статус модуля не изменился
        // 1 - идет процедура поиска модуля
        // 2 - модуль найден
        if (ret == 2) {
            if (m_gamma_module_kind == BTO::MODULE_KIND::ES) {
                m_app->diagn_gui()->set_enabled_button_set_hi_hv(false);
                m_app->diagn_gui()->set_enabled_button_set_lo_hv(false);
            }

            gui()->progress_bar_message_close();
            gui()->progress_bar_connection_close();
            start_bto_timer(0);
        }
        return;
    }

    m_gui->requests_completed(nodecode);
    if (nodecode == NodeE4::CODExE4) {
        start_bto_timer(BtoPollPeriodFast);
    } else {
        start_bto_timer(BtoPollPeriod);
    }
}

//---------------------------------------------------------
void BTO::slot_comm_not_responded(NODECODE nodecode, QString err_string)
{
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET) {
        DBG(1, QString("from  [%1] - '%2'").arg(nodecode, 2, 16, QChar('0')).arg(err_string));

        mw()->clearstatus();
        gui()->progress_bar_connection_close();
        gui()->progress_bar_message_close();
        gui()->progress_bar_apply_setting_close();

        if (nodecode == NodeBA::CODExBA || nodecode == NodeBA::CODEx7A)
        {
            disconnect_from();
            emit mw()->signal_message(QMessageBox::Critical, {}, UTILS::split_message(err_string));
            return;
        }
        start_bto_timer();
        DBGF(3, "bto timer interval: %d, remain: %d",
             m_timer_bto_polling.interval(), m_timer_bto_polling.remainingTime());
    }
}

//---------------------------------------------------------
void BTO::slot_comm_incorrect_resonse(const Adam::Response & response, const QString &msg)
{
    (void)response;
    if (mw()->current_tab() != MW::PAGES::BTO_PAGE) {
        return;
    }
    DBG(1, msg);
    mw()->clearstatus();
    start_bto_timer();
    DBGF(3, "bto timer interval: %d, remain: %d", m_timer_bto_polling.interval(), m_timer_bto_polling.remainingTime());
    emit mw()->signal_message(QMessageBox::Critical, msg);
}

//---------------------------------------------------------
void BTO::slot_comm_connection_error()
{
    int err = comm()->error();
    QString err_string = comm()->errorString();
    if (connection_type() == COMM::CONNECT_TYPE::ETHERNET) {
        DBG(0, QString("%1 connection status: %2 (%3) '%4'")
            .arg(comm()->name())
            .arg(comm()->state_string())
            .arg(err).arg(err_string));
        disconnect_from();
    }
}

//-----------------------------------------------
QDebug operator<<(QDebug d, const BTO::GAMMA_STATUS &status)
{
    if ( status == BTO::GAMMA_STATUS::NOT_POWERED )
        d << QString("NOT_POWERED");
    else if ( status == BTO::GAMMA_STATUS::NOT_FOUND )
        d << QString("NOT_FOUND");
    else if ( status == BTO::GAMMA_STATUS::FOUND )
        d << QString("FOUND");
    else if ( status == BTO::GAMMA_STATUS::SEARCH )
        d << QString("SEARCH");
    return d;
}
