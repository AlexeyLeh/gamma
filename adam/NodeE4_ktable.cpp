/*!
  \file   NodeE4_ktable.cpp
  \author Aleksey Lekhtin
  \brief  Gamma tool KTable
  \date   10.05.2023
*/

#include <cstring>
#include <vector>
#include <cstdlib>

#include <QDebug>

#include "dbg.h"
#include "NodeE4_ktable.h"
#include "utils.h"

//---------------------------------------------------------
KTable::KTable() :
    m_data(),
    m_check_sum(0),
    m_part_number(),
    m_Medium_bucket_size(0),
    m_Long_bucket_size(0),
    m_Calibration_time(0),
    m_Acquire_frequency(0),
    m_Short_bucket_size(0),
    m_reserved()
{
}

//---------------------------------------------------------
KTable::~KTable()
{
    DBG(4);
}

//---------------------------------------------------------
void KTable::clear()
{
    m_check_sum = 0;
    m_Medium_bucket_size = 0;
    m_Long_bucket_size = 0;
    m_Calibration_time = 0;
    m_Acquire_frequency = 0;
    m_Short_bucket_size = 0;
    memset(m_part_number, 0, sizeof(m_part_number));
    memset(m_reserved, 0, sizeof(m_reserved));
    memset(m_data, 0, sizeof(m_data));
}

//---------------------------------------------------------
KTable KTable::operator=(const std::vector<byte> &bytes)
{
    *this = bytes.data();
    return *this;
}

//---------------------------------------------------------
KTable KTable::operator=(const byte *data)
{
    memcpy(m_data, data, sizeof(m_data));
    m_check_sum  = (data[0] << 8) | data[1];
    memcpy(m_part_number, data+2, sizeof(m_part_number));

    uint8_t val8;
    uint16_t val16;
    val8 = *((uint8_t*)data + 13);
    m_Medium_bucket_size = val8;

    val8 = *((uint8_t*)data + 14);
    m_Long_bucket_size = val8;

    val16 = *((uint16_t*)(data + 15));
    val16 = __builtin_bswap16(val16);
    m_Calibration_time = val16;

    val8 = *((uint8_t*)data + 17);
    m_Acquire_frequency = val8;

    val16 = *((uint16_t*)(data + 18));
    val16 = __builtin_bswap16(val16);
    m_Short_bucket_size = val16;

    memset(m_reserved, 0, sizeof(m_reserved));
    return *this;
}

//---------------------------------------------------------
//! заполнение данными
uint16_t KTable::update_crc()
{
    char *p = m_data + 2;
    memcpy( p, m_part_number, sizeof(m_part_number) );
    p += sizeof(m_part_number);
    memcpy( p, &m_Medium_bucket_size, sizeof(m_Medium_bucket_size) );
    p += sizeof(m_Medium_bucket_size);
    memcpy( p, &m_Long_bucket_size, sizeof(m_Long_bucket_size) );
    p += sizeof(m_Long_bucket_size);
    memcpy( p, &m_Calibration_time, sizeof(m_Calibration_time) );
    p += sizeof(m_Calibration_time);
    memcpy( p, &m_Acquire_frequency, sizeof(m_Acquire_frequency) );
    p += sizeof(m_Acquire_frequency);
    memcpy( p, &m_Short_bucket_size, sizeof(m_Short_bucket_size) );
    p += sizeof(m_Short_bucket_size);
    memcpy( p, m_reserved, sizeof(m_reserved) );
    p += sizeof(m_reserved);
    size_t sz = p - m_data;

    quint16 crc = UTILS::calc_crc_sum( m_data+2, sz-2 );
    m_check_sum = crc;
    memcpy( m_data, &m_check_sum, sizeof(m_check_sum) );
    return crc;
}

