/*!
  \file   create_diagnostic.cpp
  \brief  Раскладка полей диагностики
  \author Aleksey Lekhtin
  \date   29.11.2016
*/

#include "gamma.h"
#include "dbg.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "terminal.h"

// Задать рабочее высоковольтное напряжение
//// void DIAGN_GUI::slot_edit_operating_voltage()
//// {
////     QApplication::setOverrideCursor(Qt::WaitCursor);
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->connect_to();
////     }
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         auto msg = tr("No connection");
////         showstatus(msg);
////         emit mw()->signal__message(QMessageBox::Warning, msg);
////         return;
////     }
////
////     int rtime = comm()->push_request(NodeE4::ReadOperationVoltage()); // 0x43
////     MW::delay(rtime + 500);
////
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         return;
////     }
////     auto value_oper = m_diagn->nodeE4()->m_hv;
////     if (value_oper == 0) value_oper = 1500;
////
////     QDialog *dlg = new QDialog(mw());
////     dlg->setWindowTitle(tr("Changing High Voltage Operating Value"));
////
////     QSpinBox *fld1 = new QSpinBox(dlg);
////     fld1->setRange(GAMMA::HV_Min, GAMMA::HV_Max);
////     fld1->setSingleStep(25);
////     fld1->setValue(value_oper);
////     QString label_title = QString("%1").arg(tr("New value"));
////
////     QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
////     buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
////
////     QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));
////
////     btn = buttons_group->button(QDialogButtonBox::Cancel);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));
////     btn->setDefault(true);
////     btn->setAutoDefault(true);
////
////     //---------------------------------------------------------
////     QGridLayout *lay = new QGridLayout();
////     lay->addWidget(new QLabel(label_title), 0, 0);
////     lay->addWidget(fld1, 0, 1);
////     lay->addWidget(buttons_group, 1, 0, 1, 2);
////
////     dlg->setLayout(lay);
////     dlg->adjustSize();
////     dlg->setMinimumWidth(400);
////
////     QFont fnt = dlg->font();
////     fnt.setPointSize(mw()->content_font_pointSize());
////     dlg->setFont(fnt);
////
////     QApplication::restoreOverrideCursor();
////     QApplication::setOverrideCursor(Qt::ArrowCursor);
////     int rc = dlg->exec();
////     QApplication::restoreOverrideCursor();
////     if (rc == QDialog::Accepted)
////     {
////         QApplication::setOverrideCursor(Qt::WaitCursor);
////         value_oper = fld1->value();
////         QString msg = QString("%1 to %2").arg(tr("Changing voltages")).arg(value_oper);
////         DBG(1, msg);
////         m_terminal->put_data_ln(msg.toUtf8());
////         rtime = comm()->push_requests({
////             NodeE4::WriteOperationVoltage((ushort)value_oper),
////             NodeE4::ReadOperationVoltage(),
////         });
////         MW::delay(rtime+500);
////         QApplication::restoreOverrideCursor();
////     }
////
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->disconnect_from();
////     }
////
////     delete dlg;
//// }
////
//// // Задать максимальное и минимальное высоковольтное напряжение
//// void DIAGN_GUI::slot_edit_calibration_voltages()
//// {
////     QApplication::setOverrideCursor(Qt::WaitCursor);
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->connect_to();
////     }
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         auto msg = tr("No connection");
////         showstatus(msg);
////         emit mw()->signal__message(QMessageBox::Warning, msg);
////         return;
////     }
////
////     int rtime = comm()->push_request(NodeE4::ReadCalibrationVoltages()); // 0x42
////     MW::delay(rtime + 500);
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         return;
////     }
////
////     int value_min = m_diagn->nodeE4()->m_calibr_hv_min;
////     int value_max = m_diagn->nodeE4()->m_calibr_hv_max;
////     int value_tst = m_diagn->nodeE4()->m_calibr_hv_tst;
////
////     if (value_min == 0) value_min = 800;
////     if (value_max == 0) value_max = 1500;
////     if (value_tst == 0) value_tst = 1800;
////
////     QString buf = m_fld_diag_HV_max->text().trimmed();
////     if (!buf.isEmpty())
////     {
////         QStringList arr = buf.split('/');
////         for(int i = 0; i < arr.size(); ++i)
////             arr[i] = arr[i].trimmed();
////
////         if (arr.size() > 2) value_max = arr[2].toInt();
////         if (arr.size() > 1) value_tst = arr[1].toInt();
////         if (arr.size() > 0) value_min = arr[0].toInt();
////     }
////
////     QDialog *dlg = new QDialog(mw());
////     dlg->setWindowTitle(tr("Changing Min/Test/Max High Voltage Calibration Values"));
////
////     QSpinBox *fld1 = new QSpinBox(dlg);
////     fld1->setRange(GAMMA::HV_Min, GAMMA::HV_Max);
////     fld1->setSingleStep(25);
////     fld1->setValue(value_min);
////
////     QSpinBox *fld2 = new QSpinBox(dlg);
////     fld2->setRange(GAMMA::HV_Min, GAMMA::HV_Max);
////     fld2->setSingleStep(25);
////     fld2->setValue(value_tst);
////
////     QSpinBox *fld3 = new QSpinBox(dlg);
////     fld3->setRange(GAMMA::HV_Min, GAMMA::HV_Max);
////     fld3->setSingleStep(25);
////     fld3->setValue(value_max);
////
////     //---------------------------------------------------------
////     QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
////     buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
////
////     QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));
////
////     btn = buttons_group->button(QDialogButtonBox::Cancel);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));
////     btn->setDefault(true);
////     btn->setAutoDefault(true);
////
////     // connect(buttons_group, SIGNAL(accepted()), dlg, SLOT(accept()));
////     // connect(buttons_group, SIGNAL(rejected()), dlg, SLOT(reject()));
////
////     //---------------------------------------------------------
////     QGridLayout *lay = new QGridLayout();
////     lay->addWidget(new QLabel(tr("New Min Value")), 0, 0);
////     lay->addWidget(fld1, 0, 1);
////     lay->addWidget(new QLabel(tr("New Value for tests")), 1, 0);
////     lay->addWidget(fld2, 1, 1);
////     lay->addWidget(new QLabel(tr("New Max Value")), 2, 0);
////     lay->addWidget(fld3, 2, 1);
////     lay->addWidget(buttons_group, 3, 0, 1, 2);
////
////     dlg->setLayout(lay);
////     dlg->adjustSize();
////     dlg->setMinimumWidth(400);
////
////     QFont fnt = dlg->font();
////     fnt.setPointSize(mw()->content_font_pointSize());
////     dlg->setFont(fnt);
////
////     QApplication::restoreOverrideCursor();
////     QApplication::setOverrideCursor(Qt::ArrowCursor);
////     int rc = dlg->exec();
////     QApplication::restoreOverrideCursor();
////     if (rc == QDialog::Accepted)
////     {
////         QApplication::setOverrideCursor(Qt::WaitCursor);
////         ushort value_min = fld1->value();
////         ushort value_tst = fld2->value();
////         ushort value_max = fld3->value();
////         QString msg = QString("%1 to %2 %3 %4")
////                           .arg(tr("Changing voltages"))
////                           .arg(value_min).arg(value_tst).arg(value_max);
////         DBG(1, msg);
////         m_terminal->put_data_ln(msg.toUtf8());
////         int rtime = comm()->push_requests({
////             NodeE4::WriteCalibrationVoltages(value_min, value_max, value_tst),
////             NodeE4::ReadCalibrationVoltages(),
////             NodeE4::WriteRecordTask({0x01, 0x25, 0x50, 0x00, 0x04, 0x00}),
////         });
////         MW::delay(rtime+500);
////         QApplication::restoreOverrideCursor();
////     }
////
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->disconnect_from();
////     }
////
////     delete dlg;
//// }
////
//// // Задать время получения гамма-отсчетов
//// void DIAGN_GUI::slot_edit_acq_time()
//// {
////     QApplication::setOverrideCursor(Qt::WaitCursor);
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->connect_to();
////     }
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         auto msg = tr("No connection");
////         showstatus(msg);
////         emit mw()->signal__message(QMessageBox::Warning, msg);
////         return;
////     }
////
////     int rtime = comm()->push_request(NodeE4::ReadGammaRayAcqusitionTime()); // 0x13
////     MW::delay(rtime + 500);
////     if (!comm()->isConnected()) {
////         QApplication::restoreOverrideCursor();
////         return;
////     }
////
////     auto value = m_diagn->nodeE4()->m_gamma_acq_num;
////
////     QDialog *dlg = new QDialog(mw());
////     dlg->setWindowTitle(tr("Changing Gamma Ray Acqure Time"));
////
////     // The units used are tenths of a second and the limits are 10 to 600 (1s to 60s).
////     int acqtime_min =  10;
////     int acqtime_max = 600;
////
////     QSpinBox *fld1 = new QSpinBox(dlg);
////     fld1->setRange(acqtime_min, acqtime_max);
////     fld1->setSingleStep(1);
////     fld1->setValue(value);
////     QString label_title = QString("%1 (0.1 %2)").arg(tr("New value"), tr("sec"));
////
////     QDialogButtonBox *buttons_group = new QDialogButtonBox(Qt::Horizontal);
////     buttons_group->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
////
////     QPushButton *btn = buttons_group->button(QDialogButtonBox::Apply);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(accept()));
////
////     btn = buttons_group->button(QDialogButtonBox::Cancel);
////     connect(btn, SIGNAL(clicked()), dlg, SLOT(reject()));
////     btn->setDefault(true);
////     btn->setAutoDefault(true);
////
////     //---------------------------------------------------------
////     QGridLayout *lay = new QGridLayout();
////     lay->addWidget(new QLabel(label_title), 0, 0);
////     lay->addWidget(fld1, 0, 1);
////     lay->addWidget(buttons_group, 1, 0, 1, 2);
////
////     dlg->setLayout(lay);
////     dlg->adjustSize();
////     dlg->setMinimumWidth(400);
////
////     QFont fnt = dlg->font();
////     fnt.setPointSize(mw()->content_font_pointSize());
////     dlg->setFont(fnt);
////
////     QApplication::restoreOverrideCursor();
////     QApplication::setOverrideCursor(Qt::ArrowCursor);
////     int rc = dlg->exec();
////     QApplication::restoreOverrideCursor();
////     if (rc == QDialog::Accepted)
////     {
////         QApplication::setOverrideCursor(Qt::WaitCursor);
////         value = fld1->value();
////         QString msg = QString("%1 to %2").arg(tr("Changing Gamma Ray Acqure Time")).arg(value);
////         DBG(1, msg);
////         m_terminal->put_data_ln(msg.toUtf8());
////         rtime = comm()->push_requests({
////             NodeE4::WriteGammaRayAcqusitionTime(value),
////             NodeE4::ReadGammaRayAcqusitionTime(),
////         });
////         MW::delay(rtime+500);
////         QApplication::restoreOverrideCursor();
////     }
////
////     if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
////         comm()->disconnect_from();
////     }
////
////     delete dlg;
//// }

