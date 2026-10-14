/*!
  \file   bto_update_BA_fields.cpp
  \author Aleksey Lekhtin
  \date   28.02.2024
  \brief  Отображение значений БТО
*/

#include <QString>

#include "bto.h"
#include "bto_gui.h"
#include "dbg.h"
#include "NodeBA.h"

//---------------------------------------------------------
void BTO_GUI::update_NodeBA_fields(byte cmd, byte ext)
{
    Q_UNUSED(ext);
    if (mw()->current_tab() != MW::PAGES::BTO_PAGE)
        return;

    DBGF(3, "cmd:%02X ext:%d", cmd, ext);
    switch (cmd)
    {
    case 0x01:
        update_BA_required_voltage();
        break;
    case 0x02:
        update_poweron_button_state();
        ////update_bto_settings_button();
        break;
    case 0x03:
        update_BA_power_output_sw();
        break;
    case 0x07:
        update_BA_output_voltage();
        break;
    case 0x08:
        update_BA_amperage();
        break;
    case 0xfb:
        update_BA_status();
        break;
    case 0xfc:
        update_BA_partnumber();
        break;
    default:
        DBG(0, QString("unregistered cmd: 0x%1").arg(cmd, 2, 16, QChar('0')));
        break;
    }
}

//---------------------------------------------------------
void BTO_GUI::update_BA_required_voltage()
{
    DBG(4);
    auto *node = m_bto->nodeBA();
    auto str = QStringLiteral("%1 %2").arg(node->m_required_voltage, 6, 'f', 3, QChar(' ')).arg(tr("V"));
    m_field_required_voltage->setText(str);
    if (m_field_requested_voltage->value() == 0) {
        m_field_requested_voltage->setValue(node->m_required_voltage);
    }
}

//---------------------------------------------------------
void BTO_GUI::update_BA_output_voltage()
{
    DBG(4);
    auto *node = m_bto->nodeBA();
    auto str = QStringLiteral("%1 %2").arg(node->m_output_voltage, 6, 'f', 3, QChar(' ')).arg(tr("V"));
    m_field_output_voltage->setText(str);
}

//---------------------------------------------------------
void BTO_GUI::update_BA_amperage()
{
    DBG(4);
    auto *node = m_bto->nodeBA();
    if (m_bto->type() == NodeBA::CODExBA) {
        auto str = QStringLiteral("%1 %2").arg(node->m_output_amperage/1000.0, 6, 'f', 3, QChar(' ')).arg("A");
        m_field_amperage_consumption->setText(str);

    } else if (m_bto->type() == NodeBA::CODEx7A) {
        auto str = QStringLiteral("%1 %2").arg(node->m_output_amperage, 6, 'f', 3, QChar(' ')).arg("A");
        if (m_bto->nodeBA()->power_input_switch() == NodeBA::PowerInputSwitch::ON) {
            m_field_amperage_consumption->setText(str);
        } else {
            m_field_amperage_limit->setText(str);
        }
    }
}

//---------------------------------------------------------
void BTO_GUI::update_BA_status()
{
    DBG(4);
    // auto *node = m_bto->nodeBA();
    // auto str = QStringLiteral("0x%1").arg(node->m_status, 2, QChar('0'));
    // m_field_status->setText(str);
}

//---------------------------------------------------------
void BTO_GUI::update_BA_partnumber()
{
    auto *node = m_bto->nodeBA();
    m_bto->set_part_number(QString::fromStdString(node->m_partnumber));
    mw()->update_statusbar_label_text();
}

//---------------------------------------------------------
void BTO_GUI::update_BA_power_output_sw()
{
    DBG(4);
    auto *node = m_bto->nodeBA();
    auto state = node->m_power_output_type;
    if (state == NodeBA::PowerOutputType::INT || state == NodeBA::PowerOutputType::EXT)
    {
        auto index = power_output_switch_to_index(state);
        auto text = m_field_set_power_output_sw->itemText(index);
        m_field_power_output_sw->setText(text);
    }
    else
    {
        m_field_power_output_sw->clear();
    }
}

//---------------------------------------------------------
void BTO_GUI::clear_NodeBA_fields()
{
    DBG(3);
    auto zeroVstr = QStringLiteral("%1 %2").arg(0, 6, 'f', 3, QChar(' ')).arg("V");
    auto zeroAstr = QStringLiteral("%1 %2").arg(0, 6, 'f', 3, QChar(' ')).arg("A");

    m_field_required_voltage->setText(zeroVstr);
    m_field_output_voltage->setText(zeroVstr);
    m_field_amperage_consumption->setText(zeroAstr);
    m_field_amperage_limit->setText(zeroAstr);
    m_field_power_output_sw->clear();
}

//---------------------------------------------------------
void BTO_GUI::clear_NodeE4_PWR_fields()
{
    DBG(3);
    m_field_amperage_consumption->setText("0.000 A");
    m_field_power_output_sw->clear();
}
