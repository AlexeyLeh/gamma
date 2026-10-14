/*!
  \file   comm_settings.cpp
  \author Aleksey Lekhtin
  \brief  Настройки для коммуникации с устройствами
  \date   10.05.2023
*/


#include <QApplication>
#include <QSettings>
#include <QVariant>
#include <QString>

#include <string>

#include "comm.h"
#include "conn_comport.h"
#include "dbg.h"
#include "frame.h"
#include "mainwindow.h"

//---------------------------------------------------------
void COMM::read_settings(QSettings *st)
{
    DBG(4);
    m_default_rtime = st->value(QStringLiteral("NodeE4/RTime"), Adam::NodeE4RTime).toInt(); // msec
    auto qstr = st->value(QStringLiteral("Connection/Type"), QString(default_ConnectionType)).toString().toLower();
    auto str = qstr.left(3);
    if (str == "eth") {
        m_connection_type = COMM::CONNECT_TYPE::ETHERNET;
    } else if (str == "com") {
            m_connection_type = COMM::CONNECT_TYPE::COMPORT;
    } else {
        DBG(0, QStringLiteral("Warning: Unknown connection type '%1' in settings. Fallback to default.").arg(str));
        m_connection_type = COMM::CONNECT_TYPE::COMPORT;
    }

    m_host = st->value("Ethernet/Host", QString(default_ConnectionHost)).toString();
    m_port = st->value("Ethernet/Port", default_ConnectionPort).toInt();
    m_timeout = st->value("Ethernet/Timeout", default_ConnectionWait).toInt();

    if (m_sp) {
        m_sp->read_settings(st);
    }
}

//---------------------------------------------------------
void COMM::save_settings(QSettings *st)
{
    DBG(4);
    st->setValue("NodeE4/RTime", m_default_rtime);
    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        st->setValue("Connection/Type", "ethernet");
    } else {
        st->setValue("Connection/Type", "comport");
    }

    st->setValue("Ethernet/Host", m_host);
    st->setValue("Ethernet/Port", m_port);
    st->setValue("Ethernet/Timeout", m_timeout);

    m_sp->save_settings(st);
}
