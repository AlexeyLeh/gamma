/*!
  \file   api.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  API-тест гамма-модуля
*/

#include <QtMath>
#include <QtDebug>
#include <QApplication>

#include "api.h"
#include "api_gui.h"
#include "bto.h"
#include "bto_gui.h"
#include "comm.h"
#include "dbg.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeBA.h"
#include "NodeE4.h"
#include "gamma.h"
#include "table.h"
#include "utils.h"

static constexpr int NEXT_STEP = 0;
static constexpr int DONE_STAGE = 1;


API::API(APP *m, API_GUI *gui) :
    m_app(m),
    m_gui(gui),
    m_total_steps(GAMMA::API_Points),
    m_factor_ver(QString::fromStdString(ODID::api_factor_version()))
{
    read_settings(m_app->qSettings());

    int num = ceil((m_settings_odid_end - m_settings_odid_beg) / m_settings_odid_stp);
    m_api_odid_old.reserve(num);
    m_api_odid_new.reserve(num);

    double x = m_settings_odid_beg;
    while (x <= m_settings_odid_end)
    {
        m_api_odid_old.append(new ODID{0, x, 0});
        m_api_odid_new.append(new ODID{1, x, 0});
        x += m_settings_odid_stp;
    }

    m_gui->set_backgr(this);

    connect(comm(), SIGNAL(signal_comm_comport_disconnected()),
            this, SLOT(slot_comm__comport_disconnected()), Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_requests_completed, this, [this](int nodecode) {
        m_gui->requests_completed(nodecode);
    });

    connect(comm(), &COMM::signal_comm_NodeE4_responded,this, [this](byte cmd, byte ext) {
        m_gui->update_fields(cmd, ext);
    });

    connect(comm(), SIGNAL(signal_comm_not_responded(NODECODE, QString)),
            this, SLOT(slot_comm__not_responded(NODECODE, QString)));

    connect(&m_api_timer, SIGNAL(timeout()), this, SLOT(slot_api_test()));
}

//---------------------------------------------------------
API::~API()
{
    DBG(4);
}

//---------------------------------------------------------
void API::update_gui()
{
    QString pn = nodeE4()->partnumber();
    if (pn.isEmpty()) pn = GAMMA::PN_00;
    m_data.toolpn = pn;
    m_data.toolsn = app()->SN();
    m_data.eskdnum = app()->ESKD();
    m_data.oper = app()->Operator();
    gui()->setup_titles(m_data);
    gui()->table1()->resizeColumnToContents(0);
}

//---------------------------------------------------------
const QString API::report_path() const
{
    return QCoreApplication::applicationDirPath() + "/reports";
}

//---------------------------------------------------------
double API::hot_mode_temperature() const
{
    return m_settings_hot_mode_temper;
}

//---------------------------------------------------------
const QVector<ODID*>& API::odid_old() const
{
    return m_api_odid_old;
}

//---------------------------------------------------------
const API::api_data_t &API::data() const
{
    return m_data;
}

//---------------------------------------------------------
API::api_data_t &API::data()
{
    return m_data;
}

//---------------------------------------------------------
int API::data_size() const
{
    return m_data.data.size();
}

//---------------------------------------------------------
const QString &API::factor_version() const {
    return m_factor_ver;
}

//---------------------------------------------------------
void API::set_factor_version(QString str) {
    m_factor_ver = str;
}

//---------------------------------------------------------
const QString API::failed_message() const
{
    return m_data.failed_message;
}

//---------------------------------------------------------
bool API::passed() const
{
    return m_data.failed_message.isEmpty();
}

//---------------------------------------------------------
void API::start_api_timer()
{
    DBG(3);
    m_test_stage = {API::INIT, 0};
    m_api_timer.start(0);
}

//---------------------------------------------------------
void API::stop_api_timer()
{
    DBG(3);
    comm()->clear_requests();
    m_api_timer.stop();
}

//---------------------------------------------------------
void API::slot_comm__comport_disconnected()
{
    if (mw()->current_tab() != MW::PAGES::API_PAGE) return;
    DBG(3, "got signal_comm_comport_disconnected();");
    gui()->stop_api_test();
    gui()->set_stop_label();
}

//---------------------------------------------------------
void API::slot_comm__not_responded(NODECODE node, QString err_string)
{
    if (gui()->mw()->current_tab() != MW::PAGES::API_PAGE) return;
    if (node != NodeE4::CODExE4) return;
    DBG(1, "got signal_comm_not_responded");
    gui()->stop_api_test();
    gui()->set_stop_label();
    emit mw()->signal_message(QMessageBox::Warning, {}, err_string);
}

