/*!
  \file   fw_update_fields.cpp
  \author Aleksey Lekhtin
  \date   20.10.2016
  \brief  Прошивка гамма-модуля
*/

#include <QString>

#include "dbg.h"
#include "fw.h"
#include "fw_gui.h"
#include "mainwindow.h"
#include "NodeE4.h"

void FIRMWARE_GUI::update_fields(byte cmd, byte ext)
{
    if (mw()->current_tab() != MW::PAGES::FIRMWARE_PAGE)
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
        update_field_0x06();
        break;
    case 0x0C:                 // Read/Write KTABLE
        update_field_0x0C();
        break;
    case 0x11:                 // Read Accelerometer Inclination Data
        update_field_0x11();
        break;
    case 0x13:                 // Read/Write Gamma Ray Acquisition Time
        update_field_0x13();
        break;
    case 0x18:                 // Read Accelerometer corrected data
        update_field_0x18();
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
        DBG(0, QString("-------- UNREGISTERED CMD: 0x%1").arg(cmd, 2, 16, QChar('0')));
        break;
    }
}

void FIRMWARE_GUI::update_field_0x01()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_gamma_ack, node->m_gamma_status, m_fld_fw_CPS))
        return;

    auto value = node->m_gamma_value;
    QString txt = QString("[%1] %2").arg(node->m_gamma_ext).arg(value, 0, 'f', 2);
    m_fld_fw_CPS->setText(QString::number(value, 'f', 2));
    m_fld_fw_CPS->setToolTip(QString::number(value, 'f', 4));
}

void FIRMWARE_GUI::update_field_0x05()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_temperature_ack, node->m_temperature_status, m_fld_fw_Temp)) {
        return;
    }
    m_fld_fw_Temp->setText(QString::number(node->m_temperature_value, 'f', 4));
    m_fld_fw_Temp->setToolTip(QString::number(node->m_temperature_value, 'f', 4));
}

void FIRMWARE_GUI::update_field_0x06()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    for (int i : {0, 1, 2})
    {
        bool ok = check_field_ack(node->code(), node->m_accelerometer_raw_ack[i],
                                  node->m_accelerometer_raw_status[i], m_fld_fw_accel_raw);
        if (!ok) {
            return;
        }
    }

    auto txt = QStringLiteral("%1 %2 %3")
            .arg(node->m_accelerometer_raw_value[0], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_raw_value[1], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_raw_value[2], 6, 'f', 3, QChar(' '));
    m_fld_fw_accel_raw->setText(txt);
    txt = QStringLiteral("%1\n%2\n%3")
            .arg(node->m_accelerometer_raw_value[0], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_raw_value[1], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_raw_value[2], 0, 'f', 6, QChar(' '));
    m_fld_fw_accel_raw->setToolTip(txt);
}

void FIRMWARE_GUI::update_field_0x0C()
{
    DBG(3);
    refresh_ktable();
}

void FIRMWARE_GUI::update_field_0x11()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    for (int i : {0, 1, 2}) {
        bool ok = check_field_ack(node->code(), node->m_accelerometer_incl_ack[i],
                                  node->m_accelerometer_incl_status[i], m_fld_fw_accel_incl);
        if (!ok) {
            return;
        }
    }

    auto txt = QString("%1 %2 %3")
            .arg(node->m_accelerometer_incl_value[0], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_incl_value[1], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_incl_value[2], 6, 'f', 3, QChar(' '));
    m_fld_fw_accel_incl->setText(txt);
    txt = QString("%1\n%2\n%3")
            .arg(node->m_accelerometer_incl_value[0], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_incl_value[1], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_incl_value[2], 0, 'f', 6, QChar(' '));
    m_fld_fw_accel_incl->setToolTip(txt);
}

void FIRMWARE_GUI::update_field_0x13()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_gamma_acq_ack, node->m_gamma_acq_status, m_fld_fw_acqtime)) {
        return;
    }
    m_fld_fw_acqtime->setValue(node->m_gamma_acq_num);
}

void FIRMWARE_GUI::update_field_0x18()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    for (int i : {0, 1, 2})
    {
        bool ok = check_field_ack(node->code(), node->m_accelerometer_corr_ack[i],
                                  node->m_accelerometer_corr_status[i], m_fld_fw_accel_corr);
        if (!ok) {
            return;
        }
    }

    auto txt = QString("%1 %2 %3")
            .arg(node->m_accelerometer_corr_value[0], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_corr_value[1], 6, 'f', 3, QChar(' '))
            .arg(node->m_accelerometer_corr_value[2], 6, 'f', 3, QChar(' '));
    m_fld_fw_accel_corr->setText(txt);
    txt = QString("%1\n%2\n%3")
            .arg(node->m_accelerometer_corr_value[0], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_corr_value[1], 0, 'f', 6, QChar(' '))
            .arg(node->m_accelerometer_corr_value[2], 0, 'f', 6, QChar(' '));
    m_fld_fw_accel_corr->setToolTip(txt);
}

