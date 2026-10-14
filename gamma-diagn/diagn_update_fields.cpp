/*!
  \file   diagn_update_fields.cpp
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Прошивка гамма-модуля
*/

#include <QString>

#include "dbg.h"
#include "diagn.h"
#include "diagn_gui.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "terminal.h"

void DIAGN_GUI::update_fields(byte cmd, byte ext)
{
    if (mw()->current_tab() != MW::PAGES::DIAGN_PAGE)
        return;

    DBG(3);
    switch (cmd)
    {
        case 0x01:                 // Read Gamma Ray Data
            update_field_0x01();
            break;
        case 0x05:                 // Read Temperature Data
            update_field_0x05();
            break;
        case 0x06:                 // Read Accelerometer Raw Data
            update_field_0x06(ext);
            break;
        case 0x11:                 // Read Accelerometer Inclination Data
            update_field_0x11(ext);
            break;
        case 0x13:                 // Read/Write Gamma Ray Acquisition Time
            update_field_0x13();
            break;
        case 0x18:                 // Read Accelerometer corrected data
            update_field_0x18(ext);
            break;
        case 0x20:                 // Device Control
            update_field_0x20();
            break;
        case 0x40:                 // Set HV Low
            update_field_0x40();
            break;
        case 0x41:                 // Set HV High
            update_field_0x41();
            break;
        case 0x42:                 // Read/Write Calibrate HV
            update_field_0x42();
            break;
        case 0x43:                 // Read/Write Voltage
            update_field_0x43();
            break;
        case 0x44:                 // Read HVcorr
            update_field_0x44();
            break;
        case 0x46:                 // Read/Write HV Warm-up
            update_field_0x46();
            break;
        case 0x50:                 // Read/Write CTABLE
            update_field_0x50();
            break;
        case 0x51:                 // Read/Write Local Gravity
            update_field_0x51();
            break;
        case 0x1e:                 // Read the MCU's A/D converter
            update_field_0x1e(ext);
            break;
        case 0x1f:                 // Read/Write CONFIG Register
            update_field_0x1f();
            break;
        case 0xf1:                 // Read/Write Master Status Register
            update_field_0xf1();
            break;
        case 0xf8:                 // Read Record Task
            update_field_0xf8();
            break;
        case 0xfc:                 // PartNumber
            update_field_0xfc();
            break;
        default:
            qDebug("FIRMWARE::%s:: - unregistered cmd: 0x%02X", __FUNCTION__, cmd);
            break;
    }
}

void DIAGN_GUI::update_field_0x01()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_gamma_cmd->setText("0x01");
    m_fld_diag_gamma_ms->setText(QString("%1 %2").arg(node->m_gamma_elapsed_time).arg(tr("ms")));

    auto value = node->m_gamma_value;
    QString txt = QString("[%1] %2").arg(node->m_gamma_ext).arg(value, 0, 'f', 2);
    m_fld_diag_gamma_cnts->setText(txt);

    auto str = QString("Gamma CPS: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_gamma_log->isChecked())
    {
        write_term_protocol_record( QString::number(node->m_gamma_ext), -12);
        write_term_protocol_record( QString::number(value, 'f', 2), -12);
    }
}

void DIAGN_GUI::update_field_0x05()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_Temperature_cmd->setText("0x05");
    m_fld_diag_Temperature_ms->setText(QString("%1 %2").arg(node->m_gamma_elapsed_time).arg(tr("ms")));

    auto txt = QString("[%1] %2")
               .arg(QString::number(node->m_temperature_num))
               .arg(node->m_temperature_value, 0, 'f', 2);
    m_fld_diag_Temperature->setText(txt);

    auto str = QString("Temp: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_Temperature_log->isChecked())
        write_term_protocol_record(QByteArray::number(node->m_temperature_value, 'f', 2), -12);
}

void DIAGN_GUI::update_field_0x06(byte ext)
{
    auto *node = m_diagn->nodeE4();
    int sum = node->m_accelerometer_raw_elapsed_time[0] +
              node->m_accelerometer_raw_elapsed_time[1] +
              node->m_accelerometer_raw_elapsed_time[2];
    m_fld_diag_accel_raw_cmd->setText("0x06");
    m_fld_diag_accel_raw_ms->setText(QString("%1 %2").arg(sum).arg(tr("ms")));

    auto txt = QString("%1 %2 %3")
               .arg(node->m_accelerometer_raw_value[0], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_raw_value[1], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_raw_value[2], 7, 'f', 4, QChar(' '));
    m_fld_diag_accel_raw->setText(txt);
    m_fld_diag_accel_raw->setToolTip(txt);

    int num = ext-1;
    auto str = QString("Acc Raw:   [%1] %2").arg(num).arg(node->m_accelerometer_raw_value[0], 7, 'f', 4, QChar(' '));
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_accel_raw_log->isChecked())
        write_term_protocol_record(QString::number(node->m_accelerometer_raw_value[num], 'f', 1), -12);
}

