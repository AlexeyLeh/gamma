/*!
  \file   conn_com.cpp
  \author Алексей Лехтин
  \brief  Adam, com-порт реализация
  \date   10.05.2023
*/

#include <string>
#include <iostream>

#include <QApplication>
#include <QtDebug>
#include <QDateTime>
#include <QByteArray>
#include <QSerialPort>

#include "conn_comport.h"
#include "response.h"
#include "dbg.h"

//---------------------------------------------------------
ConnComport:: ConnComport() :
    m_qt_serial_port(nullptr)
{
}

//-------------------------------------------------
ConnComport::~ConnComport()
{
    DBG(4);
    if (m_qt_serial_port && m_qt_serial_port->isOpen()) {
        m_qt_serial_port->setRequestToSend(false);
        m_qt_serial_port->close();
    }
    delete m_qt_serial_port;
}

//-------------------------------------------------
QSerialPort *ConnComport::qt_serialport() const
{
    return m_qt_serial_port;
}

//-------------------------------------------------
const QString &ConnComport::name() const
{
    return m_portName;
}

//-----------------------------------------------------------
const port_t &ConnComport::port_settings() const
{
    return params().port_settings;
}

//-------------------------------------------------
int ConnComport::state() const
{
    return (int)m_qt_serial_port->isOpen();
}

//-------------------------------------------------
bool ConnComport::isConnected()
{
    if (!m_qt_serial_port) {
        DBG(4, "no (serial port: null)");
        return false;
    }
    DBGF(4, "serial port: %p, state: %d", m_qt_serial_port, m_qt_serial_port->isOpen());
    return m_qt_serial_port->isOpen();
}

//////-------------------------------------------------
////bool ConnComport::isAlive()
////{
////    return m_alive;
////}

//////-------------------------------------------------
////void ConnComport::set_parameters(IConn::Params params)
////{
////    ////m_parameters = dynamic_cast<const ConnComport::Parameters&>(params);
////    m_parameters = params;
////    auto serialport_name = m_parameters.port.sName;
////    if (serialport_name.isEmpty()) serialport_name = "''";
////
////    m_qt_serial_port->setPortName(m_parameters.port.sName);
////    m_qt_serial_port->setBaudRate(m_parameters.port.baudRate);
////    m_qt_serial_port->setDataBits(m_parameters.port.dataBits);
////    m_qt_serial_port->setStopBits(m_parameters.port.stopBits);
////    m_qt_serial_port->setParity(m_parameters.port.parity);
////    m_qt_serial_port->setFlowControl(m_parameters.port.flowCtrl);
////
////    m_portName = m_qt_serial_port->portName();
////}
//-------------------------------------------------
int ConnComport::error() const
{
    return m_qt_serial_port->error();
}

//-------------------------------------------------
QString ConnComport::error_string() const
{
    return m_qt_serial_port->errorString();
}

//-------------------------------------------------
QString ConnComport::error_reason(int error) const
{
    QString reason;
    switch(error)
    {
    case QSerialPort::NoError:
        reason = tr("No error occurred");
        break;
    case QSerialPort::DeviceNotFoundError:
        reason = tr("An error occurred while attempting to open an non-existing device");
        break;
    case QSerialPort::PermissionError:
        reason = tr("An error occurred while attempting to open an already opened device by another process or a user not having enough permission and credentials to open");
        break;
    case QSerialPort::OpenError:
        reason = tr("An error occurred while attempting to open an already opened device in this object");
        break;
    case QSerialPort::NotOpenError:
        reason = tr("This error occurs when an operation is executed that can only be successfully performed if the device is open.");
        break;
    case QSerialPort::WriteError:
        reason = tr("An I/O error occurred while writing the data");
        break;
    case QSerialPort::ReadError:
        reason = tr("An I/O error occurred while reading the data");
        break;
    case QSerialPort::ResourceError:
        reason = tr("An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system");
        break;
    case QSerialPort::UnsupportedOperationError:
        reason = tr("The requested device operation is not supported or prohibited by the running operating system");
        break;
    case QSerialPort::TimeoutError:
        reason = tr("A timeout error occurred.");
        break;
    case QSerialPort::UnknownError:
        reason = tr("An unidentified error occurred");
        break;
    default:
        reason = tr("An unidentified error occurred");
        break;
    }
    return reason;
}

