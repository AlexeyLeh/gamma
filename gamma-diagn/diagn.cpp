/*!
  \file   diagn.cpp
  \brief  Диагностика гамма-модуля
  \author Aleksey Lekhtin
  \date   10.05.2023
*/

#include "comm.h"
#include "bto.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "terminal.h"
#include "utils.h"

//---------------------------------------------------------
DIAGN::DIAGN(APP *m, DIAGN_GUI *gui) :
    m_app(m),
    m_gui(gui),
    m_diagn_scanning_time(2000),
    m_gamma_counts_avrg(1)
{
    m_gui->set_backgr(this);
    read_settings(m_app->qSettings());

    connect(&m_timer_diagn, SIGNAL(timeout()),
            this, SLOT(slot_diagn_scanning()));

    connect(comm(), SIGNAL(signal_comm_comport_disconnected()),
            this, SLOT(slot_comm__comport_disconnected()), Qt::QueuedConnection);

    connect(comm(), &COMM::signal_comm_requests_completed, this, [this](int nodecode) {
        m_gui->requests_completed(nodecode);
    });

    connect(comm(), &COMM::signal_comm_NodeE4_responded, this, [this](byte cmd, byte ext){
        m_gui->update_fields(cmd, ext);
    });

    connect(comm(), SIGNAL(signal_comm_not_responded(NODECODE, QString)),
            this, SLOT(slot_comm__not_responded(NODECODE, QString)));

    m_diagn_requests =
            std::list<Adam::Frame> {
            NodeE4::ReadPartNumber(),
            NodeE4::ReadConfigRegister(),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc15),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc5),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Dv485),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::HighVoltageControl),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensor),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::TempSensorCalibr),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::Vdc25Reference),
            NodeE4::ReadAdcSignal(NodeE4::AdcSignal::PowerSupplyAmperage),
            NodeE4::ReadRecordTask(1),
            NodeE4::ReadRecordTask(5),
            NodeE4::ReadRecordTask(6),
            NodeE4::ReadTemperature(),
            NodeE4::ReadHVCorr(),
            NodeE4::ReadOperationVoltage(),
            NodeE4::ReadCalibrationVoltages(),
            NodeE4::ReadHVWarmUp(),
            NodeE4::ReadGammaRayAcqusitionTime(),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Short),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelRaw(NodeE4::AvgType::Long),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Short),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelCorrectedData(NodeE4::AvgType::Long),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Short),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Medium),
            NodeE4::ReadAccelInclination(NodeE4::AvgType::Long),
            NodeE4::ReadGammaCPS(),
            NodeE4::ReadMasterStatusRegister(),
    };

}

//---------------------------------------------------------
DIAGN::~DIAGN()
{
    DBG(4);
}

//---------------------------------------------------------
void DIAGN::update_gui()
{
    m_gui->set_led_icon_off();
}

//---------------------------------------------------------
const QString DIAGN::report_path() const
{
    return QCoreApplication::applicationDirPath() + "/reports";
}

//---------------------------------------------------------
int DIAGN::polling_time()
{
    return m_diagn_scanning_time / 1000; // sec
}

//---------------------------------------------------------
void DIAGN::set_polling_time(int sec)
{
    m_diagn_scanning_time = sec * 1000;
}

//---------------------------------------------------------
const std::list<Adam::Frame> &DIAGN::requests() const
{
    return m_diagn_requests;
}

//---------------------------------------------------------
void DIAGN::make_request(const Adam::Frame& frame) const
{
    comm()->push_request(frame);
}

//---------------------------------------------------------
void DIAGN::slot_comm__comport_disconnected()
{
    if (mw()->current_tab() != MW::PAGES::DIAGN_PAGE) return;
    DBG(3, "got signal_comm_comport_disconnected();");
    m_gui->stop_diagnostic();
}

//---------------------------------------------------------
void DIAGN::slot_comm__not_responded(NODECODE nodeCode, QString err_string)
{
    if (mw()->current_tab() != MW::PAGES::DIAGN_PAGE) return;
    if (nodeCode != NodeE4::CODExE4) return;
    gui()->showstatus(UTILS::split_message(err_string));
    DBG(3, "emit signal__message(QMessageBox::Critical, err_string)");
    gui()->stop_diagnostic();
    emit mw()->signal_message(QMessageBox::Warning, {}, err_string);
}

