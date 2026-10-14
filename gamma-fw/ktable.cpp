/*!
  \file   ktable.cpp
  \author Aleksey Lekhtin
  \date   created 17.11.2014
  \brief  KTable of Accelerometer
*/

#include <stdio.h>
#include <string.h>

#include "mainwindow.h"
#include "ktable.h"
#include "utils.h"

//---------------------------------------------------------
// Таблица калибровки акселерометра Gz
ktable_t::ktable_t()
{
    clear();
}

//---------------------------------------------------------
// очистка данных
void ktable_t::clear()
{
    m_check_sum = 0;
    m_Medium_bucket_size = 0;
    m_Long_bucket_size = 0;
    m_Calibration_time = 0;
    m_Acquire_frequency = 0;
    m_Short_bucket_size = 0;
    memset( m_part_number, 0, sizeof(m_part_number) );
    memset( m_reserved, 0, sizeof(m_reserved) );
    memset( m_data, 0, sizeof(m_data) );
}

//---------------------------------------------------------
// зполнение данными
ktable_t ktable_t::operator=( const char *data )
{
    memcpy( m_data, data, sizeof(m_data) );
    m_check_sum  = *data;
    m_check_sum  <<= 8;
    m_check_sum |= (*data & 0x00FF);
    memcpy( m_part_number, data+2, sizeof(m_part_number) );

    m_Medium_bucket_size = *((quint8 *)data + 13);
    m_Long_bucket_size = *((quint8 *)data + 14);
    m_Calibration_time = *((quint16 *)data + 15);
    m_Acquire_frequency = *((quint8 *)data + 17);
    m_Short_bucket_size = *((quint16 *)data + 18);
    memset( m_reserved, 0, sizeof(m_reserved) );
    return *this;
}

//---------------------------------------------------------
// зполнение данными
quint16 ktable_t::update_crc()
{
    char *p = m_data + 2;
    memcpy( p, m_part_number, sizeof(m_part_number) );
    p += sizeof(m_part_number);
    memcpy( p, &m_Medium_bucket_size, 1 );
    p += 4;
    memcpy( p, &m_Long_bucket_size, 1 );
    p += 4;
    memcpy( p, &m_Calibration_time, 2 );
    p += 4;
    memcpy( p, &m_Acquire_frequency, 1 );
    p += 4;
    memcpy( p, &m_Short_bucket_size, 2 );
    p += 4;
    memcpy( p, m_reserved, sizeof(m_reserved) );

    quint16 crc = UTILS::calc_crc_sum( m_data+2, sizeof( m_data )-2 );
    m_check_sum = crc;
    memcpy(  m_data, &m_check_sum, 2 );
    return crc;
}