//---------------------------------------------------------
int API::init_test()
{
    // Подготовка к тестированию
    gui()->showstatus("");
    if (m_test_stage.step == 0)
    {
        DBGF(1, "stage[%d/%d]: Getting Tool Hardware Settings",
             m_test_stage.stage, m_test_stage.step);

        gui()->clear_fields();
        gui()->progress_bar_gamma()->reset();
        gui()->set_init_label();
        m_data.clear();

        // очистка полей с результатами
        m_api_odid_data_valid = false;
        for (auto *d : m_api_odid_old) {
            d->m_factor = 0.0;
        }

        for (auto *d : m_api_odid_new) {
            d->m_factor = 0.0;
        }

        update_odid_table();

        if (simulation()) {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            // якобы считаетсся из модуля
            nodeE4()->m_temperature_value = 29.3;
            nodeE4()->m_operating_hv = 1400;
            nodeE4()->m_gamma_acq_time = 2.5; // sec
            nodeE4()->m_warmup_time = 1;
            nodeE4()->m_partnumber = "77557ES000";

            gui()->update_fields(0x05);
            gui()->update_fields(0x43);
            gui()->update_fields(0x13);
            gui()->update_fields(0x46);
            gui()->update_fields(0xfc);

            m_test_stage.step = 1;
            m_timer_delay = 0;
            return NEXT_STEP;
        }

        gui()->showstatus(tr("Getting Tool Hardware Settings"));
        app()->bto_gui()->progress_bar_message_show(tr("Getting Tool Hardware Settings"));

        // запрос уставок
        gui()->set_led_icon_active();
        auto requests = std::list<Adam::Frame> {
                        NodeE4::ReadTemperature(),              // 0x05
                        NodeE4::ReadOperationVoltage(),         // 0x43
                        NodeE4::ReadGammaRayAcqusitionTime(),   // 0x13
                        NodeE4::ReadHVWarmUp(),                 // 0x46
                        NodeE4::ReadPartNumber(),               // 0xfc
        };
        int rtime = comm()->push_requests(requests);
        m_timer_delay = (rtime + 500);
        m_test_stage.step = 1;
        m_api_mode = API::api_mode_t::API_0;
        DBG(1, QString("waiting for module settings for %1ms").arg(m_timer_delay));
        return NEXT_STEP;
    }

    // получение уставок и измеренных значений
    if (m_test_stage.step == 1)
    {
        m_current_step = 0;

        // 1.
        // m_warmup_time - время ожидания для стабилизации измерений в гамма-модуле,
        // нужно, чтобы подождать, чтобы напряжение "устаканилось"
        // время из прошивки m_e4_warmup_time используетсчя самим модулем для внутренних алгоритмов!
        if (m_settings_warmup_time > 0) {
            m_warmup_period = m_settings_warmup_time; // установка из .ini
        } else {
            m_warmup_period = std::max(30, m_warmup_period);
        }

        // 2.
        // m_gamma_acq_time - время одного измерения гамма-отсчетов в модуле (из прошивки)
        // нужно, чтобы подождать, пока устройство будет подсчитывать гамма-отсчеты
        if (m_settings_acquire_time > 0) {
            m_acq_one_gamma = m_settings_acquire_time; // ручная установка из .ini
        } else {
            m_acq_one_gamma = int(ceil(nodeE4()->m_gamma_acq_time * m_settings_gamma_acq_bucket));
        }
        m_acq_one_gamma = std::max(m_acq_one_gamma, ceil(2.5 * (double)m_settings_gamma_acq_bucket));

        // 3.
        // итоговое время в программе
        // по накоплению и расчету среднего значения гамма-отсчетов для одного измерения
        // измерение в модуле * кол-во измерений + кол-во изм * сек на каждое считывание из модуля
        m_acq_sample_time = m_acq_one_gamma * m_settings_samples_for_avrg + m_settings_samples_for_avrg;
        m_acq_total_time = m_warmup_period + m_acq_sample_time * m_total_steps;

        // 4.
        m_data.date = QDateTime::currentDateTime();
        m_data.toolname = tr("GAMMA SONDE");
        m_data.toolpn = nodeE4()->partnumber();
        m_data.toolsn = app()->SN();
        m_data.eskdnum = app()->ESKD();
        m_data.oper = app()->Operator();
        m_data.acqtime = m_acq_one_gamma;
        m_data.operating_hv = nodeE4()->hv();
        m_data.settings_calibr_min = m_settings_calibr_min;
        m_data.settings_calibr_max = m_settings_calibr_max;
        m_data.settings_calibr_old = m_settings_calibr_old;
        m_data.settings_calibr_new = m_settings_calibr_new;
        m_data.settings_tol_deviat0_min   = m_settings_tol_deviat0_min;
        m_data.settings_tol_deviat0_max   = m_settings_tol_deviat0_max;
        m_data.settings_tol_deviat200_min = m_settings_tol_deviat200_min;
        m_data.settings_tol_deviat200_max = m_settings_tol_deviat200_max;

        // 5.
        int remain = m_acq_sample_time * (m_total_steps - m_current_step);
        gui()->update_field_estimated_time(remain, m_acq_total_time);
        gui()->update_field_acquire_time(m_acq_one_gamma, m_settings_samples_for_avrg, m_acq_total_time);
        gui()->update_field_steps__reset(m_total_steps);

        // 6.
        DBG(1, QString("Got operating HV: %1").arg(nodeE4()->m_operating_hv));
        DBGF(1, "Temperature: %0.2f", nodeE4()->m_temperature_value);
        if (nodeE4()->m_temperature_value < m_settings_hot_mode_temper) {
            m_api_hot_mode = API::COLD;
            gui()->update_field_test_mode(tr("COLD"));
        }
        else
        {
            m_api_hot_mode = API::HOT;
            gui()->update_field_test_mode(tr("HOT"));
        }

        if (m_settings_warmup_time > 0) {
            DBGF(1, "WarmUp time from .ini: %d sec", m_warmup_period);
        } else {
            DBGF(1, "WarmUp time in .ini is zero - will be used %d sec", m_warmup_period);
        }
        DBGF(1, "Acq time (from gamma): %0.2f", m_acq_one_gamma);
        DBGF(1, "Acq time with %d bucket: %0.2f, rounded to %d sec",
             m_settings_gamma_acq_bucket, m_acq_one_gamma * m_settings_gamma_acq_bucket, (int)m_acq_one_gamma);
        DBGF(1, "Acquire time for one API: (%d x %d) + %d = %d sec",
             (int)m_acq_one_gamma, m_settings_samples_for_avrg, m_settings_samples_for_avrg, m_acq_total_time);
        DBG(1, QString("Total acquire time: %1 sec").arg(m_acq_total_time));

        m_timer_delay = 100;
        m_test_stage.step = 2;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 2)
    {
        if (simulation()) {
            m_test_stage = {API::SIMULATION, 0};
            m_timer_delay = 0;
            return NEXT_STEP;
        }
        m_app->bto_gui()->progress_bar_message_close();
        return DONE_STAGE;
    }

    return DONE_STAGE;
}

