/*!
  \file   comport.cpp
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Описание COM-порта
*/

#include "comport_t.h"
#include "log.h"

constexpr char *com1_linux_name      = "/dev/ttyS0";
constexpr char *com1_msdos_name      = "COM1";

constexpr char *comport_bits         = "Bits";
constexpr char *comport_speed        = "Speed";
constexpr char *comport_stopbits     = "StopBits";

constexpr char *comport_parity_even  = "Even";
constexpr char *comport_parity_odd   = "Odd";
constexpr char *comport_parity_space = "Space";
constexpr char *comport_parity_mark  = "Mark";

constexpr char *comport_flow_rtscts  = "RTS/CTS";
constexpr char *comport_flow_xonxoff = "XON/XOFF";
constexpr char *comport_none_item    = "None";

comport_t::comport_t() :
    m_localEcho(false),
    m_baudRate (QSerialPort::Baud9600),
    m_dataBits (QSerialPort::Data8),
    m_parity   (QSerialPort::NoParity),
    m_stopBits (QSerialPort::OneStop),
    m_flowCtrl (QSerialPort::NoFlowControl),
    m_sName(),
    m_sBaudRate(QString::number(m_baudRate)),
    m_sDataBits(QString::number(m_dataBits)),
    m_sStopBits(QString::number(m_stopBits)),
    m_sParity  (comport_none_item),
    m_sFlowCtrl(comport_none_item)
{
    LOG::dbg4("%s()\n", __FUNCTION__);
}

comport_t::comport_t(const comport_t& port) :
    m_localEcho (port.m_localEcho),
    m_baudRate  (port.m_baudRate),
    m_dataBits  (port.m_dataBits),
    m_parity    (port.m_parity),
    m_stopBits  (port.m_stopBits),
    m_flowCtrl  (port.m_flowCtrl),
    m_sName     (port.m_sName),
    m_sBaudRate (port.m_sBaudRate),
    m_sDataBits (port.m_sDataBits),
    m_sStopBits (port.m_sStopBits),
    m_sParity   (port.m_sParity),
    m_sFlowCtrl (port.m_sFlowCtrl)
{
    LOG::dbg4("%s()\n", __FUNCTION__);
}

comport_t comport_t::operator=(const comport_t& port)
{
    m_baudRate  = port.m_baudRate;
    m_dataBits  = port.m_dataBits;
    m_parity    = port.m_parity;
    m_stopBits  = port.m_stopBits;
    m_flowCtrl  = port.m_flowCtrl;
    m_localEcho = port.m_localEcho;
    m_sName     = port.m_sName;
    m_sBaudRate = port.m_sBaudRate;
    m_sDataBits = port.m_sDataBits;
    m_sStopBits = port.m_sStopBits;
    m_sParity   = port.m_sParity;
    m_sFlowCtrl = port.m_sFlowCtrl;
    return *this;
}

comport_t::~comport_t()
{
    LOG::dbg4("%s() \n", __FUNCTION__);
}

const char *comport_t::def_name()
{
#ifdef Q_OS_WIN
    return com1_msdos_name;
#endif
    return com1_linux_name;
}

QString comport_t::name() const
{
    return m_sName;
}

void comport_t::read_settings(QSettings *st)
{
    if (!st) return;

    st->beginGroup("Comport");
    m_sName    = st->value("Port", comport_t::def_name()).toString();

    m_baudRate = static_cast<QSerialPort::BaudRate>(st->value(comport_speed, 9600).toInt());
    m_dataBits = static_cast<QSerialPort::DataBits>(st->value(comport_bits, 8).toInt());
    m_stopBits = static_cast<QSerialPort::StopBits>(st->value(comport_stopbits, 1).toInt());

    int num = 0;
    auto str = st->value("Parity", QString(comport_parity_mark)).toString().toLower();
    if      (str == QString(comport_parity_even ).toLower()) num = 2;
    else if (str == QString(comport_parity_odd  ).toLower()) num = 3;
    else if (str == QString(comport_parity_space).toLower()) num = 4;
    else if (str == QString(comport_parity_mark ).toLower()) num = 5;
    m_parity = static_cast<QSerialPort::Parity>(num);
    // УТОЧНЕНИЕ - всегда должен быть SpaceParity!
    m_parity = QSerialPort::SpaceParity;

    num = 0;
    str = st->value("FlowControl", QString(comport_none_item)).toString().toUpper();
    if      (str == QString(comport_flow_rtscts ).toLower()) num = 1;
    else if (str == QString(comport_flow_xonxoff).toLower()) num = 2;
    m_flowCtrl  = static_cast<QSerialPort::FlowControl>(num);
    // УТОЧНЕНИЕ - всегда должен быть NoFlowControl!
    m_flowCtrl  = QSerialPort::NoFlowControl;
    st->endGroup();

    m_sBaudRate = QString::number(m_baudRate);
    m_sDataBits = QString::number(m_dataBits);
    m_sStopBits = QString::number(m_stopBits);

    if      (m_parity == 2) m_sParity = comport_parity_even;
    else if (m_parity == 3) m_sParity = comport_parity_odd;
    else if (m_parity == 4) m_sParity = comport_parity_space;
    else if (m_parity == 5) m_sParity = comport_parity_mark;
    else m_sParity = comport_none_item;

    if      (m_flowCtrl == 1) m_sFlowCtrl = comport_flow_rtscts;
    else if (m_flowCtrl == 2) m_sFlowCtrl = comport_flow_xonxoff;
    else m_sFlowCtrl = comport_none_item;
}

void comport_t::save_settings(QSettings *st)
{
    LOG::dbg4("wcomport_t::%s()\n", __FUNCTION__);
    if (!st) return;
    st->beginGroup("Comport");
    st->setValue("Port", m_sName);
    st->setValue(comport_speed,    m_baudRate);
    st->setValue(comport_bits,     m_dataBits);
    st->setValue(comport_stopbits, m_stopBits);

    // УТОЧНЕНИЕ - всегда должен быть SpaceParity!
    // if      (m_parity == 2) st->setValue("Parity", QString(comport_parity_even));
    // else if (m_parity == 3) st->setValue("Parity", QString(comport_parity_odd));
    // else if (m_parity == 4) st->setValue("Parity", QString(comport_parity_space));
    // else if (m_parity == 5) st->setValue("Parity", QString(comport_parity_mark));
    // else st->setValue("Parity", QString(comport_none_item));

    // всегда QSerialPort::NoFlowControl
    // if      (m_flowCtrl == 1) st->setValue("FlowControl", QString(comport_flow_rtscts));
    // else if (m_flowCtrl == 2) st->setValue("FlowControl", QString(comport_flow_xonxoff));
    // else st->setValue("FlowControl", QString(comport_none_item));
    st->endGroup();
}

