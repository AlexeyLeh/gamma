/*!
  \file   comm.cpp
  \author Aleksey Lekhtin
  \brief  Коммуникация с устройствами
  \date   10.05.2023
*/


#include <QtGlobal>

#include <sstream>
#include <string>

#include "comm.h"
#include "conn_eth.h"
#include "conn_comport.h"
#include "main.h"
#include "mainwindow.h"
#include "NodeE4.h"
#include "serial_port.h"

COMM::COMM(APP *m) :
    m_app(m),
    m_thread(new QThread),
    m_sp(new SERIALPORT()),
    m_connection_type(CONNECT_TYPE::COMPORT)
{
    qRegisterMetaType<IConn::Params>("IConn::Params");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<Adam::Frame>("Adam::Frame");

    DBG(4);
    read_settings(m_app->qSettings());

    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        m_conn = new ConnEthernet();
        m_conn->set_params({ m_host, m_port, m_timeout });
    } else if (m_connection_type == COMM::CONNECT_TYPE::COMPORT) {
        m_conn = new ConnComport();
        m_conn->set_params(sp()->port_settings());
    } else {
        return;
    }

    //-------------------------------------------
    m_conn->moveToThread(m_thread);
    m_thread->start();
    //connect(m_thread, SIGNAL(started()), m_conn, SLOT(start()));
    connect(m_thread, &QThread::started, m_conn, []() {
        DBG(3, "communication thread started");
    });

    connect(m_conn, &IConn::signal_finished, m_thread, &QThread::quit);
    connect(m_conn, &IConn::signal_finished, m_conn, &IConn::deleteLater);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);

    //-------------------------------------------
    connect(this, &COMM::signal_comm_connect_to, m_conn, &IConn::slot_connect_to);
    connect(this, &COMM::signal_comm_disconnect_from, m_conn, &IConn::slot_disconnect_from);
    connect(this, &COMM::signal_comm_cancel_connection, m_conn, &IConn::slot_cancel_connection);

    //-------------------------------------------
    connect(m_conn, SIGNAL(signal_conn_connected()),
            this, SLOT(slot_conn_connected()), Qt::QueuedConnection);

    connect(m_conn, SIGNAL(signal_conn_disconnected()),
            this, SLOT(slot_conn_disconnected()), Qt::QueuedConnection);

    connect(m_conn, SIGNAL(signal_conn_got_error()),
            this, SLOT(slot_conn_got_error()), Qt::QueuedConnection);

    connect(m_conn, SIGNAL(signal_conn_connection_timeout()),
            this, SLOT(slot_conn_connection_timeout()), Qt::DirectConnection);

    connect(m_conn, SIGNAL(signal_conn_connection_canceled()),
            this, SLOT(slot_conn_connection_canceled()), Qt::DirectConnection);

    connect(m_conn, SIGNAL(signal_conn_got_bytes(QByteArray, QDateTime)),
            this, SLOT(slot_got_bytes(QByteArray, QDateTime)), Qt::DirectConnection);

    connect(this, &COMM::signal_do_request, m_conn, &IConn::slot_conn_do_request);

    m_timer_requests.setInterval(TS_100ms);
    connect(&m_timer_requests, &QTimer::timeout, this, &COMM::slot_do_requests, Qt::QueuedConnection);
}

//---------------------------------------------------------
COMM::~COMM()
{
    DBG(4, "m_timer_requests.stop()");
    m_stop_requests = true;;
    m_timer_requests.stop();
    m_timer_graceful_shutdown = true;
    m_thread->quit();
    m_thread->wait();
    m_conn->deleteLater();
    delete m_sp;
}

//-------------------------------------------------
MW *COMM::mw() const
{
    return m_app->mw();
}

//-------------------------------------------------
QString COMM::state_string() const
{
    if (m_conn) {
        if (type() == COMM::CONNECT_TYPE::ETHERNET)
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
            return QString::fromUtf8(metaEnum.valueToKey(m_conn->state()));
        }
        else if (type() == COMM::CONNECT_TYPE::COMPORT)
        {
            // TODO - перенести в IConn.
            return (m_conn->state() ? "Open" : "Close");
        }
    }
    return "Unknow State";
}