void FIRMWARE_GUI::update_field_0x20()
{
    DBG(3, "nothing...");
    // auto *node = m_fw->nodeE4();
    // m_fld_diag_dev_control_cmd->setText("0x20");
    // m_fld_diag_dev_control_ms->setText(QString("%1 %2").arg(node->m_dev_control_elapsed_time).arg(tr("ms")));
}

void FIRMWARE_GUI::update_field_0x40()
{
    DBG(3, "nothing...");
    // TODO - нужно-ли отображение поля 0x40
    // auto *node = m_fw->nodeE4();
    // m_fld_diag_hv_low_cmd->setText("0x40");
    // m_fld_diag_hv_low_ms->setText(QString("%1 %2").arg(node->m_hv_low_elapsed_time).arg(tr("ms")));
}

void FIRMWARE_GUI::update_field_0x41()
{
    DBG(3, "nothing...");
    // TODO - нужно-ли отображение поля 0x41
    // auto *node = m_fw->nodeE4();
    // m_fld_diag_hv_high_cmd->setText("0x41");
    // m_fld_diag_hv_high_ms->setText(QString("%1 %2").arg(node->m_hv_high_elapsed_time).arg(tr("ms")));
}

void FIRMWARE_GUI::update_field_0x42()
{
    auto *node = m_fw->nodeE4();
    bool enough = false;
    DBGF(3, "hv_min: %d", node->m_calibr_hv_min);
    if (!check_field_ack(node->code(), node->m_calibr_hv_ack, node->m_calibr_hv_status, m_fld_fw_hv_min)) {
        enough = true;
    }
    DBGF(3, "hv_max: %d", node->m_calibr_hv_max);
    if (!check_field_ack(node->code(), node->m_calibr_hv_ack, node->m_calibr_hv_status, m_fld_fw_hv_max)) {
        enough = true;
    }
    DBGF(3, "hv_tst: %d", node->m_calibr_hv_tst);
    if (!check_field_ack(node->code(), node->m_calibr_hv_ack, node->m_calibr_hv_status, m_fld_fw_hv_tst)) {
        enough = true;
    }

    if (enough) {
        return;
    }

    m_fld_fw_hv_min->setText(QString::number(node->m_calibr_hv_min, 10));
    m_fld_fw_hv_max->setText(QString::number(node->m_calibr_hv_max, 10));
    m_fld_fw_hv_tst->setText(QString::number(node->m_calibr_hv_tst, 10));
}

void FIRMWARE_GUI::update_field_0x43()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_hv_ack, node->m_hv_status, m_fld_fw_hv_opr))
        return;
    m_fld_fw_hv_opr->setText(QString::number(node->m_operating_hv, 10));
}

void FIRMWARE_GUI::update_field_0x44()
{
    DBG(3);
    // auto *node = m_fw->nodeE4();
    // auto txt = QString("%1").arg(node->m_hv_corr, 2, 16, QChar('0')).toUpper();
    // m_fld_fw_HV_corr->setText(txt);
}

void FIRMWARE_GUI::update_field_0x46()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_warmup_ack, node->m_warmup_status, m_fld_fw_warmuptime))
        return;
    auto txt = QString("%1 (%2 %3)").arg(node->m_warmup_num).arg(node->m_warmup_time, 0, 'f', 1).arg(tr("sec"));
    m_fld_fw_warmuptime->setValue(node->m_warmup_num);
    m_fld_fw_warmuptime->setToolTip(txt);
}

void FIRMWARE_GUI::update_field_0x50()
{
    DBG(3);
    refresh_ctable();
}

void FIRMWARE_GUI::update_field_0x51()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_gravity_ack, node->m_gravity_status, m_fld_fw_grav_field))
        return;
    m_fld_fw_grav_field->setValue(node->m_gravity);
}

void FIRMWARE_GUI::update_field_0x1e(byte ext)
{
    DBG(3);
    auto *node = m_fw->nodeE4();
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

    if (!check_field_ack(node->code(), node->m_adc_ack[ext], node->m_adc_status[ext], m_fld_fw_ADC[ext]))
        return;
    m_fld_fw_ADC[ext]->setText(QString::number(value, 'f', 6));
    m_fld_fw_ADC[ext]->setToolTip(QString::number(value, 'f', 6));
}

void FIRMWARE_GUI::update_field_0x1f()
{
    DBG(3);
    //auto *node = m_fw->nodeE4();
    //if (!check_field_ack(node->code(), node->m_config_register_ack, node->m_config_register_status, m_fld_fw_cfgreg))
    //    return;
    //auto txt = QStringLiteral("%1").arg(node->m_config_register, 2, 16, QChar('0')).toUpper();
    //m_fld_fw_cfgreg->setText(QString("0x%1").arg(txt));
}