//---------------------------------------------------------
int API::set_voltage()
{
    // Установить высоковольтное напряжение
    if (m_test_stage.step == 0)
    {
        int hv = nodeE4()->m_operating_hv;
        DBG(1, QString("HV of test: %1").arg(hv));
        gui()->showstatus(QString("[%1 %2] %3").arg(hv).arg(tr("V"), tr("Set HV")));

        if (hv < m_settings_HV_min_value) {
            auto msg = QString("%1\n\n* %2 %3\n* %4 %5/%6/%7\n\n%8")
                       .arg(tr("Incorrect High voltage settings in a tool"))
                       .arg(tr("Operating voltage level -"))
                       .arg(nodeE4()->m_operating_hv)
                       .arg(tr("Calibrated HV values (Min/Max/Test) -"))
                       .arg(nodeE4()->m_calibr_hv_min)
                       .arg(nodeE4()->m_calibr_hv_max)
                       .arg(nodeE4()->m_calibr_hv_tst)
                       .arg(tr("Test stopped"));
            gui()->display_error_message(msg, tr("Test stopped"));
            m_test_stage = {API::CANCEL, 0}; // отмена тестирования
            m_timer_delay = 0;
            return NEXT_STEP;
        }

        gui()->set_led_icon_active();
        int rtime = comm()->push_request(NodeE4::WriteOperationVoltage(hv));
        m_timer_delay = (rtime + 100);
        m_test_stage.step = 1;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 1)
    {
        gui()->set_led_icon_active();
        int rtime = comm()->push_request(NodeE4::ReadOperationVoltage());
        m_timer_delay = (rtime + 100);
        m_test_stage.step = 2;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 2)
    {
        return DONE_STAGE;
    }

    return DONE_STAGE;
}