void DIAGN_GUI::update_field_0x11(byte ext)
{
    auto *node = m_diagn->nodeE4();
    int sum = node->m_accelerometer_incl_elapsed_time[0] +
              node->m_accelerometer_incl_elapsed_time[1] +
              node->m_accelerometer_incl_elapsed_time[2];
    m_fld_diag_accel_incl_cmd->setText("0x11");
    m_fld_diag_accel_incl_ms->setText(QString("%1 %2").arg(sum).arg(tr("ms")));

    auto txt = QString("%1 %2 %3")
               .arg(node->m_accelerometer_incl_value[0], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_incl_value[1], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_incl_value[2], 7, 'f', 4, QChar(' '));
    m_fld_diag_accel_incl->setText(txt);
    m_fld_diag_accel_incl->setToolTip(txt);

    int num = ext-1;
    auto str = QString("Acc Incl:  [%1] %2").arg(num).arg(node->m_accelerometer_incl_value[num], 7, 'f', 4, QChar(' '));
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_accel_incl_log->isChecked())
        write_term_protocol_record( QString::number(node->m_accelerometer_incl_value[num], 'f', 2), -12);
}

void DIAGN_GUI::update_field_0x13()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_gamma_AcqTime_cmd->setText("0x13");
    m_fld_diag_gamma_AcqTime_ms->setText(QString("%1 %2").arg(node->m_gamma_acq_time_elapsed_time).arg(tr("ms")));

    auto txt = QString("%1 (%2 %3)").arg(node->m_gamma_acq_num).arg(node->m_gamma_acq_time, 0, 'f', 1).arg(tr("sec"));
    m_fld_diag_gamma_AcqTime->setText(txt);
    m_fld_diag_gamma_AcqTime->setToolTip(txt);

    auto str = QString("Acq Time: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_gamma_AcqTime_log->isChecked())
        write_term_protocol_record(QString("%1").arg(node->m_gamma_acq_time, 0, 'f', 1), -12);
}

void DIAGN_GUI::update_field_0x18(byte ext)
{
    auto *node = m_diagn->nodeE4();
    int sum = node->m_accelerometer_corr_elapsed_time[0] +
              node->m_accelerometer_corr_elapsed_time[1] +
              node->m_accelerometer_corr_elapsed_time[2];
    m_fld_diag_accel_corr_cmd->setText("0x18");
    m_fld_diag_accel_corr_ms->setText(QString("%1 %2").arg(sum).arg(tr("ms")));

    auto txt = QStringLiteral("%1 %2 %3")
               .arg(node->m_accelerometer_corr_value[0], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_corr_value[1], 7, 'f', 4, QChar(' '))
            .arg(node->m_accelerometer_corr_value[2], 7, 'f', 4, QChar(' '));
    m_fld_diag_accel_corr->setText(txt);
    m_fld_diag_accel_corr->setToolTip(txt);

    int num = ext-1;
    auto str = QString("Acc Corr: [%1] %2").arg(num).arg(node->m_accelerometer_corr_value[num], 7, 'f', 4, QChar(' '));
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_accel_corr_log->isChecked())
        write_term_protocol_record( QString::number(node->m_accelerometer_corr_value[num], 'f', 1), -12);
}

