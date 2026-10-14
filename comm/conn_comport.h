/*!
  \file   conn_com.h
  \author Алексей Лехтин
  \brief  Adam, com-порт реализация
  \date   10.05.2023
*/

#ifndef __CONNECTION_COM_H__
#define __CONNECTION_COM_H__

#include <string>

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "serial_port.h"
#include "conn.h"


class ConnComport : public IConn
{
    Q_OBJECT;

public:
    ConnComport();
    virtual ~ConnComport();

public:
    const QString   &name() const override;
    bool            isConnected() override;
    int             state() const override;
    int             error() const override;
    QString         error_string() const override;
    QString         error_reason(int error) const override;

public:
    QSerialPort     *qt_serialport() const;
    const port_t    &port_settings() const;
    static QString  port_info(const port_t &port);
    static QString  port_info_short(const port_t &port);

private slots:
    void slot_connect_to(Params params) override;
    void slot_disconnect_from() override;
    void slot_conn_do_request(Adam::Frame request_frame) override;
    void slot_cancel_connection() override;

    void slot_serialport_error(QSerialPort::SerialPortError err);
    void slot_serialport_ready_read();

signals:

private:
    QSerialPort *m_qt_serial_port;  // последовательный порт Qt
    QString     m_portName;
};

#endif // __CONNECTION_COM_H__