//-------------------------------------------------
const QString COMM::name() const
{
    //// if (!m_conn) return {};
    //// return m_conn->name();
    return m_host;
}

//-------------------------------------------------
const QString &COMM::host() const
{
    return m_host;
}

//-------------------------------------------------
int COMM::port() const
{
    return m_port;
}

//-------------------------------------------------
int COMM::timeout() const
{
    return m_timeout;
}

//-------------------------------------------------
void COMM::set_host(const QString &host)
{
    m_host = host;
}

//-------------------------------------------------
void COMM::set_port(int port)
{
    m_port = port;
}

//-------------------------------------------------
int COMM::state() const
{
    if (!m_conn) return {};
    return m_conn->state();
}

//---------------------------------------------------------
COMM::CONNECT_TYPE COMM::type() const
{
    return m_connection_type;
}

//---------------------------------------------------------
void COMM::set_type(const COMM::CONNECT_TYPE type)
{
    m_connection_type = type;
}

//---------------------------------------------------------
void COMM::set_port_settings(const port_t & port)
{
    sp()->set_port_settings(port);
}

//-------------------------------------------------
SERIALPORT *COMM::sp()
{
    return m_sp;
}

//-------------------------------------------------
NodeBA *COMM::nodeBA() const
{
    return m_app->nodeBA();
}

//-------------------------------------------------
NodeE4 *COMM::nodeE4() const
{
    return m_app->nodeE4();
}

//-------------------------------------------------
bool COMM::isConnected()
{
    if (!m_conn) return {};
    return m_conn->isConnected();
}

//-------------------------------------------------
int COMM::error() const
{
    ////if (!m_conn) return -1;
    ////return m_conn->error();
    return m_conn_error;
}

//-------------------------------------------------
QString COMM::errorString() const
{
    ////if (!m_conn) return {};
    ////if (m_conn->error() > 0) {
    ////    return m_conn->error_string();
    ////}
    ////return {};
    if (m_conn_error == 0) {
        return {};
    }
    return m_conn_error_string;
}

//-------------------------------------------------
QString COMM::errorReason() const
{
    if (!m_conn) return {};
    ////return m_conn->error_reason(m_conn->error());
    ////return m_conn->error_reason(m_conn_error);
    return m_conn->error_reason(m_conn_error);
}

//-------------------------------------------------
QString COMM::errorReason(int err) const
{
    if (!m_conn) return {};
    return m_conn->error_reason(err);
}

//-------------------------------------------------
void COMM::clear_error()
{
    m_conn_error = 0;
    m_conn_error_string.clear();
}

//-------------------------------------------------
int COMM::push_request(const Adam::Frame &frame)
{
    return push_requests({frame});
}

//-------------------------------------------------
int COMM::push_requests(const std::list<Adam::Frame> &frames)
{
    std::lock_guard<std::mutex> lock(m_lock_queue);
    if (!isConnected()) {
        DBG(1, "(!) no connection - skip");
        return 0;
    }
    if (frames.empty() || m_request_queue.size() > 100) {
        return 0; // to be safe
    }

    std::list<Adam::Frame> queue;
    int rtime = 0; // ожидаемое максимальное время выполнения команды в гамма-модуле
    for (const auto &frame : frames)
    {
        rtime += frame.rtime();
        queue.emplace_back(frame);
    }

    if (DBG::debug_level >= 3) {
        auto buf = QString("%1:").arg(frames.size());
        for (const auto &frame : frames)
        {
            buf += QString(" %1:%2%3")
                    .arg(QString("%1").arg(frame.to(), 2, 16, QChar(u'0')).toUpper())
                    .arg(frame.payload().size() == 0 ? "R:": "W:")
                    .arg(frame.cmd(), 2, 16, QChar(u'0'));
        }
        DBG(3, buf);
    }

    m_request_queue.insert(m_request_queue.end(), queue.begin(), queue.end());
    return rtime;
}

//-------------------------------------------------
int COMM::number_of_requests()
{
    std::lock_guard<std::mutex> lock(m_lock_queue);
    return m_request_queue.size();
}

