/*!
  \file   serial_port.cpp
  \brief  Работа с последовательным портом
  \author Aleksey Lekhtin
  \date   17.10.2016
*/

#include <QApplication>
#include <QMessageBox>
#include <QStatusBar>
#include <QDateTime>

#include "dbg.h"
#include "serial_port.h"

//-----------------------------------------------------------
SERIALPORT::SERIALPORT( QWidget *parent ) :
    QSerialPort( parent ),
    m_port_settings()
{
    connect( this, SIGNAL( readyRead() ),
             this, SLOT( slot_read_from_serial() ) );

    connect( this, SIGNAL( error(QSerialPort::SerialPortError) ),
             this, SLOT( slot_serial_error(QSerialPort::SerialPortError) ) );

    // connect( this, SIGNAL( requestToSendChanged(bool) ),
    //         this, SLOT( slot_serial_RTS_changed(bool) ) );

    // connect( this, SIGNAL( parityChanged(QSerialPort::Parity) ),
    //         this, SLOT( slot_serial_parity_changed(QSerialPort::Parity) ) );
}

//-----------------------------------------------------------
SERIALPORT::~SERIALPORT()
{
    DBG(4);
}

//-----------------------------------------------------------
QString SERIALPORT::sp_errorString()
{
    return errorString();
}

//-----------------------------------------------------------
void SERIALPORT::read_settings(QSettings *st)
{
    DBG(4);
    port_t port = port_settings();
    st->beginGroup("Comport");
    port.sName = st->value( "Port", port_t::def_name() ).toString();
    port.baudRate = (QSerialPort::BaudRate)st->value( QStringLiteral("Speed"), 9600 ).toInt();
    port.dataBits = (QSerialPort::DataBits)st->value( QStringLiteral("Bits"),     8 ).toInt();
    port.stopBits = (QSerialPort::StopBits)st->value( QStringLiteral("StopBits"), 1 ).toInt();
    int num = 0;
    QString str = st->value( "Parity", "Auto" ).toString();
    if( str.toLower() == QStringLiteral("even" ) ) num = 2;
    else if( str.toLower() == QStringLiteral("odd" ) ) num = 3;
    else if( str.toLower() == QStringLiteral("space") ) num = 4;
    else if( str.toLower() == QStringLiteral("mark" ) ) num = 5;
    port.parity = static_cast<QSerialPort::Parity>(-1);
    num = 0;
    str = st->value( "FlowControl", "None" ).toString();
    if(str.toUpper() == QStringLiteral("RTS/CTS" ) ) num = 1;
    else if( str.toUpper() == QStringLiteral("XON/XOFF") ) num = 2;
    port.flowCtrl  = static_cast<QSerialPort::FlowControl>( num );
    st->endGroup();
    set_port_settings( port );
}

//-----------------------------------------------------------
void SERIALPORT::save_settings(QSettings *st)
{
    DBG(4);
    const port_t &port = port_settings();
    st->beginGroup("Comport");
    st->setValue( "Port",        port.sName      );
    st->setValue( "Speed",       port.sBaudRate  );
    st->setValue( "Bits",        port.sDataBits  );
    st->setValue( "Parity",      port.sParity    );
    st->setValue( "StopBits",    port.sStopBits  );
    st->setValue( "FlowControl", port.sFlowCtrl  );
    st->endGroup();
}

//-----------------------------------------------------------
const port_t & SERIALPORT::port_settings() const
{
    return m_port_settings;
}

//-----------------------------------------------------------
void SERIALPORT::set_port_settings( const port_t & port )
{
    m_port_settings = port;
}

//-----------------------------------------------------------
// Подключение к порту
//
bool SERIALPORT::serialport_connect()
{
    if( isOpen() )
    {
        emit signal__serialport_connected( isOpen() );
        return true;
    }
    m_serial_buf.clear();

    port_t port = port_settings();
    setPortName(port.sName);
    setBaudRate(port.baudRate);
    setDataBits(port.dataBits);
    setStopBits(port.stopBits);
    setParity(port.parity);
    setFlowControl( port.flowCtrl );
    DBG( 1, QString("%1 %2 %3").arg( portName() ).arg( baudRate() ).arg( parity() ) );

    SERIALPORT::open( QIODevice::ReadWrite );
    emit signal__serialport_connected( isOpen() );
    return isOpen();
}

//-----------------------------------------------------------
// Отключение от порта
//
void SERIALPORT::serialport_disconnect()
{
    m_serial_buf.clear();
    if( !isOpen() ) return;
    DBG( 1, portName() );
    setRequestToSend( false );
    delay( 10 );
    if( isOpen() ) SERIALPORT::close();
    emit signal__serialport_connected( isOpen() );
}