//---------------------------------------------------------
int API::warmup()
{
    // Выдержка паузы для "прогрева" устройства
    if (m_test_stage.step == 0)
    {
        // подготовка
        DBGF(1, "stage[%d/%d]: sample: %d, warmup period: %d (bar:%d/%d)",
             m_test_stage.stage, m_test_stage.step,
             m_current_step, m_warmup_period, m_progress_bar_index, gui()->progress_bar_gamma()->maximum());

        m_progress_bar_index = 0;
        gui()->progress_bar_gamma()->reset();
        gui()->progress_bar_gamma()->setFormat("%v/%m");
        gui()->progress_bar_gamma()->setRange(0, m_warmup_period);
        auto style = "QProgressBar {/*height: 5px;*/} QProgressBar::chunk {background: #eb8000;}"; // оранж
        gui()->progress_bar_gamma()->setStyleSheet(style);
        gui()->progress_bar_gamma()->setValue(0);

        //// nodeE4()->gamma_accumulation_turnOn();  // тут - чтобы обнулить накопленные ранее значения
        //// nodeE4()->gamma_accumulation_turnOff();
        m_api_warmup_counter = 0; // sec
        m_test_stage.step = 1;
    }

    if (m_test_stage.step == 1)
    {
        // ожидание "стабилизации напряжения"
        if (m_api_warmup_counter < m_warmup_period)
        {
            gui()->showstatus(QString("[%1 %2] %3 [%4/%5 %6]")
                              .arg(nodeE4()->m_operating_hv)
                              .arg(tr("V"), tr("High Voltage Stabilization"))
                              .arg(m_api_warmup_counter)
                              .arg(m_warmup_period)
                              .arg(tr("sec")));
            gui()->progress_bar_gamma()->setValue(++m_progress_bar_index);
            gui()->set_led_icon_active();
            auto rtime = comm()->push_requests( { NodeE4::ReadOperationVoltage(),    // 0x43
                                                  NodeE4::ReadGammaCPS(1) });        // 0x01
            ++m_api_warmup_counter;
            m_timer_delay = 1000 + rtime + m_acq_one_gamma;
            return NEXT_STEP;
        }

        gui()->set_led_icon_active();
        auto rtime = comm()->push_request(NodeE4::ReadGammaCPS(1)); // 0x01
        m_timer_delay = rtime + m_acq_one_gamma;
        ++m_test_stage.step;
        return NEXT_STEP;
    }

    return DONE_STAGE;
}

//---------------------------------------------------------
int API::read_gamma()
{
    if (m_test_stage.step == 0)
    {
        DBGF(1, "stage[%d/%d]: sample: %d hv: %dV",
             m_test_stage.stage, m_test_stage.step,
             m_current_step, nodeE4()->m_operating_hv);
        //// nodeE4()->gamma_accumulation_turnOn(); // включается режим накопления

        int remain = m_acq_sample_time * (m_total_steps - m_current_step);
        gui()->update_field_estimated_time(remain, m_acq_total_time);

        m_waiting_for_acqure = 0;
        m_acq_counter = 0;

        m_progress_bar_index = 0;
        gui()->progress_bar_gamma()->reset();
        gui()->progress_bar_gamma()->setFormat("%v/%m");
        gui()->progress_bar_gamma()->setRange(0, m_acq_sample_time);
        auto style = "QProgressBar {/*height: 5px;*/} QProgressBar::chunk {background: #7D94B0;}"; // серо-синий
        gui()->progress_bar_gamma()->setStyleSheet(style);
        gui()->progress_bar_gamma()->setValue(0);

        m_test_stage.step = 1;
        m_timer_delay = 0;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 1)
    {
        gui()->progress_bar_gamma()->setValue(++m_progress_bar_index);

        // задержка на время считывания гамма-отсчета
        if (m_waiting_for_acqure < m_acq_sample_time)
        {
            gui()->showstatus(QString("[%1 %2] %3 [%4]")
                              .arg(nodeE4()->m_operating_hv)
                              .arg(tr("V"), tr("Waiting to acquire one gamma sample"))
                              .arg(UTILS::secs_to_minute(m_acq_sample_time - m_waiting_for_acqure)));

            ++m_waiting_for_acqure;

            bool status = temperature_control();
            if (status == false)
            {
                m_timer_delay = 0;
                m_test_stage = {API::STOP, 0}; // не-нормальное завершение
                return NEXT_STEP;
            }

            gui()->set_led_icon_active();
            comm()->push_requests({ NodeE4::ReadTemperature(), // 0x05
                                    NodeE4::ReadOperationVoltage(),     // 0x43
                                  });

            m_test_stage.step = 1;
            m_timer_delay = 1000;
            return NEXT_STEP;
        }
        m_waiting_for_acqure = 0;
        gui()->showstatus(QString("[%1 %2] %3")
                          .arg(nodeE4()->m_operating_hv)
                          .arg(tr("V"), tr("Getting one gamma sample")));

        // считывание гамма-отсчета
        gui()->set_led_icon_active();
        int rtime = comm()->push_requests({ NodeE4::ReadGammaCPS(byte(m_settings_gamma_acq_bucket)), // 0x01
                                            NodeE4::ReadTemperature(),      // 0x05
                                            NodeE4::ReadOperationVoltage(), // 0x43
                                          });
        m_test_stage.step = 3;
        m_timer_delay = (rtime + 100);
        return NEXT_STEP;
    }

    if (m_test_stage.step == 3)
    {
        DBGF(1, "stage[%d/%d]: %0.2f", m_test_stage.stage, m_test_stage.step, nodeE4()->m_gamma_value);

        ++m_acq_counter;
        if (m_acq_counter < m_settings_samples_for_avrg)
        {
            m_test_stage.step = 1;     // продолжить считывание гаммы - для накопления, для расчета среднего
            m_timer_delay = 0;
            return NEXT_STEP;
        }

        m_acq_counter = 0;
        m_test_stage.step = 4;         // закончили считывание гамма-отсчетов
        m_timer_delay = 0;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 4)
    {
        // закончили накапливание отсчетов гаммы
        //// nodeE4()->gamma_accumulation_turnOff();
        m_test_stage.step = 5;
        m_timer_delay = 0;
        return NEXT_STEP;
    }

    if (m_test_stage.step == 5)
    {
        // расчеты
        remember_CPS();
        update_table();
        calculate_cps_average();
        return DONE_STAGE;
    }

    return DONE_STAGE;
}

