/*!
  \file   diagn_settings.cpp
  \brief  Сохранение настроек диагностики гамма-модуля
  \author Aleksey Lekhtin
  \date   10.05.2023
*/

#include "diagn.h"
#include "diagn_gui.h"
#include "dbg.h"
#include "mainwindow.h"
#include "gamma.h"

void DIAGN_GUI::read_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Diagnostic");

    //// m_low_bound_HV = st->value("HV_Bound_Min", GAMMA::HV_Test_Initial).toInt();
    //// m_high_bound_HV = st->value("HV_Bound_Max", GAMMA::HV_Test_Final).toInt();

    auto log_fname = st->value("LogFile",m_diagn->report_path()).toString();
    QFileInfo fi(log_fname);
    log_fname = QDir::toNativeSeparators(fi.filePath());
    m_fld_diag_term_fname_log->setText(log_fname);

    int num = st->value("LogEnabled", 0).toInt();
    if (log_fname.isEmpty()) num = 0;
    m_chkbox_term_log_enabled->setChecked((num == 1 ? true : false));
    slot_set_term_log_enabled((num == 1 ? true : false));

    num = st->value("LogSoftPN",     0).toInt();
    m_fld_diag_Partnumber_log->setChecked((num == 1 ? true : false));

    num = st->value("LogGammaCnts",  0).toInt();
    m_fld_diag_gamma_log->setChecked((num == 1 ? true : false));

    num = st->value("LogGammaTime",  0).toInt();
    m_fld_diag_gamma_AcqTime_log->setChecked((num == 1 ? true : false));

    num = st->value("LogHV_max",   0).toInt();
    m_fld_diag_HV_max_log->setChecked((num == 1 ? true : false));

    num = st->value("LogHV_corr",  0).toInt();
    m_fld_diag_HV_corr_log->setChecked((num == 1 ? true : false));

    num = st->value("LogHV_warm",  0).toInt();
    m_fld_diag_HV_warm_log->setChecked((num == 1 ? true : false));

    num = st->value("LogHV",       0).toInt();
    m_fld_diag_HV_log->setChecked((num == 1 ? true : false));

    num = st->value("LogConfig",     0).toInt();
    m_fld_diag_config_reg_log->setChecked((num == 1 ? true : false));

    num = st->value("LogRecordTask", 0).toInt();
    m_fld_diag_rectask_log->setChecked((num == 1 ? true : false));

    num = st->value("LogTemp",       0).toInt();
    m_fld_diag_Temperature_log->setChecked((num == 1 ? true : false));

    num = st->value("LogGz",0).toInt();
    m_fld_diag_accel_raw_log->setChecked((num == 1 ? true : false));

    num = st->value("LogGzCorrected",0).toInt();
    m_fld_diag_accel_corr_log->setChecked((num == 1 ? true : false));

    num = st->value("LogGzInclination",0).toInt();
    m_fld_diag_accel_incl_log->setChecked((num == 1 ? true : false));

    for(quint8 i = 0; i < 8; ++i) {
        num = st->value(QString("LogADC-%1").arg(i), 0).toInt();
        m_fld_diag_ADC_log[i]->setChecked((num == 1 ? true : false));
    }

    st->endGroup();
}

void DIAGN_GUI::save_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Diagnostic");
    auto str = QDir::fromNativeSeparators(m_fld_diag_term_fname_log->text());
    st->setValue("LogFile", str);
    st->setValue("LogEnabled",         (m_chkbox_term_log_enabled->isChecked()   ? 1 : 0));
    st->setValue("LogSoftPN",          (m_fld_diag_Partnumber_log->isChecked()   ? 1 : 0));
    st->setValue("LogStatus",          (m_fld_diag_rectask_log->isChecked()      ? 1 : 0));
    st->setValue("LogGammaCnts",       (m_fld_diag_gamma_log->isChecked()        ? 1 : 0));
    st->setValue("LogGammaTime",       (m_fld_diag_gamma_AcqTime_log->isChecked()? 1 : 0));
    st->setValue("LogHV_max",          (m_fld_diag_HV_max_log->isChecked()     ? 1 : 0));
    st->setValue("LogHV_corr",         (m_fld_diag_HV_corr_log->isChecked()    ? 1 : 0));
    st->setValue("LogHV_warm",         (m_fld_diag_HV_warm_log->isChecked()    ? 1 : 0));
    st->setValue("LogHV",              (m_fld_diag_HV_log->isChecked()         ? 1 : 0));
    st->setValue("LogConfig",          (m_fld_diag_config_reg_log->isChecked()   ? 1 : 0));
    st->setValue("LogRecordTask",      (m_fld_diag_rectask_log->isChecked()      ? 1 : 0));
    st->setValue("LogTemp",            (m_fld_diag_Temperature_log->isChecked()  ? 1 : 0));
    st->setValue("LogGz",              (m_fld_diag_accel_raw_log->isChecked()    ? 1 : 0));
    st->setValue("LogGzCorrected",     (m_fld_diag_accel_corr_log->isChecked()   ? 1 : 0));
    st->setValue("LogGzInclination",   (m_fld_diag_accel_incl_log->isChecked()   ? 1 : 0));
    for(quint8 i = 0; i < 8; ++i) {
        st->setValue(QString("LogADC-%1").arg(i), (m_fld_diag_ADC_log[i]->isChecked() ? 1 : 0));
    }

    st->endGroup();
}

void DIAGN::read_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Diagnostic");

    int sec = st->value("PollingTime", 2).toInt();
    m_diagn_scanning_time = sec * 1000;
    m_gui->set_poll_time(sec);

    // Количество усреднений отсчетов для процессора
    int avrg = st->value("CPSAverage", 4).toInt();
    if (avrg == 0 || avrg > 30) avrg = 4;
    m_gamma_counts_avrg = avrg;
    m_gui->set_E4_CPS_avrg(m_gamma_counts_avrg);

    st->endGroup();
}

void DIAGN::save_settings(QSettings *st)
{
    DBG(4);
    st->beginGroup("Diagnostic");
    st->setValue("CPSAverage", m_gamma_counts_avrg);
    st->setValue("PollingTime",  m_diagn_scanning_time / 1000);
    st->endGroup();
}

//---------------------------------------------------------
void DIAGN_GUI::set_content_font_size(int sz)
{
    DBG(4);

    std::list<QPushButton*> buttons {
        m_button_diagn_start,
    };

    for (auto but : buttons)
    {
        auto style = QString("QPushButton { font: %1pt; }").arg(sz);
        but->setStyleSheet(style);
    }

    ////auto fnt = m_button_diagn_start->font();
    ////fnt.setPointSize(sz);
    ////fnt.setBold(true);
    ////fnt.setStyleHint(QFont::Monospace);
    ////fnt.setFamily("Fixed");
    ////fnt.setFixedPitch(true);
    ////m_button_diagn_start->setFont(fnt);
}