//-----------------------------------------------------------
// Обработка ошибки порта
//
void SERIALPORT::slot_serial_error( QSerialPort::SerialPortError err )
{
    if( err == 0 ) {
        return;
    }
    DBG( 1, QString("%1 %2 %3").arg( portName() ).arg( int(err) ).arg( errorString() ) );
    if( err == QSerialPort::ResourceError )
    {
        emit signal__serialport_connected( isOpen() );
        serialport_disconnect();
    }
}

//-----------------------------------------------------------
// Запись данных в последовательный порт
//
bool SERIALPORT::serialport_write_to( const QByteArray & data )
{
    if( !isOpen() )
    {
        DBG( 0, "Serial port doesn't open" );
        emit signal__serialport_connected( false );
        return false;
    }

    DBG( 1, QString("send: %1 len: %2 %3")
         .arg( portName() )
         .arg( data.toHex().constData() )
         .arg( data.size() ) );

    DBG( 2, "set RTS" );
    setRequestToSend( true );

    qint64 sz;
    bool ok = setParity( QSerialPort::MarkParity );
    const char *p = data.constData();
    int cnt = data.size();
    for( int i = 0; i < cnt; ++i )
    {
        if( i == 1 )
        {
            ok = setParity( QSerialPort::SpaceParity );
        }
        if ( ok )
        {
            if( !isOpen() ) break;
            sz = write( p, 1 );
        }
        if( !ok || sz == -1 )
        {
            QString msg = QString( "%1\n%2" )
                    .arg( SERIALPORT::tr("Serial port error") )
                    .arg( errorString() );
            DBG( 1, msg );
            emit signal__serialport_connected( isOpen() );
            emit signal__serialport_error(msg);
            DBG( 1, "done (fail)" );
            return false;
        }

        QCoreApplication::sendPostedEvents();
        delay( 3 );
        ++p;
    }
    DBG( 1, "sent" );
    delay( 6 );

    setRequestToSend( false );
    DBG( 2, "clear RTS" );

    setParity( QSerialPort::MarkParity );
    DBG( 1, "done (ok)" );
    return true;
}

//-----------------------------------------------------------
// Чтение данных из последовательного порта
//
void SERIALPORT::slot_read_from_serial()
{
    // формат ожидаемого пакета данных:
    // [0]to [1]from [2-3]len [4]code {[5]cmd ack [6]data ...} [len-2]crc
    QDateTime dt = QDateTime::currentDateTime();
    QByteArray data = SERIALPORT::readAll();
    m_serial_buf += data;

    DBG( 3, QString("got data: len: %1, %2").arg(data.size()).arg(data.toHex().constData()) );

    while( m_serial_buf.size() >= 7 ) // минимальная длина 'целого' пакета
    {
        quint16 len = (m_serial_buf.at(2) << 8) & 0xFF00;
        len |= (m_serial_buf.at(3) & 0x00FF);

        if( len == 0 ) // неверные данные
        {
            DBG( 3, QString("len:%1").arg(len) );
            emit signal__serialport_data_got( m_serial_buf, dt );
            m_serial_buf.clear();
            return;
        }

        if( m_serial_buf.size() == len ) // принят один полный пакет данных
        {
            DBG( 3, QString("got whole packet: len: %1, %2").arg(len).arg(m_serial_buf.toHex().constData()) );
            emit signal__serialport_data_got( m_serial_buf, dt );
            m_serial_buf.clear();
            return;
        }

        if( m_serial_buf.size() < len )
        {
            DBG( 3, QString("not enough data - total in buf: len: %1, %2")
                 .arg(m_serial_buf.size()).arg(m_serial_buf.toHex().constData()) );
            return; // принят не полный пакет данных
        }

        // в данных может быть несколько пакетов, как целиком, так и "без хвостов"
        QByteArray buf = m_serial_buf.left(len);
        DBG( 3, QString("requested len: %1 - accepted packet: %2").arg(len).arg(buf.toHex().constData()));
        emit signal__serialport_data_got( buf, dt );

        m_serial_buf = m_serial_buf.mid(len);
        DBG( 3, QString("next data for process: len: %1, %2")
             .arg(m_serial_buf.size()).arg(m_serial_buf.toHex().constData()) );
    }

    DBG( 3, QString("too little data - total in buf: len: %1, %2")
         .arg(m_serial_buf.size()).arg(m_serial_buf.toHex().constData()) );
    return;
}

//-----------------------------------------------------------
// Пауза (без "замораживания" GUI)
//
void SERIALPORT::delay( int ms )
{
    QDateTime tim_cur = QDateTime::currentDateTime();
    QDateTime tim_end = tim_cur.addMSecs( ms );
    do
    {
        tim_cur = QDateTime::currentDateTime();
        QCoreApplication::sendPostedEvents();
        //QCoreApplication::processEvents();
    } while( tim_cur < tim_end );
}