//---------------------------------------------------------
// Количество усреднений отсчетов для процессора
void DIAGN_GUI::slot_set_gamma_counts_avrg(int counts)
{
    m_diagn->set_E4_CPS_avrg(counts);
}

//---------------------------------------------------------
void DIAGN_GUI::set_E4_CPS_avrg(int counts)
{
    m_fld_diag_gamma_avrg_cnts->setValue(counts);
}

//---------------------------------------------------------
// Установка нижней границы ЦАП высоковольтного напряжения
void DIAGN_GUI::slot_set_hv_low_level()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        QApplication::restoreOverrideCursor();
        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    m_terminal->put_data_ln("set voltage to Low level (DAC 0x00)");
    int rtime = comm()->push_requests({
        NodeE4::SetHVLow(),
        NodeE4::ReadOperationVoltage(),
    });
    mw()->delay(rtime + 500);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->disconnect_from();
    }
}

//---------------------------------------------------------
// Установка верхней границы ЦАП высоковольтного напряжения
void DIAGN_GUI::slot_set_hv_high_level()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->connect_to();
    }
    if (!comm()->isConnected()) {
        QApplication::restoreOverrideCursor();
        auto msg = tr("No connection");
        showstatus(msg);
        emit mw()->signal_message(QMessageBox::Warning, msg);
        return;
    }

    m_terminal->put_data_ln("set voltage to High level (DAC 0xFF)");
    int rtime = comm()->push_requests({
        NodeE4::SetHVHigh(),
        NodeE4::ReadOperationVoltage(),
    });
    mw()->delay(rtime + 500);
    if (comm()->type() == COMM::CONNECT_TYPE::COMPORT) {
        comm()->disconnect_from();
    }
}
