/*!
  \file   fw.cpp
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Работа с гамма-модулем
*/

//#include <QtDebug>

#include "comm.h"
#include "fw.h"
#include "fw_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "utils.h"


//---------------------------------------------------------
FIRMWARE::FIRMWARE(APP *m, FIRMWARE_GUI *gui) :
    m_app(m),
    m_gui(gui),
    m_scanning_time(1000)
{
    m_gui->set_backgr(this);
    read_settings(m_app->qSettings());

    connect(comm(), SIGNAL(signal_comm_comport_disconnected()),
            this, SLOT(slot_comm__comport_disconnected()), Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_requests_completed, this, [gui = m_gui](int nodecode) {
        if (gui->mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) {
            return;
        }
        DBGF(3, "node: %02X - got signal_comm_requests_completed()", nodecode);
        gui->requests_completed(nodecode);
    }, Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_NodeE4_responded, this, [gui = m_gui](byte cmd, byte ext) {
        if (gui->mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) {
            return;
        }
        DBG(3, "got signal_comm_NodeE4_responded()");
        gui->update_fields(cmd, ext);
    }, Qt::QueuedConnection);

    qRegisterMetaType<Adam::Response>("Adam::Response");
    connect(comm(), &COMM::signal_comm_incorrect_resonse, this,
            [=](const Adam::Response &response, const QString &msg) {

        if (m_app->mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) {
            return;
        }

        if (DBG::debug_level >= 3) {
            QString dump = QString::fromStdString(response.dump());
            auto str = QString("%1 [%2] %3 [%4] %5")
                    .arg(tr("From"))
                    .arg(response.from(), 0, 16)
                    .arg(tr("To"))
                    .arg(response.to(), 0, 16)
                    .arg(dump);
            DBG(3, QString("got signal_comm_incorrect_resonse(): %1").arg(str));
        }

        int ack = (int)response.ack();
        int status = (int)response.status();
        int cmd = (int)response.cmd();

        m_gui->set_ACK_field_code(QString("%1").arg(ack, 2, 16, QChar('0')).toUpper());
        m_gui->set_ACK_field_status(QString("%1").arg(status, 4, 16, QChar('0')).toUpper());
        m_gui->set_ACK_field_cmd(QString("%1").arg(cmd, 2, 16, QChar('0')).toUpper(), NodeBA::frame_name(cmd));
        auto desc = NodeE4::status_description(ack, static_cast<NodeE4::Status>(status));
        auto tip = NodeE4::status_tip(ack, static_cast<NodeE4::Status>(status));
        m_gui->set_ACK_field_text(desc, tip);

        emit m_app->mw()->signal_message(QMessageBox::Critical, msg);

    }, Qt::QueuedConnection);

    connect(comm(), SIGNAL(signal_comm_not_responded(NODECODE, QString)),
            this, SLOT(slot_comm__not_responded(NODECODE, QString)));

    m_fw_requests =
            std::list<Adam::Frame> {
            NodeE4::ReadPartNumber(),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Short),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Long),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Short),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Long),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Short),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Long),
            NodeE4::ReadGammaCPS(),
            NodeE4::ReadTemperature(),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc15),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc5),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Dv485),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::HighVoltageControl),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensor),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensorCalibr),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc25Reference),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::PowerSupplyAmperage),
            NodeE4::ReadCtable(),
            NodeE4::ReadKtable(),
            NodeE4::ReadLocalGravity(),
            NodeE4::ReadConfigRegister(),
            NodeE4::ReadHVWarmUp(),
            NodeE4::ReadGammaRayAcqusitionTime(),
            NodeE4::ReadOperationVoltage(),
            NodeE4::ReadCalibrationVoltages(),
            // NodeE4::ReadMasterStatusRegister(), // not supported
        };
}

//---------------------------------------------------------
FIRMWARE::~FIRMWARE()
{
    DBG(4);
}

//---------------------------------------------------------
void FIRMWARE::update_gui()
{
    gui()->refresh_ctable();
    gui()->refresh_ktable();
}

//---------------------------------------------------------
CTable *FIRMWARE::ctable() const
{
    return nodeE4()->m_ctable;
}

//---------------------------------------------------------
KTable *FIRMWARE::ktable() const
{
    return nodeE4()->m_ktable;
}

//---------------------------------------------------------
void FIRMWARE::make_request(const Adam::Frame& frame) const
{
    comm()->push_request(frame);
}

//---------------------------------------------------------
void FIRMWARE::slot_comm__comport_disconnected()
{
    if (mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) return;
    DBG(3, "got signal_comm_comport_disconnected();");
    gui()->showstatus({});
    gui()->firmware_read_finished();
}

//---------------------------------------------------------
void FIRMWARE::slot_comm__not_responded(NODECODE nodeCode, QString err_string)
{
    if (mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE) {
        return;
    }
    if (nodeCode != NodeE4::CODExE4) {
        return;
    }
    DBGF(1, "node: %02X", nodeCode);
    gui()->showstatus(UTILS::split_message(err_string));

    //push_gamma_power_button(false);
    //start_bto_timer(BtoPollPeriodFast);

    DBG(3, "emit signal__message(QMessageBox::Critical, err_string)");
    emit mw()->signal_message(QMessageBox::Warning, {}, err_string);
    gui()->firmware_read_finished();
}

//---------------------------------------------------------
int FIRMWARE::polling_time()
{
    return m_scanning_time / 1000; // sec
}

//---------------------------------------------------------
void FIRMWARE::set_polling_time(int sec)
{
    m_scanning_time = sec * 1000;
}

//---------------------------------------------------------
void FIRMWARE::start_read_firmware()
{
    DBG(3);
    nodeE4()->clear();
    comm()->push_requests(m_fw_requests);
}

//// //---------------------------------------------------------
//// void FIRMWARE::cancel_read()
//// {
////     DBG(3);
////     comm()->clear_requests();
//// }