std::vector<byte> KTable::serialize() const
{
    std::vector<byte> bytes;
    uint16_t val16;
    val16 = __builtin_bswap16(m_check_sum);
    bytes.insert(bytes.end(), (char*)&val16, (char*)&val16 + sizeof(val16));

    bytes.insert(bytes.end(), m_part_number, m_part_number + sizeof(m_part_number));

    bytes.insert(bytes.end(), (char*)&m_Medium_bucket_size, (char*)&m_Medium_bucket_size + sizeof(m_Medium_bucket_size));
    bytes.insert(bytes.end(), (char*)&m_Long_bucket_size, (char*)&m_Long_bucket_size + sizeof(m_Long_bucket_size));

    val16 = __builtin_bswap16(m_Calibration_time);
    bytes.insert(bytes.end(), (char*)&val16, (char*)&val16 + sizeof(val16));

    bytes.insert(bytes.end(), (char*)&m_Acquire_frequency, (char*)&m_Acquire_frequency + sizeof(m_Acquire_frequency));

    val16 = __builtin_bswap16(m_Short_bucket_size);
    bytes.insert(bytes.end(), (char*)&val16, (char*)&val16 + sizeof(val16));

    bytes.insert(bytes.end(), m_reserved, m_reserved + sizeof(m_reserved));
    return bytes;
}

//void KTable::deserialize(std::vector<byte>& bytes)
//{
//    //// NodeE4::Ktable ktable;
//    //// ::deserialize(ktable, serialized.begin(), serialized.end());
//    //// return ktable;
//
//    const byte *p = bytes.data();
//    m_check_sum = *(uint16_t*)p;
//    p += sizeof(uint16_t);
//
//    memcpy(m_part_number, p, sizeof(m_part_number));
//    p += sizeof(m_part_number);
//
//    memcpy((char*)&m_Medium_bucket_size, p, sizeof(m_Medium_bucket_size));
//    p += sizeof(m_Medium_bucket_size);
//    memcpy((char*)&m_Long_bucket_size, p, sizeof(m_Long_bucket_size));
//    p += sizeof(m_Long_bucket_size);
//    memcpy((char*)&m_Calibration_time, p, sizeof(m_Calibration_time));
//    p += sizeof(m_Calibration_time);
//    memcpy((char*)&m_Acquire_frequency, p, sizeof(m_Acquire_frequency));
//    p += sizeof(m_Acquire_frequency);
//    memcpy((char*)&m_Short_bucket_size, p, sizeof(m_Short_bucket_size));
//    p += sizeof(m_Short_bucket_size);
//    memcpy(m_reserved, p, sizeof(m_reserved));
//}

uint16_t KTable::get_checksum() const
{
    return m_check_sum;
}

void KTable::set_checksum(uint16_t value)
{
    m_check_sum = value;
}

const char *KTable::get_partnumber() const
{
    return m_part_number;
}

int KTable::partnumber_size() const
{
    return (int)sizeof(m_part_number);
}

void KTable::set_partnumber(char *value)
{
    memcpy(m_part_number, value, sizeof(m_part_number));
}

uint8_t KTable::get_Medium_bucket_size() const
{
    return m_Medium_bucket_size;
}

void KTable::set_Medium_bucket_size(uint8_t value)
{
     m_Medium_bucket_size = value;
}

uint8_t KTable::get_Long_bucket_size() const
{
   return m_Long_bucket_size;
}

void KTable::set_Long_bucket_size(uint8_t value)
{
    m_Long_bucket_size = value;
}

uint16_t KTable::get_Calibration_time() const
{
   return m_Calibration_time;
}

void KTable::set_Calibration_time(uint16_t value)
{
    m_Calibration_time = value;
}

uint8_t KTable::get_Acquire_frequency() const
{
   return m_Acquire_frequency;
}

void KTable::set_Acquire_frequency(uint8_t value)
{
   m_Acquire_frequency  = value;
}

uint16_t KTable::get_Short_bucket_size() const
{
   return m_Short_bucket_size;
}

void KTable::set_Short_bucket_size(uint16_t value)
{
   m_Short_bucket_size = value;
}
