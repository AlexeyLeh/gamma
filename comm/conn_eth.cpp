/*!
  \file   conn_eth.cpp
  \author Алексей Лехтин
  \brief  Adam, ethernet реализация
  \date   10.05.2023
*/

#include <iostream>

#include <QApplication>
#include <QDateTime>
#include <QNetworkProxy>
#include <QtDebug>
#include <QTimer>

#include "conn_eth.h"
#include "dbg.h"
#include "response.h"

//-------------------------------------------------
ConnEthernet::ConnEthernet() :
    m_socket(nullptr),
    m_connect_timer(nullptr)
{
    //// m_connect_timer = new QTimer();
    //// m_connect_timer->setSingleShot(true);
    //// connect(m_connect_timer, &QTimer::timeout, this, &ConnEthernet::slot_connecting_timeout);
}

//---------------------------------------------------------
ConnEthernet::~ConnEthernet()
{
    DBG(4);
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->abort();
        delete m_socket;
    }
    if (m_connect_timer) {
        delete m_connect_timer;
    }
}

//-------------------------------------------------
const QString &ConnEthernet::name() const
{
    return params().host;
}

//-------------------------------------------------
int ConnEthernet::port() const
{
    return params().port;
}

//-------------------------------------------------
int ConnEthernet::timeout() const
{
    return params().timeout;
}

//-------------------------------------------------
QTcpSocket *ConnEthernet::socket() const
{
    return m_socket;
}

//-------------------------------------------------
int ConnEthernet::state() const
{
    // QAbstractSocket::SocketState
    return (int)m_socket->state();
}

//-------------------------------------------------
bool ConnEthernet::isConnected()
{
    if (!m_socket) {
        DBG(4, "no (socket: null)");
        return false;
    }
    QString enumstr;
    QDebug(&enumstr) << m_socket->state();
    DBG(4, QString("%1 - %2 (%3)")
        .arg(m_socket->state() == QAbstractSocket::ConnectedState ? "yes" : "no")
        .arg(enumstr)
        .arg((int)m_socket->state()));
    return (m_socket->state() == QAbstractSocket::ConnectedState);
}

//---------------------------------------------------------
////bool ConnEthernet::_connect_to()
////{
////    return true;
////#if 0
////    DBG(3, QString("connect to %1:%2 ...)").arg(name()).arg(port()));
////    m_socket->connectToHost(name(), port(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
////    bool connected = m_socket->waitForConnected(m_parameters.timeout); // ms
////    if (connected) {
////        DBG(3, QString("connected to %1)").arg(name()));
////        return true;
////    }
////
////    int err = error();
////    DBG(1, QString("%1:%2 - %3 - %4")
////         .arg(name())
////         .arg(port())
////         .arg(error_string())
////         .arg(error_reason(err)));
////    m_socket->abort();
////    return false;
////#endif
////}

//////-------------------------------------------------
////bool ConnEthernet::connect_to()
////{
////    DBG(1, name());
////    ////emit signal_connect_to(m_socket, m_parameters);
////    ////emit signal_connect_to2();
////    return true;
////    //// bool ret = _connect_to();
////    //// return ret;
////}

//////-------------------------------------------------
////void ConnEthernet::disconnect_from()
////{
////    DBG(3, name());
////    _disconnect_from();
////}

//////-------------------------------------------------
////void ConnEthernet::_disconnect_from()
////{
////    DBG(3, QString("%1 current state:%2, status:%3").arg(name()).arg(state()).arg((state() ? "Open" : "Close")));
////    if (m_socket->state() != QAbstractSocket::UnconnectedState)
////    {
////        m_socket->disconnectFromHost();
////        IConn::delay(100);
////    }
////}

//-------------------------------------------------
QString ConnEthernet::local_address_connection()
{
    if (m_socket && m_socket->localPort() > 0) {
        auto text = QString("localAddress: '%1:%2', peerAddress: '%3:%4'")
                .arg(m_socket->localAddress().toString()).arg(m_socket->localPort())
                .arg(m_socket->peerAddress().toString()).arg(m_socket->peerPort());
        return text;
    }
   return {};
}

//-------------------------------------------------
int ConnEthernet::error() const
{
    return m_socket->error();
    ////auto err = m_socket->error();
    ////return (int)(err == -1 ? 0 : err);
}

//-------------------------------------------------
QString ConnEthernet::error_string() const
{
    return m_socket->errorString();
}

