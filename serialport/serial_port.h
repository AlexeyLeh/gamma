/*!
  \file   serial_port.h
  \brief  Работа с последовательным портом
  \author Aleksey Lekhtin
  \date   17.10.2016
*/

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#include <QDateTime>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QSettings>

#include "serial_conf.h"

//-----------------------------------------------------------
// Работа с последовательным портом
//
class SERIALPORT : public QSerialPort
{
    Q_OBJECT;

public:
    explicit SERIALPORT( QWidget *parent = nullptr );
    ~SERIALPORT();

    bool serialport_connect();
    void serialport_disconnect();
    bool serialport_write_to( const QByteArray & data );
    SerialPortError sp_error() { return error(); }
    QString sp_errorString();

    void read_settings(QSettings *st);
    void save_settings(QSettings *st);
    const port_t & port_settings() const;
    void set_port_settings( const port_t & port );

    static void delay( int ms );

signals:
    void signal__serialport_connected( bool status );
    void signal__serialport_data_got( QByteArray data, QDateTime dt );
    void signal__serialport_error( QString msg );

private slots:
    void slot_serial_error( QSerialPort::SerialPortError error );
    void slot_read_from_serial();

private:
    port_t         m_port_settings;// параметры порта
    QByteArray     m_serial_buf;   // буфер приема данных с порта

};


#endif // _SERIALPORT_H_