//---------------------------------------------------------
QString ConnComport::port_info(const port_t &port)
{
    auto name = port.sName;
    if (name.isEmpty())
    {
#ifdef Q_OS_WIN
        name = "COM?";
#else
        name = "/dev/ttyS?";
#endif
    }

    QString str = QString("[%1, %2, %3, %4, %5, %6]")
            .arg(name)
            .arg(port.sBaudRate)
            .arg(port.sDataBits)
            .arg(port.sStopBits)
            .arg(port.sParity)
            .arg(port.sFlowCtrl);
    return str;
}

//---------------------------------------------------------
QString ConnComport::port_info_short(const port_t &port)
{
    auto name = port.sName;
    if (name.isEmpty())
    {
#ifdef Q_OS_WIN
        name = "COM?";
#else
        name = "/dev/ttyS?";
#endif
    }

    //QString str = QString("[%1, %2, %3, %4]")
    //        .arg(name)
    //        .arg(port.sBaudRate)
    //        .arg(port.sDataBits)
    //        .arg(port.sStopBits);
    QString str = QString("[%1, %2]").arg(name).arg(port.sBaudRate);
    return str;
}


//---------------------------------------------------------
void ConnComport::slot_connect_to(IConn::Params parameters)
{
    if (!m_qt_serial_port) {
        m_qt_serial_port = new QSerialPort();

        connect(m_qt_serial_port, SIGNAL(readyRead()), this, SLOT(slot_serialport_ready_read()));
        connect(m_qt_serial_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
                this, SLOT(slot_serialport_error(QSerialPort::SerialPortError)));

        // connect(m_qt_serial_port, SIGNAL(requestToSendChanged(bool)),
        //         this, SLOT(slot_serial_RTS_changed(bool)));

        // connect(m_qt_serial_portS, SIGNAL(parityChanged(QSerialPort::Parity)),
        //         this, SLOT(slot_serial_parity_changed(QSerialPort::Parity)));
    }

    set_params(parameters);

    if (m_qt_serial_port->isOpen())
    {
        DBG(3, QString("%1 already connected - fine").arg(name()));
        IConn::delay(100);
    }

    if (m_qt_serial_port->open(QIODevice::ReadWrite))
    {
        DBG(1, QString("connected to %1 %2 - open: %3")
             .arg(name())
             .arg(m_qt_serial_port->baudRate())
             .arg(m_qt_serial_port->isOpen()));
        m_qt_serial_port->setRequestToSend(false);
        IConn::delay(100);
        return;
    }

    int err = error();
    DBG(1, QString("%1 - %2 - %3")
         .arg(error())
         .arg(error_string())
         .arg(error_reason(err)));
}

//-------------------------------------------------
void ConnComport::slot_cancel_connection()
{
    DBG(3, "nothing...");
}

//-------------------------------------------------
////bool ConnComport::_connect_to()
////{
////    if (m_qt_serial_port->isOpen())
////    {
////        DBG(3, QString("%1 already connected - fine").arg(name()));
////        IConn::delay(100);
////    }
////
////    if (m_qt_serial_port->open(QIODevice::ReadWrite))
////    {
////        DBG(1, QString("connected to %1 %2 - open: %3")
////             .arg(name())
////             .arg(m_qt_serial_port->baudRate())
////             .arg(m_qt_serial_port->isOpen()));
////        m_qt_serial_port->setRequestToSend(false);
////        IConn::delay(100);
////        return true;
////    }
////
////    ////m_alive = false;
////
////    int err = error();
////    DBG(1, QString("%1 - %2 - %3")
////         .arg(error())
////         .arg(error_string())
////         .arg(error_reason(err)));
////    return false;
////}

//////-------------------------------------------------
////bool ConnComport::connect_to()
////{
////    DBG(1, name());
////    bool ok = _connect_to();
////    DBGF(3, "connection is %s", (isConnected() ? "alive" : "not alive"));
////    if (ok) {
////        DBG(3, "emit signal_conn_connected();");
////        emit signal_conn_connected();
////    } else {
////        DBG(1, QString("Couldn't connected"));
////        ////int err = error();
////        ////DBG(1, QString("%1 - connection error - %2 - %3")
////        ////    .arg(port_info_short(m_parameters.port)).arg(error_string()).arg(error_reason(err)));
////        ////DBG(3, "emit signal_conn_disconnected();");
////        ////emit signal_conn_disconnected();
////    }
////    return ok;
////}