//-------------------------------------------------
QString ConnEthernet::error_reason(int error) const
{
    QString reason;
    switch(error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        reason = "The connection was refused by the peer (or timed out)";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        reason = "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent";
        break;
    case QAbstractSocket::HostNotFoundError:
        reason = "The host address was not found";
        break;
    case QAbstractSocket::SocketAccessError:
        reason = "The socket operation failed because the application lacked the required privileges";
        break;
    case QAbstractSocket::SocketResourceError:
        reason = "The local system ran out of resources (e.g., too many sockets)";
        break;
    case QAbstractSocket::SocketTimeoutError:
        reason = "The socket operation timed out";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        reason = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)";
        break;
    case QAbstractSocket::NetworkError:
        reason = "An error occurred with the network (e.g., the network cable was accidentally plugged out)";
        break;
    case QAbstractSocket::AddressInUseError:
        reason = "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive";
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        reason = "The address specified to QAbstractSocket::bind() does not belong to the host";
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        reason = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)";
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        reason = "The socket is using a proxy, and the proxy requires authentication";
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        reason = "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket";
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        reason = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)";
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        reason = "Could not contact the proxy server because the connection to that server was denied";
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        reason = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established";
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        reason = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase";
        break;
    case QAbstractSocket::ProxyNotFoundError:
        reason = "The proxy address set with setProxy() (or the application proxy) was not found";
        break;
    case QAbstractSocket::ProxyProtocolError:
        reason = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood";
        break;
    case QAbstractSocket::OperationError:
        reason = "An operation was attempted while the socket was in a state that did not permit it";
        break;
    case QAbstractSocket::SslInternalError:
        reason = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library";
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        reason = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library";
        break;
    case QAbstractSocket::TemporaryError:
        reason = "A temporary error occurred (e.g., operation would block and socket is non-blocking)";
        break;
        //case QAbstractSocket::UnknownSocketError: // -1
        //    reason = "An unidentified error occurred";
    default:
        reason = "An unidentified error occurred";
        break;
    }
    return reason;
}

//////-------------------------------------------------
////bool ConnEthernet::request(const Adam::Frame &request_frame)
////{
////    DBG(4, request_frame.dump());
////    auto request = request_frame.bytes();
////    auto num = m_socket->write((char*)request.data(), request.size());
////    if (num == -1) {
////        QString text;
////        QDebug(&text) << m_socket->error();
////        DBG(1, QString("[%1] %2 - %3").arg(error()).arg(error_string(), text));
////        DBG(3, "emit signal_conn_got_error()");
////        emit signal_conn_got_error(error(), error_string());
////        return false;
////    }
////
////    bool fail = !m_socket->flush();
////    if (fail) {
////        DBG(0, "flush FAILED");
////    }
////    QCoreApplication::sendPostedEvents();
////    return true;
////}

