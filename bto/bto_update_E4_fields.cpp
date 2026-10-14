/*!
  \file   bto_update_E4_fields.cpp
  \author Aleksey Lekhtin
  \date   28.02.2024
  \brief  Отображение значений гамма-модуля на БТО экране
*/

#include <QString>

#include "bto.h"
#include "bto_gui.h"
#include "dbg.h"

//---------------------------------------------------------
void BTO_GUI::update_nodeE4_fields(byte cmd, byte ext)
{
    Q_UNUSED(ext);
    if (mw()->current_tab() != MW::PAGES::BTO_PAGE)
        return;

    switch (cmd)
    {
        case 0x01:                 // Read Gamma Ray Data
            update_E4_field_0x01();
            break;
        case 0x05:                 // Read Temperature Data
            update_E4_field_0x05();
            break;
        case 0x43:                 // Read Voltage
            update_E4_field_0x43();
            break;
        case 0x1e:                 // Read the MCU's A/D converter
            update_E4_field_0x1e(ext);
            break;
        case 0xfc:                 // PartNumber
            update_E4_field_0xfc();
            break;
        default:
            DBG(0, QString("unregistered cmd: 0x%1").arg(cmd, 2, 16, QChar('0')));
            break;
    }
}

//---------------------------------------------------------
void BTO_GUI::update_E4_field_0x01()
{
    // Gamma Ray Data
    auto *node = m_bto->nodeE4();
    auto value = node->m_gamma_value;
    //QString txt = QString("[%1]%2").arg(node->m_gamma_num).arg(value, 0, 'f', 2);
    QString txt = QString("%1").arg(value, 0, 'f', 2);
    m_field_gamma_gamma->setText(txt);
}

//---------------------------------------------------------
void BTO_GUI::update_E4_field_0x05()
{
    // Temperature Data
    auto *node = m_bto->nodeE4();
    auto value = node->m_temperature_value;
    QString txt = QString::number(value, 'f', 2);
    m_field_gamma_temp->setText(txt);
}

//---------------------------------------------------------
void BTO_GUI::update_E4_field_0x43()
{
    // Read Voltage
    auto *node = m_bto->nodeE4();
    auto value = node->m_operating_hv;
    m_field_gamma_voltage->setText(QString::number(value));
}

//---------------------------------------------------------
void BTO_GUI::update_E4_field_0x1e(byte ext)
{
    // the MCU's A/D converter
    auto *node = m_bto->nodeE4();
    float value;
    if (ext == 0)
    {
        value = node->m_voltage15;
        m_field_gamma_15VDC->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 1)
    {
        value = node->m_voltage5;
        m_field_gamma_5VDC->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 2)
    {
        value = node->m_dv485;
        m_field_gamma_DV485->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 3)
    {
        value = node->m_high_voltage_control;
        m_field_gamma_HV_raw->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 4)
    {
        value = node->m_temperature_sensor;
        m_field_gamma_TempV->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 5)
    {
        value = node->m_temperature_circuit_calibration;
        m_field_gamma_TempVCal->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 6)
    {
        value = node->m_vdc25_reference;
        m_field_gamma_25Ref->setText(QString::number(value, 'f', 2));
    }
    else if (ext == 7)
    {
        value = node->m_power_amperage;
        //m_field_gamma_TempV->setText(QString::number(value, 'f', 2));
    }
}

//---------------------------------------------------------
void BTO_GUI::update_E4_field_0xfc()
{
    // PartNumber
    auto *node = m_bto->nodeE4();
    m_bto->set_gamma_module_pn(node->m_partnumber);
    DBG(3, m_bto->gamma_module_pn());
    if (m_bto->gamma_module_pn().isEmpty()) {
        m_bto->set_gamma_module_pn("000000000");
        DBG(3, m_bto->gamma_module_pn());
    }
    m_field_part_number->setText(m_bto->gamma_module_pn());
    mw()->update_statusbar_label_text();
}

//---------------------------------------------------------
void BTO_GUI::clear_nodeE4_fields()
{
    m_field_gamma_15VDC->clear();
    m_field_gamma_5VDC->clear();
    m_field_gamma_DV485->clear();
    m_field_gamma_HV_raw->clear();
    m_field_gamma_TempV->clear();
    m_field_gamma_TempVCal->clear();
    m_field_gamma_25Ref->clear();

    m_field_gamma_gamma->clear();
    m_field_gamma_temp->clear();
    m_field_gamma_voltage->clear();
}