//---------------------------------------------------------
bool API::temperature_control()
{
    QString str1, str2;
    auto value = nodeE4()->m_temperature_value;
    if (m_api_hot_mode == API::COLD)
    {
        if (value > m_settings_hot_mode_temper)
        {
            str1 = QString("%1 - %2")
                   .arg(tr("Test Mode")).arg(tr("COLD MODE"));
            str2 = QString("%1°C > %2°C")
                   .arg(value, 0, 'f', 1)
                   .arg(m_settings_hot_mode_temper);

        }
    }
    else if (m_api_hot_mode == API::HOT)
    {
        if (value < m_settings_hot_mode_temper)
        {
            str1 = QString("%1 - %2")
                   .arg(tr("Test Mode"), tr("HOT MODE"));
            str2 = QString("%1°C < %2°C")
                   .arg(value, 0, 'f', 1)
                   .arg(m_settings_hot_mode_temper);
        }
    }

    if (!str1.isEmpty())
    {
        auto msg = QString("%1\n\n%2\n%3\n\n%4")
                   .arg(tr("WRONG TEMPERATURE MODE"), str1, str2, tr("Test stopped"));
        gui()->display_error_message(msg, tr("Test stopped"));
        return false;
    }

    return true;
}

//---------------------------------------------------------
void API::remember_CPS()
{
    // запоминание значений
    auto dt         = QDateTime::currentDateTime();
    auto value_hv   = nodeE4()->m_operating_hv;
    auto value_temp = nodeE4()->m_temperature_value;
    auto value_cps  = nodeE4()->m_gamma_value;

    m_data.date = dt;
    if (m_api_mode == API::api_mode_t::API_0)
    {
        m_data.data.append({dt, value_hv, value_cps, value_temp, {}, 0, 0, 0});
    }
    else // API::api_mode_t::API_200
    {
        int idx = m_current_step;
        m_data.data[idx].dt2     = dt;
        m_data.data[idx].hv2     = value_hv;
        m_data.data[idx].cps2    = value_cps;
        m_data.data[idx].temper2 = value_temp;
    }
}

//---------------------------------------------------------
void API::calculate_cps_average()
{
    int num = m_current_step + 1;
    double total_cps = 0.0;
    double total_temper = 0.0;
    double avrg;
    if (m_api_mode == API::api_mode_t::API_0)
    {
        for (int idx = 0; idx < num; ++idx)
        {
            total_cps += data().data[idx].cps1;
            total_temper += data().data[idx].temper1;
        }
        avrg = total_cps / num;
        m_data.avrg_CPS0 = avrg;
        gui()->table1()->set_field_text(1, 1, QString::number(data().avrg_CPS0, 'f', 2));
        gui()->table1()->set_field_text(1, 2, QString::number(total_temper/num, 'f', 2));
    }
    else // API::api_mode_t::API_200
    {
        for (int idx = 0; idx < num; ++idx)
        {
            total_cps += data().data[idx].cps2;
            total_temper += data().data[idx].temper2;
        }
        avrg = total_cps / num;
        m_data.avrg_CPS200 = avrg;
        gui()->table1()->set_field_text(1, 3, QString::number(data().avrg_CPS200, 'f', 2));
        gui()->table1()->set_field_text(1, 4, QString::number(total_temper/num, 'f', 2));
    }

    DBG(1, QString("sample: %1\n").arg(avrg, 0, 'f', 2));
}

