/*!
  \file   plato_update_fields.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Тест линейности - отображения данных
*/

#include <QtDebug>
#include <QString>

#include "dbg.h"
#include "NodeE4.h"
#include "mainwindow.h"
#include "plato.h"
#include "plato_gui.h"
#include "utils.h"

//---------------------------------------------------------
void PLATO_GUI::update_fields(byte cmd, byte ext)
{
    Q_UNUSED(ext);
    if (mw()->current_tab() != MW::PAGES::PLATO_PAGE)
        return;

    DBG(4);
    switch (cmd)
    {
        case 0x01:                 // Read Gamma Ray Data
            update_field_0x01();
            break;
        case 0x05:                 // Read Temperature Data
            update_field_0x05();
            break;
        case 0x13:                 // Read Gamma Ray Acquisition Time
            update_field_0x13();
            break;
        case 0x42:                 // Read Calibrate HV
            update_field_0x42();
            break;
        case 0x43:                 // Read Voltage
            update_field_0x43();
            break;
        case 0x46:                 // Read HV Warm-up
            update_field_0x46();
            break;
        case 0xfc:                 // PartNumber
            update_field_0xfc();
            break;
        default:
            qDebug("FIRMWARE_GUI::%s() - unregistered cmd: 0x%02X", __FUNCTION__, cmd);
            break;
    }
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x01()
{
    m_fld_CPS->setText(QString::number(m_plato->nodeE4()->m_gamma_value, 'f', 2));
    m_fld_CPS->setToolTip(m_fld_CPS->text());
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x05()
{
    auto value = m_plato->nodeE4()->m_temperature_value;
    m_fld_Temper->setText(QString::number(value, 'f', 1));
    m_fld_Temper->setToolTip(m_fld_Temper->text());
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x13()
{
    // auto *node = m_plato->nodeE4();
    // node->m_gamma_acq_time;
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x42()
{
    auto *node = m_plato->nodeE4();
    m_plato->m_calibr_hv_min = node->m_calibr_hv_min;
    m_plato->m_calibr_hv_max = node->m_calibr_hv_max;
    m_plato->m_calibr_hv_tst = node->m_calibr_hv_tst;

    if (m_plato->m_HV_orig_lo == 0)
    {
        m_plato->m_HV_orig_lo = m_plato->m_calibr_hv_min;
        DBGF(1, "Original Low HW:       %d", m_plato->m_HV_orig_lo);
    }
    if (m_plato->m_HV_orig_hi == 0)
        m_plato->m_HV_orig_hi = m_plato->m_calibr_hv_max;
    {
        DBGF(1, "Original High HV:      %d", m_plato->m_HV_orig_hi);
    }
    if (m_plato->m_HV_orig_tst == 0)
    {
        m_plato->m_HV_orig_tst = m_plato->m_calibr_hv_tst;
        DBGF(1, "Original Test HV:      %d", m_plato->m_HV_orig_tst);
    }
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x43()
{
    auto *node = m_plato->nodeE4();
    m_fld_HV->setText(QString::number(node->m_operating_hv));
    m_fld_HV->setToolTip(m_fld_HV->text());

    if (m_plato->m_HV_orig == 0) {
        m_plato->m_HV_orig = node->m_operating_hv;
    }
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0x46()
{
    // auto *node = m_plato->nodeE4();
    // node->m_warmup_time;
}

//---------------------------------------------------------
void PLATO_GUI::update_field_0xfc()
{
    auto *node = m_plato->nodeE4();
    m_fld_PN->setText(node->partnumber());
    m_fld_PN->setToolTip(node->partnumber());
}

//---------------------------------------------------------
void PLATO_GUI::update_field__acquire_time(int acq_time)
{
    DBG(4);
    auto buf = QString("%1 %2").arg(acq_time).arg(tr("sec"));
    m_fld_acq_time->setText(buf);
    auto tip = QString("%1\n%2").arg(tr("Time of one measurement"), buf);
    m_fld_acq_time->setToolTip(tip);
}

//---------------------------------------------------------
void PLATO_GUI::update_field__acquire_time_details(int warmup_time, int gamma_acq_bucket, int acq_one_gamma, int samples_for_avrg)
{
    DBG(4);
    int one_voltage_acq_time = warmup_time + (acq_one_gamma * samples_for_avrg);
    auto buf = QString("%1 %2").arg(one_voltage_acq_time).arg(tr("sec"));
    m_fld_acq_time->setText(buf);

    buf = QString("%1 + ([%2x%3] x %4) = %5 %6")
              .arg(warmup_time)
              .arg(2.5)
              .arg(gamma_acq_bucket)
              .arg(samples_for_avrg)
              .arg(one_voltage_acq_time)
              .arg(tr("sec"));
    auto tip = QString("%1\n%2\n%3")
                   .arg(tr("Time of one measurement"), tr("WarmUp time + Acquire time"), buf);
    m_fld_acq_time->setToolTip(tip);
}

//---------------------------------------------------------
void PLATO_GUI::update_field__steps_done(int step, int total)
{
    DBG(4);
    auto txt = QString("%1 %2/%3").arg(tr("Sample")).arg(step).arg(total);
    m_fld_samples_msg->setText(txt);

    txt = QString("%1\n%2 - %3")
              .arg(tr("How much samples has been passed already"), tr("Total steps"))
              .arg(total);
    m_fld_samples_msg->setToolTip(txt);

    int percent = (double)step / (double)total * 100.0;
    m_samples_progress_bar->setValue(percent);
}

//---------------------------------------------------------
void PLATO_GUI::update_field__remain_time(int remain_sec, int total_sec)
{
    DBG(4);
    (void)total_sec;
    // QLocale loc(QLocale::system());
    // auto txt = loc.toDateTime(dt.toString(), QLocale::ShortFormat).toString();

    QDateTime dt = QDateTime::currentDateTime();
    dt = dt.addSecs(remain_sec);
    auto estimated_end_time = dt.toString("hh:mm:ss");
    // auto txt = QString("%1 [%2]").arg(UTILS::secs_to_minute(remain_sec)).arg(estimated_end_time);
    QString txt;
    if (remain_sec < 60) {
        txt = QString("%1 %2 [%3]").arg(remain_sec).arg(tr("sec")).arg(estimated_end_time);
        m_fld_est_time->setToolTip(QString("%1\n%2\n%3")
                                   .arg(tr("Estimated time of calibration completion"))
                                   .arg(UTILS::secs_to_minute(remain_sec))
                                   .arg(estimated_end_time));
    } else {
        txt = QString("%1 %2 [%3]").arg(int(remain_sec / 60)).arg(tr("min")).arg(estimated_end_time);
        m_fld_est_time->setToolTip(QString("%1\n%2").arg(tr("Estimated time of calibration completion"), txt));
    }
    m_fld_est_time->setText(txt);
}

//---------------------------------------------------------
void PLATO_GUI::update_field__estimated_time(const QString &cdate)
{
    DBG(4);
    m_fld_est_time->setText(cdate);
    m_fld_est_time->setToolTip(QString("%1\n%2").arg(tr("Date of calibration"), cdate));
}