//-------------------------------------------------
////void ConnComport::disconnect_from()
////{
////    if (m_qt_serial_port->isOpen()) {
////        DBG(3);
////        ////_disconnect_from();
////        slot_disconnect_from();
////        DBG(3, "emit signal_conn_disconnected()");
////        emit signal_conn_disconnected();
////    }
////}

//-------------------------------------------------
void ConnComport::slot_disconnect_from()
{
    DBG(3, QString("%1 current state:%2, status:%3").arg(name()).arg(state()).arg((state() ? "Open" : "Close")));
    if (m_qt_serial_port->isOpen()) {
        m_qt_serial_port->setRequestToSend(false);
        IConn::delay(100);
        m_qt_serial_port->close();
        DBG(3, "emit signal_conn_disconnected()");
        emit signal_conn_disconnected();
    }
    ////m_alive = false;
    if (DBG::debug_level > 0) {
        if (state()) {
            DBG(3, QString("can't disconnect - state:%1, status:%2").arg(state()).arg((state() ? "Open" : "Close")));
        } else {
            DBG(3, QString("disconnected - state:%1, status:%2").arg(state()).arg((state() ? "Open" : "Close")));
        }
    }
}

//---------------------------------------------------------
void ConnComport::slot_serialport_ready_read()
{
    DBG(4);
    read_data(m_qt_serial_port);
}

//---------------------------------------------------------
void ConnComport::slot_serialport_error(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError) {
        return;
    }
    const auto funcname = QString("%1").arg("ConnComport::slot_serialport_error(Error)", -DBG::FUNCNAMEWIDTH);
    DBG::dbg_print(0, QString("%1 err: %2 ; '%3' ; '%4'\n")
                   .arg(error())
                   .arg(error_string())
                   .arg(error_reason(error())));

    auto msg = QString("%1 %2 ; %3 ; %4").arg(port_info_short(port_settings()))
            .arg(tr("connection error")).arg(error_string()).arg(error_reason(error()));
    if (DBG::debug_level >= 3) {
        DBG::dbg_print(3, QString("%1 signal_conn_got_error()\n").arg(funcname));
    }
    emit signal_conn_got_error();
    if (m_qt_serial_port->isOpen()) {
        m_qt_serial_port->setRequestToSend(false);
    }
}

//-------------------------------------------------
void ConnComport::slot_conn_do_request(Adam::Frame request_frame)
{
    if( !m_qt_serial_port->isOpen() )
    {
        int err = QSerialPort::NotOpenError;
        QString msg = error_reason(err);
        DBG(3, "QSerialPort::NotOpenError");
        DBG(3, QString("err: %1, '%2'").arg(err).arg(msg));
        DBG(3, "emit signal_conn_got_error()");
        ////emit signal_conn_got_error(err, msg);
        emit signal_conn_got_error();
        return;
    }

    DBG(3, request_frame.dump());
    auto request = request_frame.bytes();
    const char *p = reinterpret_cast<const char *>(request.data());
    int sz = request.size();

    if (!m_qt_serial_port->isOpen()) {
        return;
    }
    if (!m_qt_serial_port->setRequestToSend(true))
    {
        DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
        return;
    }

    for(int i = 0; i < sz; ++i)
    {
        if (!m_qt_serial_port->isOpen()) {
            return;
        }

        if (i == 0)
        {
            if (!m_qt_serial_port->setParity(QSerialPort::MarkParity))
            {
                DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
                return;
            }
        }
        else if (i == 1)
        {
            if (!m_qt_serial_port->setParity(QSerialPort::SpaceParity))
            {
                DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
                return;
            }
        }

        if (!m_qt_serial_port->isOpen()) {
            return;
        }

        qint64 sz = m_qt_serial_port->write(p, 1);
        if (sz == -1)
        {
            DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
            return;
        }

        QCoreApplication::sendPostedEvents();
        IConn::delay(6);
        ++p;
    }

    if (!m_qt_serial_port->isOpen()) {
        return;
    }

    if (!m_qt_serial_port->setRequestToSend(false))
    {
        DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
        return;
    }

    if (!m_qt_serial_port->isOpen()) {
        return;
    }

    if (!m_qt_serial_port->setParity(QSerialPort::MarkParity))
    {
        DBG(3, QString("err: %1, '%2'").arg(error()).arg(error_string()));
        return;
    }

    return;
}