//---------------------------------------------------------
bool API::calculations()
{
    gui()->showstatus(tr("Calculations..."), 10);
    bool status2 = false;
    bool status1 = calculate_calibration_factors();
    if (status1) {
        status2 = calculate_std_deviation();
    }
    DBGF(3, "%s | %s = %s",
         status1 ? "ok" : "fail",
         status2 ? "ok" : "fail",
         (status1 | status2) ? "ok" : "fail");
    return status1 | status2;
}

//---------------------------------------------------------
bool API::calculate_calibration_factors()
{
    // 1.
    m_data.failed_message.clear();
    if (m_data.avrg_CPS0 < 10 || m_data.avrg_CPS200 < 10) {
        DBG(1, QString("too low value(s) - avrgCPS1: %1 avrgCPS2: %2")
            .arg(m_data.avrg_CPS0, m_data.avrg_CPS200, 0, 'f', 2));

        m_data.failed_message = QString("%1\n\n%2 - %3\n%4 - %5")
                                    .arg(tr("Too low average of CPS(s)"))
                                    .arg(tr("Blank source"))
                                    .arg(m_data.avrg_CPS0)
                                    .arg(tr("Gamma source"))
                                    .arg(m_data.avrg_CPS200);
        return false;
    }

    // 2.
    m_data.calibr_factor_old = m_data.settings_calibr_old / (m_data.avrg_CPS200 - m_data.avrg_CPS0);
    m_data.calibr_factor_new = m_data.settings_calibr_new / (m_data.avrg_CPS200 - m_data.avrg_CPS0);

    DBGF(1, "calibration factors - old: %0.2f, new: %0.2f\n",
         m_data.calibr_factor_old, m_data.calibr_factor_new);

    bool status = true;
    QString errmsg = QString("<html><body>\n<div style='text-align:center;'>\n%1\n<table align='center'>\n")
                     .arg(tr("Calibration factor out of tolerance"));

    if (m_data.calibr_factor_old < m_data.settings_calibr_min || m_data.calibr_factor_old > m_data.settings_calibr_max) {
        status = false;
        errmsg += QString("<tr><td>%1</td><td>\u00A0 \u00A0</td><td>%2</td></tr>\n")
                  .arg(tr("OLD Method"))
                  .arg(m_data.calibr_factor_old);
    }

    if (m_data.calibr_factor_new < m_data.settings_calibr_min || m_data.calibr_factor_new > m_data.settings_calibr_max) {
        status = false;
        errmsg += QString("<tr><td>%1</td><td>\u00A0 \u00A0</td><td>%2</td></tr>\n")
                  .arg(tr("NEW Method"))
                  .arg(m_data.calibr_factor_new);
    }

    errmsg += "</table>\n\n</div>\n</body></html>";

    // 5.
    gui()->update_field__calibration_factor(status);

    if (!status) {
        DBG(1, errmsg);
        m_data.failed_message += errmsg;
    }
    return status;
}

//---------------------------------------------------------
bool API::calculate_std_deviation()
{
    // 1.
    QString errmsg;
    bool ok = true;
    const auto &d = data();
    int num = d.data.size();
    m_data.failed_message.clear();

    for (int api_mode : {0, 200}) {
        double sum = 0.0;
        for (int idx = 0; idx < num; ++idx) {
            char buf[256] = {0};
            double v;
            if (api_mode == 0) {
                v = d.avrg_CPS0 - d.data[idx].cps1;
                snprintf(buf, sizeof(buf), "%10.6f - %10.6f = %10.6f, v*v: %10.6f",
                         d.avrg_CPS0, d.data[idx].cps1, v, v*v);
            }
            else
            {
                v = d.avrg_CPS200 - d.data[idx].cps2;
                snprintf(buf, sizeof(buf), "%10.6f - %10.6f = %10.6f, v*v: %10.6f",
                         d.avrg_CPS200, d.data[idx].cps2, v, v*v);
            }
            sum += v * v;
            DBG(1, QString("api mode: %1 - %2, sum: %3").arg(api_mode).arg(buf).arg(sum, 10, 'f', 6));
        }
        auto avrg = sum / num;
        double stddev = sqrt(avrg);
        DBG(2, QString("api mode: %1 - stddev: avrg(%2) = %3").arg(api_mode).arg(avrg, 0, 'f', 6).arg(stddev, 0, 'f', 6));

        // 2.
        if (api_mode == 0) {
            m_data.std_deviation_0 = stddev;
            if (d.std_deviation_0 < d.settings_tol_deviat0_min || d.std_deviation_0 > d.settings_tol_deviat0_max)
            {
                ok = false;
                if (errmsg.isEmpty()) {
                    errmsg = QString("<br><html><body>%1<table>").arg(tr("Standard deviation is out of tolerance"));
                }
                errmsg += QString("<tr><td>%</td></tr><td>%2</td></tr>")
                          .arg(tr("0 API")).arg(d.std_deviation_0);
                DBGF(1, "standard deviations [0 API]: %0.2f", d.std_deviation_0);
            }
        }
        else
        {
            m_data.std_deviation_200 = stddev;
            if (d.std_deviation_200 < d.settings_tol_deviat200_min || d.std_deviation_200 > d.settings_tol_deviat200_max)
            {
                ok = false;
                if (errmsg.isEmpty()) {
                    errmsg = QString("<br><html><body>%1").arg(tr("Standard deviation is out of tolerance"));
                }
                errmsg += QString("<tr><td>%</td></tr><td>%2</td></tr>")
                          .arg(tr("200 API")).arg(d.std_deviation_200);
                DBGF(1, "standard deviations [200 API]: %0.2f", d.std_deviation_200);
            }
        }
    }
    if (!ok) {
        errmsg += QString("</table></body></html>");
    }

    // 4.
    gui()->update_field__standard_deviation(ok);
    if (!ok) {
        m_data.failed_message += errmsg;
    }
    return ok;
}