//-------------------------------------------------
void COMM::clear_requests()
{
    DBG(4);
    m_clear_requests = true;
    QApplication::processEvents();
}

//-------------------------------------------------
void COMM::connect_to()
{
    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        IConn::Params params { m_host, m_port, m_timeout };
        DBGF(3, "emit signal_connect_to(%s, %d, %d)", qPrintable(m_host), m_port, m_timeout);
        emit signal_comm_connect_to(params);
    } else if (m_connection_type == COMM::CONNECT_TYPE::COMPORT) {
        IConn::Params params { m_sp->port_settings() };
        DBGF(3, "emit signal_connect_to(%s)", qPrintable(m_sp->port_settings().sName));
        emit signal_comm_connect_to(params);
    }
}

//-------------------------------------------------
void COMM::disconnect_from()
{
    DBG(4, "m_timer_requests.stop()");
    m_timer_requests.stop();
    clear_requests();
    DBG(3, "emit signal_comm_disconnect_from()");
    emit signal_comm_disconnect_from();
}

//-------------------------------------------------
void COMM::cancel_connection()
{
    DBG(3, "emit signal_comm_cancel_connection()");
    emit signal_comm_cancel_connection();
}

//-------------------------------------------------
void COMM::_conn_connected_0()
{
    //// IConn::delay(1000);
    DBGF(1, "connected now - %s", (isConnected() ? "yes" : "no"));
    if (isConnected()) {
        DBG(3, QString("state:%1, status: %2").arg(state()).arg(state_string()));
        if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
            DBG(2, "emit signal_comm_ethernet_connected()");
            emit signal_comm_ethernet_connected();
        } else {
            DBG(2, "emit signal_comm_comport_connected()" );
            emit signal_comm_comport_connected();
        }

        DBGF(2, "m_timer_requests.start(%dms) [1]", COMM::TS_200ms);
        m_timer_requests.start(COMM::TS_200ms);
    }
}

//-------------------------------------------------
void COMM::slot_conn_connected()
{
    m_conn_error = m_conn->error();
    m_conn_error_string = m_conn->error_string();
    QTimer::singleShot(500, this, &COMM::_conn_connected_0);
}

//-------------------------------------------------
void COMM::slot_conn_disconnected()
{
    m_conn_error = m_conn->error();
    DBG(3, QString("\nstate: %1 (%2), err: %3 (%4)")
        .arg(state())
        .arg(state_string())
        .arg(m_conn_error)
        .arg(m_conn_error_string));

    if (m_conn_error == -1) {
        m_conn_error = 0;
        m_conn_error_string = "";
    } else {
        m_conn_error_string = m_conn->error_string();
    }

    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        DBG(3, QString("emit signal_comm_ethernet_disconnected()"));
        emit signal_comm_ethernet_disconnected();
    } else {
        DBG(3, QString("emit signal_comm_comport_disconnected()"));
        emit signal_comm_comport_disconnected();
    }
    m_timer_requests.stop();
    clear_requests();
}

//-------------------------------------------------
void COMM::slot_conn_connection_canceled()
{
    m_conn_error = QAbstractSocket::SocketTimeoutError;
    m_conn_error_string = m_conn->error_reason(m_conn_error);
    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        DBG(3, QString("emit signal_comm_ethernet_canceled()"));
        emit signal_comm_ethernet_canceled();
    }
}

//-------------------------------------------------
void COMM::slot_conn_connection_timeout()
{
    m_conn_error = QAbstractSocket::SocketTimeoutError;
    m_conn_error_string = m_conn->error_reason(m_conn_error);
    if (m_connection_type == COMM::CONNECT_TYPE::ETHERNET) {
        DBG(3, QString("emit signal_comm_ethernet_disconnected()"));
        emit signal_comm_ethernet_disconnected();
    }
}

//-------------------------------------------------
void COMM::slot_conn_got_error()
{
    DBG(4, "stop requests timer");
    m_timer_requests.stop();
    clear_requests();
    m_conn_error = m_conn->error();
    m_conn_error_string = m_conn->error_string();
    DBG(3, QString("err: %1, '%2'").arg(m_conn_error).arg(m_conn_error_string));
    DBG(3, QString("emit signal_comm_connection_error()"));
    emit signal_comm_connection_error();
}