void DIAGN_GUI::update_field_0x20()
{
    auto *node = m_diagn->nodeE4();
    // TODO - нужно-ли отображение поля 0x20
    // m_fld_diag_dev_control_cmd->setText("0x20");
    // m_fld_diag_dev_control_ms->setText(QString("%1 %2").arg(node->m_dev_control_elapsed_time).arg(tr("ms")));

    auto txt = QString("[%1] %2").arg(node->m_dev_control_num).arg(node->m_dev_control_value, 0, 'f', 6);
    auto str = QString("Dev Ctrl: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());
    // if (m_fld_diag___log->isChecked())
    //     write_term_protocol_record( txt.toLocal8Bit(), -12);
}

void DIAGN_GUI::update_field_0x40()
{
    // команда не имеет ответа с данными!
}

void DIAGN_GUI::update_field_0x41()
{
    // команда не имеет ответа с данными!
}

void DIAGN_GUI::update_field_0x42()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_HV_max_cmd->setText("0x42");
    m_fld_diag_HV_max_ms->setText(QString("%1 %2").arg(node->m_calibr_hv_elapsed_time).arg(tr("ms")));

    auto txt = QString("%1 / %2 / %3").arg(node->m_calibr_hv_min).arg(node->m_calibr_hv_tst).arg(node->m_calibr_hv_max);
    m_fld_diag_HV_max->setText(txt);

    auto str = QString("HV [min/test/max]: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_HV_max_log->isChecked())
        write_term_protocol_record(txt, -18);
}

void DIAGN_GUI::update_field_0x43()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_HV_cmd->setText("0x43");
    m_fld_diag_HV_ms->setText(QString("%1 %2").arg(node->m_hv_elapsed_time).arg(tr("ms")));

    auto txt = QString::number(node->m_operating_hv);
    m_fld_diag_HV->setText(txt);

    auto str = QString("Operating HV: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_HV_log->isChecked())
        write_term_protocol_record(QString::number(node->m_operating_hv, 10), -12);
}

void DIAGN_GUI::update_field_0x44()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_HV_corr_cmd->setText("0x44");
    m_fld_diag_HV_corr_ms->setText(QString("%1 %2").arg(node->m_hv_corr_elapsed_time).arg(tr("ms")));

    auto txt = QString("%1").arg(node->m_hv_corr, 2, 16, QChar('0')).toUpper();
    m_fld_diag_HV_corr->setText(txt);

    auto str = QString("HV corr: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_HV_corr_log->isChecked())
        write_term_protocol_record(txt.toUtf8(), -12);
}

void DIAGN_GUI::update_field_0x46()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_HV_warm_cmd->setText("0x46");
    m_fld_diag_HV_warm_ms->setText(QString("%1 %2").arg(node->m_warmup_elapsed_time).arg(tr("ms")));

    auto txt = QString::number(node->m_warmup_time, 'f', 1);
    m_fld_diag_HV_warm->setText(txt);

    auto str = QString("Acq WarmUp: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_HV_warm_log->isChecked())
        write_term_protocol_record(txt.toUtf8(), -12);
}

void DIAGN_GUI::update_field_0x50()
{
    // не используется
}

void DIAGN_GUI::update_field_0x51()
{
    // не используется
}

void DIAGN_GUI::update_field_0x1e(byte ext)
{
    // qDebug("DIAGN_GUI::%s(): ext:%d", __FUNCTION__, ext);
    auto *node = m_diagn->nodeE4();
    float value;
    if (ext == 0)
        value = node->m_voltage15;
    else if (ext == 1)
        value = node->m_voltage5;
    else if (ext == 2)
        value = node->m_dv485;
    else if (ext == 3)
        value = node->m_high_voltage_control;
    else if (ext == 4)
        value = node->m_temperature_sensor;
    else if (ext == 5)
        value = node->m_temperature_circuit_calibration;
    else if (ext == 6)
        value = node->m_vdc25_reference;
    else if (ext == 7)
        value = node->m_power_amperage;
    else
        return;

    if (m_fld_diag_ADC_log[ext]->isChecked())
    {
        m_fld_diag_ADC_cmd[ext]->setText("0x1E");
        m_fld_diag_ADC_ms[ext]->setText(QString("%1 %2").arg(node->m_adc_elapsed_time[ext]).arg(tr("ms")));
        m_fld_diag_ADC[ext]->setText(QString::number(value, 'f', 6));

        auto str = QString("ADC: %1 (%2)")
                   .arg(value, 10, 'f', 6, QChar(' '))
                   .arg(NodeE4::ADC_name(ext));
        m_terminal->put_data_ln(str.toUtf8());

        write_term_protocol_record(QString("%1").arg(value, 6, 'f', 4), -12);
    }
}

void DIAGN_GUI::update_field_0x1f()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_config_reg_cmd->setText("0x1F");
    m_fld_diag_config_reg_ms->setText(QString("%1 %2").arg(node->m_config_register_elapsed_time).arg(tr("ms")));

    auto txt = QStringLiteral("%1").arg(node->m_config_register, 2, 16, QChar('0')).toUpper();
    m_fld_diag_config_reg->setText(QString("0x%1").arg(txt));

    auto str = QString("CONFIG: 0x%1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_config_reg_log->isChecked())
        write_term_protocol_record(QString("0x%1").arg(txt), -12);
}

void DIAGN_GUI::update_field_0xf1()
{
    // не используется
}

void DIAGN_GUI::update_field_0xf8()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_rectask_cmd->setText("0xF8");
    m_fld_diag_rectask_ms->setText(QString("%1 %2").arg(node->m_rectask_elapsed_time).arg(tr("ms")));

    auto txt = QStringLiteral("[%1] %2s %3s %4 %5 %6")
               .arg(node->m_rectask_num)
               .arg(node->m_rectask_initdelay/10.0)
               .arg(node->m_rectask_period/10.0)
               .arg(node->m_rectask_records)
               .arg(node->m_rectask_buckets)
               .arg(node->m_rectask_logmode);
    m_fld_diag_rectask->setText(txt);

    auto str = QString("Record Task: %1").arg(txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_rectask_log->isChecked())
        write_term_protocol_record(txt.toUtf8(), -12);
}

void DIAGN_GUI::update_field_0xfc()
{
    auto *node = m_diagn->nodeE4();
    m_fld_diag_Partnumber_cmd->setText("0xFC");
    m_fld_diag_Partnumber_ms->setText(QString("%1 %2").arg(node->m_partnumber_elapsed_time).arg(tr("ms")));

    QString txt = node->partnumber();
    m_fld_diag_Partnumber->setText(txt);

    auto str = QString("%1 %2").arg(tr("Part number:"), txt);
    m_terminal->put_data_ln(str.toUtf8());

    if (m_fld_diag_Partnumber_log->isChecked())
        write_term_protocol_record(txt.toUtf8(), -12);
}