//---------------------------------------------------------
bool DIAGN::start_scan()
{
    DBG(3);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        auto msg = tr("No connection");
        gui()->showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return FAIL;
    }
    if (!m_app->bto()->gamma_module_found()) {
        auto msg = tr("Module is not found");
        gui()->showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return FAIL;
    }
    m_timer_diagn.start(0);
    return OK;
}

//---------------------------------------------------------
void DIAGN::stop_diagn_timer()
{
    DBG(3);
    m_timer_diagn.stop();
    comm()->clear_requests();
}

//---------------------------------------------------------
void DIAGN::restart_scan()
{
    DBG(2, QString("diagn scanning time: %1").arg(m_diagn_scanning_time));
    auto was_activated = m_timer_diagn.isActive();
    m_timer_diagn.stop();
    if (was_activated) {
        m_timer_diagn.start(m_diagn_scanning_time);
    }
}

//// //---------------------------------------------------------
//// void DIAGN::cancel_scan()
//// {
////     DBG(2);
////     m_gui->stop_diagnostic();
//// }

//---------------------------------------------------------
void DIAGN::slot_diagn_scanning()
{
    m_timer_diagn.stop();
    m_gui->set_led_icon_active();

    if (m_diagn_requests.size() > 0) {
        gui()->terminal()->put_data("\n");
    }

    std::list<Adam::Frame> lst;
    for (const auto &req : m_diagn_requests)
    {
        auto cmd = req.cmd();
        if (cmd == 0x01 && m_gui->m_fld_diag_gamma_log->isChecked()) {
            auto frame = NodeE4::ReadGammaCPS(byte(m_gamma_counts_avrg));
            lst.emplace_back(frame);
        }
        else if (cmd == 0x05 && m_gui->m_fld_diag_Temperature_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x06 && m_gui->m_fld_diag_accel_raw_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x11 && m_gui->m_fld_diag_accel_incl_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x13 && m_gui->m_fld_diag_gamma_AcqTime_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x18 && m_gui->m_fld_diag_accel_corr_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x42 && m_gui->m_fld_diag_HV_max_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x43 && m_gui->m_fld_diag_HV_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x44 && m_gui->m_fld_diag_HV_corr_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x46 && m_gui->m_fld_diag_HV_warm_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0x1e) {
            const auto &data = req.payload();
            if (data.size() > 0)
            {
                byte ext = data[0];
                if (ext < 8 && m_gui->m_fld_diag_ADC_log[ext]->isChecked())
                {
                    lst.emplace_back(req);
                }
            }
        }
        else if (cmd == 0x1f && m_gui->m_fld_diag_config_reg_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0xf8 && m_gui->m_fld_diag_rectask_log->isChecked()) {
            lst.emplace_back(req);
        }
        else if (cmd == 0xfc && m_gui->m_fld_diag_Partnumber_log->isChecked()) {
            lst.emplace_back(req);
        }
    }

    int rtime = 0;
    if (lst.size() > 0) {
        rtime = comm()->push_requests(lst);
    } else {
        m_gui->set_led_icon_idle();
    }

    m_timer_diagn.start(m_diagn_scanning_time + rtime);
}

//---------------------------------------------------------
void DIAGN::set_E4_CPS_avrg(int counts)
{
    m_gamma_counts_avrg = counts;
}

//---------------------------------------------------------
//// void DIAGN::restore_original_HV()
//// {
////     // LOG::dbgf3("DIAGN::%s()\n", __FUNCTION__);
////     // Восстановление в процессоре исходного высоковольтного напряжения
////     if (m_diagn_orig_hv == 0) return;
////     LOG::dbgf2("DIAGN::%s()\n", __FUNCTION__);
////     m_gui->showstatus(tr("Restoring Original HV"), 2);
////     comm()->push_request(NodeE4::WriteVoltage(m_diagn_orig_hv));
////     MW::delay(500);
////     qDebug() << __FUNCTION__ << "(): Original HV is restored -" << m_diagn_orig_hv << "V";
//// }