//-------------------------------------------------
void COMM::slot_do_requests()
{
    std::unique_lock<std::mutex> lock(m_lock_queue);
    if (m_request_queue.empty()) {
        DBG(5, QString("requests: %1").arg(m_request_queue.size()));
        lock.unlock();
        m_timer_requests.start(COMM::TS_200ms);
        return;
    }
    clear_error();

    DBG(5, "\nm_timer_requests.stop()");
    m_timer_requests.stop();
    m_stop_requests = false;
    m_clear_requests = false;

    std::list<Adam::Frame> queue;
    queue.swap(m_request_queue);
    lock.unlock();
    Adam::Frame frame;

    auto all = queue.size();
    size_t cntr = 0;
    while(queue.size() > 0) {

        frame = queue.front();
        queue.pop_front();
        ++cntr;

        if (m_timer_graceful_shutdown) {
            DBG(2, "graceful shutdown [1]\n");
            return;
        }

        if (m_clear_requests) {
            m_clear_requests = false;
            DBG(2, "clear requests [1]\n");
            queue.clear();
            clear_error();
            m_timer_requests.start();
            return;
        }

        if (m_stop_requests) {
            DBG(2, "[1] stop requests");
            m_stop_requests = false;
            queue.clear();
            clear_error();
            {
                std::lock_guard<std::mutex> lock(m_lock_queue);
                m_request_queue.clear();
            }
            m_timer_requests.stop();
            DBG(2, "emit signal_comm_requests_completed() [1]");
            emit signal_comm_requests_completed(frame.to());
            return;
        }

        DBG(3, QString("\n%1/%2: to [%3] cmd:%4 %5 %6 (%7ms)")
            .arg(cntr)
            .arg(all)
            .arg(QString("%1").arg(frame.to(), 2, 16, QChar('0')).toUpper())
            .arg(frame.cmd(), 2, 16, QChar('0'))
            .arg(frame.name(), -20)
            .arg(frame.dump())
            .arg(frame.rtime()));

        if (frame.to() == (int)NodeE4::CODExE4) {
            if (frame.rtime() < m_default_rtime) {
                frame.set_rtime(m_default_rtime);   // ожидаемое максимальное время ответа
            }
        }

        m_got_response = 0; // ожидается установки в NODE::CODE в процедуре ответа slot_comm_got_bytes()
        DBG(4, "emit signal_do_request(frame)");
        emit signal_do_request(frame);

        //---------------------------------------
        // ожидание ответа либо таймаут ожидания
        {
            m_time_request_begin = QDateTime::currentDateTime();
            int wait_time = (COMM::TS_200ms > frame.rtime() ? COMM::TS_200ms : frame.rtime());
            mw()->centralWidget()->blockSignals(true);

            QEventLoop loop;

            // 1. Создаем таймер общего таймаута
            QTimer timeoutTimer;
            timeoutTimer.setSingleShot(true);
            QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
            timeoutTimer.start(wait_time);

            // 2. Создаем быстрый таймер опроса флага ответа
            QTimer pollTimer;
            QObject::connect(&pollTimer, &QTimer::timeout, &loop, [this, &loop]() {
                if (this->m_got_response != 0) {
                    loop.quit(); // Если железо ответило, МГНОВЕННО прерываем сон локального цикла!
                }
            });
            pollTimer.start(10); // Частый опрос флага в фоне

            // 3. Запускаем локальный цикл. Поток засыпает с 0% нагрузки на CPU,
            // но фоновые сигналы сокетов/портов продолжают приниматься!
            loop.exec();

            // Обязательно останавливаем таймеры после выхода из loop
            timeoutTimer.stop();
            pollTimer.stop();

            mw()->centralWidget()->blockSignals(false);
        }

        //---------------------------------------
        if (m_stop_requests || m_clear_requests) {
            continue;
        }

        if (m_got_response == 0) {
            DBG(3, QString("%1 [%2/%3ms]")
                .arg("response timed out")
                .arg(m_time_request_begin.msecsTo(QDateTime::currentDateTime()))
                .arg(frame.rtime()));

            QString msg;
            if (frame.to() == NodeE4::CODExE4) {
                msg = tr("Gamma tool is not responding");
            }
            else if (frame.to() == NodeBA::CODExBA) {
                msg = tr("Service Unit is not responding");
            }
            else if (frame.to() == NodeBA::CODEx7A) {
                msg = tr("Service Unit is not responding");
            } else {
                msg = tr("Device not responding");
            }

            DBG(2, QString("[%1] '%2'").arg(frame.to(), 2, 16, QChar('0')).arg(msg));
            DBG(2, QString("emit signal_comm_not_responded(%1, msg)")
                .arg(frame.to(), 2, 16, QChar('0')));
            emit signal_comm_not_responded((NODECODE)frame.to(), msg);

            {
                std::lock_guard<std::mutex> lock(m_lock_queue);
                m_request_queue.clear();
            }
            DBGF(4, "m_timer_requests.start(%dms) [3]", COMM::TS_200ms);
            m_timer_requests.start(COMM::TS_200ms);
            return;
        }
        IConn::delay(100);

    } // while()

    DBGF(4, "m_timer_requests.start(%dms) [3]", COMM::TS_200ms);
    m_timer_requests.start(COMM::TS_200ms);

    DBG(3, "emit signal_comm_requests_completed() [2]");
    emit signal_comm_requests_completed(frame.to());
    DBGNL(2);
}

