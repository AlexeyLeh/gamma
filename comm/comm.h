/*!
  \file   comm.h
  \author Aleksey Lekhtin
  \date   10.05.2023
  \brief  Коммуникация с устройствами
*/

#ifndef __COMM_H_
#define __COMM_H_

#include <mutex>

#include <QAbstractSocket>
#include <QObject>
#include <QSettings>
#include <QTimer>
#include <QDateTime>
#include <QThread>

#include "conn.h"
#include "NodeBA.h"
#include "NodeE4.h"
#include "frame.h"
#include "response.h"

#include "serial_conf.h"
#include "serial_port.h"

////class IConn;
class APP;
class MW;
class Worker;

//---------------------------------------------------------
class COMM : public QObject
{
    Q_OBJECT;

public:
    static constexpr int TS_50ms  = 50; // ms
    static constexpr int TS_100ms = 100;
    static constexpr int TS_200ms = 200;
    static constexpr int TS_500ms = 500;

    public:
    explicit COMM(APP *m);
    ~COMM() override;

public:
    static constexpr const char* const default_ConnectionType = "eth";
    static constexpr const char* const default_ConnectionHost = "192.168.1.250";
    static constexpr int        default_ConnectionPort = 4001;
    static constexpr int        default_ConnectionWait = 3000;

public:
    enum class CONNECT_TYPE
    {
        NONE,
        COMPORT,
        ETHERNET
    };
    Q_ENUM(CONNECT_TYPE)

public:
    MW              *mw() const;
    void            connect_to();
    void            disconnect_from();
    void            cancel_connection();
    bool            isConnected();
    int             state() const;
    QString         state_string() const;
    const QString   name() const;
    const QString   &host() const;
    int             port() const;
    int             timeout() const;
    void            set_host(const QString &host);
    void            set_port(int port);
    SERIALPORT      *sp();
    NodeBA          *nodeBA() const;
    NodeE4          *nodeE4() const;

    void            read_settings(QSettings *st);
    void            save_settings(QSettings *st);
    CONNECT_TYPE    type() const;
    void            set_type(const COMM::CONNECT_TYPE type);
    void            set_port_settings(const port_t &port);

    int             error() const;
    QString         errorString() const;
    QString         errorReason() const;
    QString         errorReason(int err) const;

    int             push_request(const Adam::Frame &frame);
    int             push_requests(const std::list<Adam::Frame> &frames);
    int             number_of_requests();
    void            clear_requests();

private:
    void            clear_error();

private slots:
    void            slot_conn_connected();
    void            _conn_connected_0();
    void            slot_conn_disconnected();
    void            slot_conn_connection_timeout();
    void            slot_conn_connection_canceled();
    void            slot_conn_got_error();
    void            slot_got_bytes(const QByteArray &ba, const QDateTime &dt);
    void            slot_do_requests();
    ////void            slot_resultReady(bool ok);

signals:
    void            signal_comm_ethernet_connected();
    void            signal_comm_ethernet_disconnected();
    void            signal_comm_ethernet_canceled();

    void            signal_comm_comport_connected();
    void            signal_comm_comport_disconnected();
    void            signal_comm_comport_canceled();
    void            signal_comm_connection_error();

    void            signal_comm_not_responded(NODECODE nodeCode, QString err_string);
    void            signal_comm_showstatus(const QString &msg, int sec);
    void            signal_comm_NodeBA_responded(byte cmd, byte ext);
    void            signal_comm_NodeE4_responded(byte cmd, byte ext);
    void            signal_comm_requests_completed(int nodecode);
    void            signal_comm_incorrect_resonse(const Adam::Response &response, const QString &msg);
    void            signal_comm_connect_to(IConn::Params params);
    void            signal_comm_disconnect_from();
    void            signal_comm_cancel_connection();
    void            signal_do_request(Adam::Frame request_frame);


private:
    APP            *m_app; // манагер глобальных ресурсов программы
    IConn           *m_conn{};
    QThread         *m_thread;
    int             m_conn_error;
    QString         m_conn_error_string;
    std::mutex      m_lock_queue;
    bool            m_timer_graceful_shutdown{false};
    bool            m_stop_requests{false};
    bool            m_clear_requests{false};
    QTimer          m_timer_requests;
    byte            m_got_response;
    QDateTime       m_time_request_begin;
    QDateTime       m_time_request_end;
    std::list<Adam::Frame>  m_request_queue;

    // settings from .ini
private:
    SERIALPORT      *m_sp;              // параметры com-порта
    CONNECT_TYPE    m_connection_type;
    QString         m_host;
    int             m_port;
    int             m_timeout;          // ms
    int             m_default_rtime;    // ReadTime - время ожидания при чтении с Adam-устройства (msec)
};

//---------------------------------------------------------
QDebug operator<<(QDebug d, const COMM::CONNECT_TYPE & status);

#endif // __COMM_H_
