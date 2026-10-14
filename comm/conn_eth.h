/*!
  \file   conn_eth.h
  \author Алексей Лехтин
  \brief  Подключение к БТО, реализация Ethernet
  \date   10.05.2023
*/

#ifndef __CONNECTION_ETH_H__
#define __CONNECTION_ETH_H__

#include <string>
#include <vector>
#include <mutex>

#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>

#include "conn.h"

class ConnEthernet : public IConn
{
    Q_OBJECT;

public:
    ConnEthernet();
    virtual ~ConnEthernet();

public:
    const QString       &name() const override;
    int                 port() const;
    int                 timeout() const;
    QTcpSocket          *socket() const;
    bool                isConnected() override;
    ////bool                request(const Adam::Frame &frame) override;

    int                 state() const override;
    int                 error() const override;
    QString             error_string() const override;
    QString             error_reason(int error) const override;

private:
    QString             local_address_connection();

public slots:

private slots:
    void                slot_connect_to(const Params parameters) override;
    void                slot_disconnect_from() override;
    void                slot_cancel_connection() override;
    void                slot_conn_do_request(Adam::Frame request_frame) override;

    void                slot_connected();
    void                slot_disconnected();
    void                slot_connecting_timeout();
    void                slot_stateChanged(QAbstractSocket::SocketState state);
    void                slot_connection_ready_read();

signals:

private:
    QTcpSocket          *m_socket;
    QTimer              *m_connect_timer;
    ////QThread             m_ethThread;
};

#endif // __CONNECTION_ETH_H__