void FIRMWARE_GUI::update_field_0xf1()
{
    DBG(3);
    //auto *node = m_fw->nodeE4();
    //if (!check_field_ack(node->code(), node->m_statusreg_ack, node->m_statusreg_status, m_fld_fw_statusreg))
    //    return;
    //}
    //
    //auto fnt = m_fld_fw_statusreg->font();
    //m_fld_fw_statusreg->setStyleSheet("background: normal;");
    //m_fld_fw_statusreg->setFont(fnt);
    //auto txt = QString("%1").arg(node->m_statusreg, 8, 16, QChar('0')).toUpper();
    //m_fld_fw_statusreg->setText("0x" + txt);
    //QString tip = tr("Bit 31:  Master State"
    //                 "   1 = The Bus Master is running tasks that require data from slave nodes. When this\n"
    //                 "   bit is set, and the rotation state match the log requirement in the recorder task\n"
    //                 "   record is match, the firmware will put the node into power management mode.\n"
    //                 "   0 = The Bus Master is not running any tasks which require slave node data.\n"
    //                 "   Whenever the slave node receive the MSR with this bit clear, it will shut down the\n"
    //                 "   power management function.\n"
    //                 "Bit 30:  Flow State\n"
    //                 "   1 = Flow On has been detected by the Bus Master.\n"
    //                 "   0 = Flow Off has been detected by the Bus Master.\n"
    //                 "Bit 29:  Rotation State\n"
    //                 "   1 = Rotation has been detected by the Bus Master.\n"
    //                 "   0 = Rotation has not been detected by the Bus Master.\n"
    //                 "Bit 28:  Inclination State\n"
    //                 "   1 = High Inclination has been detected by the Bus Master.\n"
    //                 "   0 = Low Inclination has been detected by the Bus Master.\n"
    //                 "Bit 27 - Bit 0\n"
    //                 "   Unassigned\n");
    //m_fld_fw_statusreg->setToolTip(tip);
}

void FIRMWARE_GUI::update_field_0xf8()
{
    DBG(3);
    // TODO - нужно-ли отображение поля 0xf8
    // auto *node = m_fw->nodeE4();
    // auto txt = QStringLiteral("[%1] %2s %3s %4 %5 %6")
    //            .arg(node->m_rectask_num)
    //            .arg(node->m_rectask_initdelay/10.0)
    //            .arg(node->m_rectask_period/10.0)
    //            .arg(node->m_rectask_records)
    //            .arg(node->m_rectask_buckets)
    //            .arg(node->m_rectask_logmode);
}

void FIRMWARE_GUI::update_field_0xfc()
{
    DBG(3);
    auto *node = m_fw->nodeE4();
    if (!check_field_ack(node->code(), node->m_partnumber_ack, node->m_partnumber_status, m_fld_fw_partnumber))
        return;
    QString pn = node->partnumber();
    m_fld_fw_partnumber->setText(pn);
}

bool FIRMWARE_GUI::check_field_ack(byte code, byte ack, ushort status, QLineEdit *field)
{
    DBG(4);
    if (ack == 0 || ack == 1) return true;

    auto fnt = field->font();
    field->setStyleSheet("QLineEdit {background: red; color: white;} QTipLabel {color:black;}");
    field->setFont(fnt);

    auto c_ack    = QString("%1").arg((int)ack, 2, 16, QChar('0')).toUpper();
    auto c_status = QString("%1").arg((int)status, 4, 16, QChar('0')).toUpper();
    QString tip = QString("Ack: 0x%1, Status: 0x%2").arg(c_ack, c_status);
    field->setToolTip(tip);
    field->setText(QString("[%1]").arg(c_ack));

    set_ACK_field_code(c_ack);
    set_ACK_field_status(c_status);
    set_ACK_field_cmd(QString("%1").arg(code, 2, 16, QChar('0')).toUpper(),NodeBA::frame_name(code));
    auto desc = NodeE4::status_description(ack, static_cast<NodeE4::Status>(status));
    auto tips = NodeE4::status_tip(ack, static_cast<NodeE4::Status>(status));
    set_ACK_field_text(desc, tips);
    return false;
}

bool FIRMWARE_GUI::check_field_ack(byte code, byte ack, ushort status, QAbstractSpinBox *field)
{
    DBG(4);
    if (ack == 0 || ack == 1) return true;

    auto fnt = field->font();
    field->setStyleSheet("QLineEdit {background: red; color: white;} QTipLabel {color:black;}");
    field->setFont(fnt);

    auto c_ack    = QString("%1").arg((int)ack, 2, 16, QChar('0')).toUpper();
    auto c_status = QString("%1").arg((int)status, 4, 16, QChar('0')).toUpper();
    QString tip = QString("Ack: 0x%1, Status: 0x%2").arg(c_ack, c_status);
    field->setToolTip(tip);
    field->clear();

    set_ACK_field_code(c_ack);
    set_ACK_field_status(c_status);
    set_ACK_field_cmd(QString("%1").arg(code, 2, 16, QChar('0')).toUpper(), NodeBA::frame_name(code));
    auto desc = NodeE4::status_description(ack, static_cast<NodeE4::Status>(status));
    auto tips = NodeE4::status_tip(ack, static_cast<NodeE4::Status>(status));
    set_ACK_field_text(desc, tips);
    return false;
}