//---------------------------------------------------------
void ConnEthernet::slot_connect_to(IConn::Params parameters)
{
    if (!m_socket) {
        m_socket = new QTcpSocket;
        DBGF(3, "new QTcpSocket created - %p", m_socket);
        connect(m_socket, &QAbstractSocket::connected,    this, &ConnEthernet::slot_connected);
        connect(m_socket, &QAbstractSocket::disconnected, this, &ConnEthernet::slot_disconnected);
        connect(m_socket, &QAbstractSocket::stateChanged, this, &ConnEthernet::slot_stateChanged);
        connect(m_socket, &QAbstractSocket::readyRead,    this, &ConnEthernet::slot_connection_ready_read);
        //connect(m_socket, &QAbstractSocket::errorOccurred, this, &ConnEthernet::slot_connection_error);
    }

    //-------------------------------------------
    auto state = m_socket->state();
    QString enumstr;
    if (DBG::debug_level >= 3) {
        QDebug(&enumstr) << state;
        DBG(3, QString("status:%1 %2").arg((int)state).arg(enumstr));
    }
    while (state != QAbstractSocket::UnconnectedState)
    {
        m_socket->disconnectFromHost();
        QThread::msleep(10);

        state = m_socket->state();
        if (DBG::debug_level >= 3) {
            QDebug(&enumstr) << state;
            DBG(3, QString("status:%1 %2").arg((int)state).arg(enumstr));
        }
    }

    //-------------------------------------------
    set_params(parameters);
    DBG(3, QString("connect to %1:%2").arg(name()).arg(port()));

    if (m_connect_timer) {
        delete m_connect_timer;
    }
    m_connect_timer = new QTimer();
    m_connect_timer->setSingleShot(true);
    connect(m_connect_timer, &QTimer::timeout, this, &ConnEthernet::slot_connecting_timeout);
    m_connect_timer->start(parameters.timeout);

    m_socket->connectToHost(name(), port(), QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
}

//-------------------------------------------------
void ConnEthernet::slot_disconnect_from()
{
    DBG(3, QString("%1 current state:%2, status:%3")
        .arg(name()).arg(state()).arg((state() ? "Open" : "Close")));
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
    {
        m_socket->disconnectFromHost();
        IConn::delay(100);
    }
}

//-------------------------------------------------
void ConnEthernet::slot_connecting_timeout()
{
    DBG(3);
    m_connect_timer->stop(); // for safe
    if (!m_socket) {
        return;
    }

    auto state = m_socket->state();
    if (DBG::debug_level >= 3) {
        state = m_socket->state();
        QString enumstr;
        QDebug(&enumstr) << state;
        DBG(3, QString("status:%1 %2").arg((int)state).arg(enumstr));
    }

    DBG(1, "abort connection...");
    m_socket->abort();
    IConn::delay(100);

    int counter = 20;
    do {
        state = m_socket->state();
        if (DBG::debug_level >= 3) {
            QString enumstr;
            QDebug(&enumstr) << state;
            DBG(3, QString("status:%1 %2").arg((int)state).arg(enumstr));
        }

        if (state == QAbstractSocket::UnconnectedState) {
            DBG(3, "emit signal_conn_stopped()");
            emit signal_conn_connection_timeout();
            return;
        }

        m_socket->abort();
        --counter;
        IConn::delay(100);

    } while(state != QAbstractSocket::UnconnectedState && counter > 0);
    DBG(3, "emit signal_conn_stopped()");
    emit signal_conn_connection_timeout();
}

//-------------------------------------------------
void ConnEthernet::slot_cancel_connection()
{
    m_connect_timer->stop();
    if (!m_socket) {
        DBG(3);
        return;
    }

    auto state = m_socket->state();
    if (state == QAbstractSocket::UnconnectedState) {
        DBG(3, "signal_conn_connection_canceled()");
        emit signal_conn_connection_canceled();
        return;
    }

    if (DBG::debug_level >= 3) {
        state = m_socket->state();
        QString enumstr;
        QDebug(&enumstr) << state;
        DBG(3, QString("status:%1 %2").arg((int)state).arg(enumstr));
    }

    DBG(1, "abort connection...");
    m_socket->abort();
    IConn::delay(100);

    DBG(3, "emit signal_conn_connection_canceled()");
    emit signal_conn_connection_canceled();
}

//-------------------------------------------------
void ConnEthernet::slot_stateChanged(QAbstractSocket::SocketState state)
{
    const auto funcname = QString("%1")
            .arg("ConnEthernet::slot_stateChanged(SocketState)", -DBG::FUNCNAMEWIDTH);
    if (state == QAbstractSocket::ConnectedState) {
        m_connect_timer->stop();
    }

    if (DBG::debug_level >= 3) {

        QString enumstr;
        QDebug(&enumstr) << state;
        DBG::dbg_print(4, QString("%1 %2 %3\n")
                       .arg(funcname)
                       .arg((int)state)
                       .arg(enumstr));

        if (state == QAbstractSocket::ConnectedState|| state == QAbstractSocket::UnconnectedState) {
            auto text = local_address_connection();
            if (!text.isEmpty()) {
                DBG::dbg_print(4, QString("%1 %2\n").arg(funcname).arg(text));
            }
        }
    }
}

//-------------------------------------------------
void ConnEthernet::slot_connected()
{
    DBG(3, "emit signal_conn_connected()");
    emit signal_conn_connected();
}

//-------------------------------------------------
void ConnEthernet::slot_disconnected()
{
    DBG(3, "emit signal_conn_disconnected()");
    emit signal_conn_disconnected();
}

//////-------------------------------------------------
////// TODO 4DEL UNUSED
////void ConnEthernet::slot_socket_error(QAbstractSocket::SocketError socket_error)
////{
////    (void)socket_error;
////    QString enumstr;
////    QDebug(&enumstr) << m_socket->state();
////    DBG(1, QString("[%1] %2 - %3").arg(error()).arg(error_string(), enumstr));
////    DBG(3, "emit signal_conn_got_error()");
////    emit signal_conn_got_error(error(), error_string());
////}

//-------------------------------------------------
void ConnEthernet::slot_connection_ready_read()
{
    DBG(4);
    read_data(m_socket);
}

//-------------------------------------------------
void ConnEthernet::slot_conn_do_request(Adam::Frame request_frame)
{
    DBG(4, request_frame.dump());
    auto request = request_frame.bytes();
    auto num = m_socket->write((char*)request.data(), request.size());
    if (num == -1) {
        QString errstr;
        QDebug(&errstr) << m_socket->error();
        DBG(1, QString("(%1) %2 - %3").arg(m_socket->error()).arg(errstr, m_socket->errorString()));
        DBG(3, "emit signal_conn_got_error()");
        emit signal_conn_got_error();
        return;
    }

    bool fail = !m_socket->flush();
    if (fail) {
        DBG(0, "flush FAILED");
    }
    QCoreApplication::sendPostedEvents();
}