//-------------------------------------------------
void COMM::slot_got_bytes(const QByteArray &ba, const QDateTime &dt)
{
    clear_error();
    m_time_request_end = dt;
    const std::vector<byte> bytes(ba.begin(), ba.end());
    if (ba.isEmpty()) {
        return;
    }

    byte to = bytes.at(0);
    if (to != 0xff) {
        DBG(4, QString("\necho - skip [%1]").arg(ba.toHex().constData()));
        return;  // это echo из comport-а
    }

    auto response = Adam::Response(bytes);
    response.set_elapsed_time(m_time_request_begin.msecsTo(m_time_request_end));
    if (response.from() == NodeBA::CODExBA) {
        response.set_frame_name(NodeBA::frame_name(response.cmd()));
    } else if (response.from() == NodeBA::CODEx7A) {
        response.set_frame_name(NodeBA::frame_name(response.cmd()));
    } else if (response.from() == NodeE4::CODExE4) {
        response.set_frame_name(NodeE4::frame_name(response.cmd()));
    } else {
        response.set_frame_name(QStringLiteral("UNKN"));
    }

    if (response.error().has_value()) {
        m_got_response = response.from();
        auto err = response.error();
        DBG(1, QString("ERROR response: %1 - %2 - %3")
            .arg(err.value()->e)
            .arg(err.value()->s.c_str())
            .arg(Adam::Frame::dump(bytes)));

        QString nodealert;
        if (response.from() != NodeBA::CODExBA &&
                response.from() != NodeBA::CODEx7A &&
                response.from() != NodeE4::CODExE4) {
            nodealert = tr("Incorrect node number");
        }

        auto msg = QString("%1\n\n%2 [%3] %4 [%5]\n%6\n\n%7")
                .arg(QString::fromStdString(err.value()->s))
                .arg(tr("From"))
                .arg(response.from(), 0, 16)
                .arg(tr("To"))
                .arg(response.to(), 0, 16)
                .arg(Adam::Frame::dump(bytes))
                .arg(nodealert);
        DBG(1, msg.replace('\n', "; "));
        DBG(3, QString("emit signal_comm_incorrect_resonse(msg) [1]"));
        emit signal_comm_incorrect_resonse(response, msg);
        return;
    }

    auto cmd = response.cmd();
    auto ext = response.ext();
    auto from = response.from();

    QString from_text;
    if (DBG::debug_level > 0) {
        from_text = QString("%1").arg(from, 2, 16, QChar('0')).toUpper();
    }

    DBG(4, QString("from  [%1] cmd:%2 %3 ack:%4 status:0x%5 (%6ms) - %7")
        .arg(from_text.toUpper())
        .arg(response.cmd(), 2, 16, QChar('0'))
        .arg(response.name())
        .arg(response.ack(), 2, 16, QChar('0'))
        .arg(response.status(), 4, 16, QChar('0'))
        .arg(response.elapsed_time())
        .arg(response.dump().c_str()));

    if (response.ack() == 1 && (cmd == 0x40 || cmd == 0x41 || cmd == 0x42)) {
        // set HV low, set HV high, set Calibr HV
        // нормальный ответ от операций записи данных в gamma sonde
        /* т.е. ничего */ ;
    } else if (response.ack() != 0) {
        m_got_response = response.from();
        DBG(1, QString("[%1] Error: ACK: %3, Status: 0x%5, Command: 0x%7  %8")
                .arg(QString("%1").arg((int)response.from(), 0, 16, QChar(u' ')).toUpper())
                .arg((int)response.ack(), 0, 16, QChar(u' '))
                .arg((int)response.status(), 2, 16, QChar('0'))
                .arg((int)response.cmd(), 2, 16, QChar('0'))
                .arg(QString::fromStdString(response.dump())));
        auto msg = QString("<html><body>"
                           "%1<br><br>\n"
                           "%2<br><br>\n"
                           "[%3] %4\n\n"
                           "<table>"
                           "<tr><td>ACK:</td><td> %5  </td></tr>\n"
                           "<tr><td>%6: </td><td> 0x%7</td></tr>\n"
                           "<tr><td>%8: </td><td> 0x%9</td></tr>\n"
                           "<tr><td colspan=2>%10</td></tr>\n"
                           "</table></body></html>")
                .arg(NodeBA::frame_name(response.cmd()))
                .arg(NodeE4::status_description(response.ack(), static_cast<NodeE4::Status>(response.status())))
                .arg(QString("%1").arg((int)response.from(), 0, 16, QChar(u' ')).toUpper())
                .arg(tr("ACK Error"))
                .arg((int)response.ack(), 0, 16, QChar(u' '))
                .arg(tr("Status"))
                .arg((int)response.status(), 2, 16, QChar('0'))
                .arg(tr("Command"))
                .arg((int)response.cmd(), 2, 16, QChar('0'))
                .arg(QString::fromStdString(response.dump()));
        DBG(3, QString("emit signal_comm_incorrect_resonse(msg) [2]"));
        emit signal_comm_incorrect_resonse(response, msg);
        return;
    }

    switch (from) {
    case NodeBA::CODExBA:
    case NodeBA::CODEx7A:
        if(nodeBA()->response(response)) {
            DBG(3, QString("emit signal_comm_NodeBA_responded(cmd:%1 ext:%2)")
                .arg(cmd, 2, 16, QChar('0')).arg(ext));
            emit signal_comm_NodeBA_responded(cmd, ext);
        }
        break;
    case NodeE4::CODExE4:
        if (nodeE4()->response(response)) {
            DBG(3, QString("emit signal_comm_NodeE4_responded(cmd:%1, ext:%2)")
                .arg(cmd, 2, 16, QChar('0')).arg(ext));
            emit signal_comm_NodeE4_responded(cmd, ext);
        }
        break;
    default:
        auto msg = QString("%1 [%2]\n\n%3\n%4")
                .arg(tr("The response(s) is received from unknown device"))
                .arg(from_text.toUpper())
                .arg(tr("The connection is reset"))
                .arg(tr("Check the connection address"));
        DBG(0, QString("ERROR - %1").arg(msg).replace('\n', "; "));
        DBG(3, QString("emit signal_comm_incorrect_resonse() [3]"));
        emit signal_comm_incorrect_resonse(response, msg);
        break;
    }

    m_got_response = response.from();
}

//-----------------------------------------------
QDebug operator<<(QDebug d, const COMM::CONNECT_TYPE & type)
{
    if (type == COMM::CONNECT_TYPE::COMPORT) {
        d << QStringLiteral("CONNECT_TYPE::COMPORT");
    }
    else if (type == COMM::CONNECT_TYPE::ETHERNET) {
        d << QStringLiteral("CONNECT_TYPE::ETHERNET");
    } else {
        d << QStringLiteral("CONNECT_TYPE::NONE");
    }
    return d;
}
