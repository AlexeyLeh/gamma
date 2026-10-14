/*!
  \file   conn.h
  \author Алексей Лехтин
  \brief  Подключение к БТО, описание интерфейса
  \date   10.05.2023
*/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QIODevice>
#include <QThread>

#include <string>
#include <vector>

#include "dbg.h"
#include "frame.h"
#include "serial_conf.h"
#include "response.h"

class IConn : public QObject
{
    Q_OBJECT;

public:
    explicit IConn() {}
    virtual ~IConn() = default;

public:
    class Params
    {
    public:
        Params()
        {}

        Params(const QString &host, int port, int ms = 3000) :
            host(host), port(port), timeout(ms)
        {}

        Params(const port_t &port) :
            port_settings(port)
        {}

        virtual ~Params() = default;
    public:
        QString     host;
        int         port;
        int         timeout; // ms
        port_t      port_settings;
    };

public:
    virtual const   QString &name() const = 0;
    virtual bool    isConnected() = 0;
    virtual int     state() const = 0;
    virtual int     error() const = 0;
    virtual QString error_string() const = 0;
    virtual QString error_reason(int error) const = 0;

signals:
    void signal_connect_to(IConn::Params params);
    void signal_conn_connected();
    void signal_conn_disconnected();
    void signal_conn_connection_canceled();
    void signal_conn_connection_timeout();
    void signal_conn_got_error();
    void signal_conn_got_bytes(QByteArray data, QDateTime dt);
    void signal_finished();

public slots:
    virtual void slot_connect_to(IConn::Params params) = 0;
    virtual void slot_disconnect_from() = 0;
    virtual void slot_cancel_connection() = 0;
    virtual void slot_conn_do_request(Adam::Frame request_frame) = 0;

public:

    const IConn::Params &params() const
    {
        return m_params;
    }

    void set_params(const IConn::Params &params)
    {
        m_params = params;
    }

    virtual void read_data(QIODevice *device)
    {
        if (!device) return;
        // формат ожидаемого пакета данных:
        // [0]to [1]from [2-3]len [4]code {[5]cmd ack [6]data ...} [len-2]crc
        QDateTime dt = QDateTime::currentDateTime();
        QByteArray data = device->readAll();
        m_data.append(data);
        //DBG(4, QString("got %1 - '%2'").arg(data.size()).arg(data.toHex().constData()));

        //--------------------------------------------------------
        while(m_data.size() >= 7) // минимальная длина 'целого' пакета
        {
            quint16 len = (m_data.at(2) << 8) | (m_data.at(3) & 0x00FF);
            len |= (m_data.at(3) & 0x00FF);

            if(len == 0) // неверные данные
            {
                DBG(3, QString("len:%1 - incorrect data").arg(len));
                DBG(3, QString("emit signal_conn_got_bytes('%1', %2)")
                        .arg(m_data.constData()).arg(dt.toString("hh:mm:ss.zzz")));
                emit signal_conn_got_bytes(m_data, dt);
                m_data.clear();
                return;
            }

            if(m_data.size() == len) // принят один полный пакет данных
            {
                DBG(4, QString("got whole packet: len: %1, %2")
                    .arg(len).arg(m_data.toHex().constData()));
                DBG(4, QString("emit signal_conn_got_bytes('%1', %2)")
                        .arg(m_data.toHex().constData()).arg(dt.toString("hh:mm:ss.zzz")));
                emit signal_conn_got_bytes(m_data, dt);
                m_data.clear();
                return;
            }

            if(m_data.size() < len)
            {
                DBG(4, QString("not enough data - total in buf: len: %1, %2")
                    .arg(m_data.size()).arg(m_data.toHex().constData()));
                return; // принят не полный пакет данных
            }

            // в данных может быть несколько пакетов, как целиком, так и "без хвостов"
            QByteArray buf = m_data.left(len);
            DBG(4, QString("requested len: %1 - accepted packet: %2")
                .arg(len).arg(buf.toHex().constData()));
            DBG(4, QString("emit signal_conn_got_bytes('%1', %2)")
                    .arg(m_data.constData()).arg(dt.toString("hh:mm:ss.zzz")));
            emit signal_conn_got_bytes(buf, dt);
            m_data = m_data.mid(len);
            DBG(4, QString("next data for processing: len: %1, %2")
                .arg(m_data.size()).arg(m_data.toHex().constData()));
        }

        DBG(4, QString("total in buf: %1 [%2] - too little data")
            .arg(m_data.size()).arg(m_data.toHex().constData()));
    }

public:
    inline static void delay(int ms)
    {
        QDateTime tim_cur = QDateTime::currentDateTime();
        QDateTime tim_end = tim_cur.addMSecs(ms);
        do {
            QCoreApplication::processEvents();
            QCoreApplication::sendPostedEvents();
            tim_cur = QDateTime::currentDateTime();
        } while(tim_cur < tim_end);
    }

private:
    QByteArray      m_data;   // буфер приема данных с порта
    IConn::Params   m_params;
};

#endif // __CONNECTION_H__