//---------------------------------------------------------
void API::calculate_odid_table()
{
    // odid - коэф. для разных толщин стенок трубы (внешний диаметр минус внутреннй)
    int i = 0;
    double x = m_settings_odid_beg;
    while (x <= m_settings_odid_end)
    {
        double value = m_data.calibr_factor_old * ODID::calculate_api_old_method(x);
        m_api_odid_old[i]->m_factor = value;

        value = m_data.calibr_factor_new * ODID::calculate_api_new_method(x);
        m_api_odid_new[i]->m_factor = value;

        x += m_settings_odid_stp;
        ++i;
    }
    m_api_odid_data_valid = true;
}

//---------------------------------------------------------
void API::update_odid_table()
{
    // таблица коэф-ов для разных значений толщины стенок трубы (внешний диаметр минус внутреннй)

    if (!m_api_odid_data_valid) {
        return;
    }

    const QVector<ODID*>* odid_data = &m_api_odid_old;
    if (gui()->is_selected_new_table()) {
        odid_data = &m_api_odid_new;
    }

    if (odid_data->isEmpty()) {
        return;
    }

    DBG(2, QString("%1[0]: %2 %3")
        .arg(gui()->is_selected_new_table() ? "NEW" : "OLD")
        .arg(odid_data->at(0)->m_odid, 0, 'f', 3)
        .arg(odid_data->at(0)->m_factor, 0, 'f', 3));

    auto *table = gui()->table2();
    if (!table) {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    table->model()->blockSignals(true);
    table->blockSignals(true);
    auto sorts = table->isSortingEnabled();
    if (sorts) {
        table->setSortingEnabled(false);
    }

    int nrow = 0;
    int ncol = 0;
    int max_rows = table->rowCount();
    int max_cols = table->columnCount();
    for (const auto *odid : *odid_data) {
        if (nrow >= max_rows) {
            nrow = 0;
            ncol += 2;
        }
        if (ncol + 1 >= max_cols) {
            break;
        }

        auto text = QString::number(odid->m_odid, 'f', 3);
        auto *item = table->item(nrow, ncol + 0);
        if (item) {
            item->setText(text);
            item->setToolTip(text);
        }

        text = QString::number(odid->m_factor, 'f', 3);
        item = table->item(nrow, ncol + 1);
        if (item) {
            item->setText(text);
            item->setToolTip(text);
        }
        ++nrow;
    }

    table->model()->blockSignals(false);
    table->blockSignals(false);
    table->setSortingEnabled(sorts);
    table->doItemsLayout();

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------
void API::update_table()
{
    int idx = m_current_step;
    int nrow = idx + 2;
    if (idx < 0 || idx >= data().data.size()) {
        return;
    }

    const auto &data = this->data().data[idx];
    if (m_api_mode == API::api_mode_t::API_0)
    {
        auto cps    = data.cps1;
        auto temper = data.temper1;
        gui()->table1()->set_field_text(nrow, 1, QString::number(cps, 'f', 2));
        gui()->table1()->set_field_text(nrow, 2, QString::number(temper, 'f', 2));
    }
    else // API::api_mode_t::API_200
    {
        auto cps    = data.cps2;
        auto temper = data.temper2;
        gui()->table1()->set_field_text(nrow, 3, QString::number(cps, 'f', 2));
        gui()->table1()->set_field_text(nrow, 4, QString::number(temper, 'f', 2));
    }
    gui()->table1()->set_field_alignment(nrow, 0, Qt::AlignCenter);
    gui()->table1()->set_field_alignment(nrow, 1, Qt::AlignCenter);
    gui()->table1()->set_field_alignment(nrow, 2, Qt::AlignCenter);
    gui()->table1()->set_field_alignment(nrow, 3, Qt::AlignCenter);
    gui()->table1()->set_field_alignment(nrow, 4, Qt::AlignCenter);
}

//---------------------------------------------------------
void API::slot_api_test()
{
    qint64 msec = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    qint64 passed_ms = (msec - m_prev_msec);

    if (passed_ms < m_timer_delay) {
        return;
    }

    m_timer_delay = 1000; // default

    if (comm()->number_of_requests() > 0) {
        DBG(3);
        return;
    }

    if (m_test_stage.stage == 0 && (!simulation() && !comm()->isConnected())) {
        m_prev_msec = QDateTime::currentDateTime().currentMSecsSinceEpoch();
        return;
    }

    if (!app()->simulation()) {
        if (!comm()->isConnected()) {
            if (m_test_stage.stage == 0) {
                DBG(1, "--- device is not connected");
                QApplication::restoreOverrideCursor();
                m_test_stage = {API::STOP, 0};
                m_timer_delay = 0;
                m_prev_msec = QDateTime::currentMSecsSinceEpoch();
                return;
            }
        }
    }

    switch (m_test_stage.stage)
    {
        case API::INIT:
        {
            auto done = init_test();
            if (done)
            {
                m_test_stage = {API::TYPE, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case API::TYPE:
        {
            auto ok = gui()->message_about_selected_api_source(m_api_mode);
            if (!ok)
            {
                m_test_stage = {API::CANCEL, 0}; // отменено
                m_timer_delay = 0;
                break;
            }

            if (m_api_mode == API::api_mode_t::API_0)
            {
                m_test_stage = {API::SET_VOLTAGE, 0};
            }
            else // API::api_mode_t::API_200
            {
                m_test_stage = {API::WARMUP, 0};
            }
            gui()->set_run_label();

            m_current_step = 0;
            gui()->update_field_steps__reset(m_total_steps);
            gui()->progress_bar_gamma()->reset();

            m_timer_delay = 0;
            break;
        }

        case API::SET_VOLTAGE:
        {
            auto done = set_voltage();
            if (done)
            {
                m_test_stage = {API::WARMUP, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case API::WARMUP:
        {
            auto done = warmup();
            if (done)
            {
                m_test_stage = {API::READ_GAMMA, 0};
                m_timer_delay = 0;
            }
            break;
        }

        case API::READ_GAMMA:
        {

            gui()->update_field_steps__done(m_current_step + 1, m_current_step, m_total_steps);
            auto done = read_gamma();
            if (done)
            {
                ++m_current_step;
                if (m_current_step < m_total_steps)
                {
                    // переход на чтение следующей порции CPS для расчета среднего значения
                    m_test_stage = {API::READ_GAMMA, 0};
                    m_timer_delay = 0;
                    break;
                }
                else
                {
                    gui()->update_field_steps__done(m_current_step, m_current_step, m_total_steps);
                    if (m_api_mode == API::api_mode_t::API_0)
                    {
                        // закончили c 0 API - переходим к 200 API
                        m_api_mode = API::api_mode_t::API_200;
                        m_test_stage = {API::TYPE, 0};
                        m_timer_delay = 0;
                        break;
                    }

                    // закончили и с 200 API
                    m_test_stage = {API::CALCULATIONS, 0};
                    m_timer_delay = 0;
                    break;
                }
            }
            break;
        }

        case API::SIMULATION:
        {
            do_simulation();
            break;
        }

        case API::CALCULATIONS:  // расчеты
        {
            DBGF(1, "stage[%d/%d]: finishing test", m_test_stage.stage, m_test_stage.step);
            QApplication::setOverrideCursor(Qt::WaitCursor);
            gui()->stop_api_test();
            calculations();
            calculate_odid_table();
            update_odid_table();
            gui()->finish_message();
            protocol_create_last_log();
            QApplication::restoreOverrideCursor();
            break;
        }

        case API::CANCEL:  // простая отмена тестирования
        {
            DBGF(1, "stage[%d/%d]: cancel test", m_test_stage.stage, m_test_stage.step);
            gui()->stop_api_test();
            gui()->finish_message(false);
            QApplication::restoreOverrideCursor();
            break;
        }

        case API::STOP:  // не-нормальное завершение
        {
            DBGF(1, "stage[%d/%d]: stop test", m_test_stage.stage, m_test_stage.step);
            QApplication::setOverrideCursor(Qt::WaitCursor);
            gui()->stop_api_test();
            calculations();
            calculate_odid_table();
            update_odid_table();
            gui()->finish_message();
            protocol_create_last_log();
            QApplication::restoreOverrideCursor();
            break;
        }
    }

    m_prev_msec = QDateTime::currentDateTime().currentMSecsSinceEpoch();
}

