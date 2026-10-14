/*!
  \file   NodeE4_ctable.cpp
  \author Aleksey Lekhtin
  \brief  Gamma CTable
  \date   10.05.2023
*/

#include <cstring>
#include <vector>

#include <QDebug>

#include "dbg.h"
#include "NodeE4_ctable.h"
#include "unmarshal_float.h"

//---------------------------------------------------------
CTable::CTable() :
    m_data(),
    m_check_sum(0),
    m_part_number(),
    m_scale_Ksfz0(0),
    m_scale_Ksfz1(0),
    m_scale_Ksfz2(0),
    m_scale_Ksfz3(0),
    m_bias_Kbz0(0),
    m_bias_Kbz1(0),
    m_bias_Kbz2(0),
    m_bias_Kbz3(0),
    m_accel_SN(),
    m_date(),
    m_time(),
    m_reserved()
{
}

//---------------------------------------------------------
CTable::~CTable()
{
    DBG(4);
}

//---------------------------------------------------------
void CTable::clear()
{
    m_check_sum   = 0;
    m_scale_Ksfz0 = 0.0;
    m_scale_Ksfz1 = 0.0;
    m_scale_Ksfz2 = 0.0;
    m_scale_Ksfz3 = 0.0;
    m_bias_Kbz0   = 0.0;
    m_bias_Kbz1   = 0.0;
    m_bias_Kbz2   = 0.0;
    m_bias_Kbz3   = 0.0;
    memset(m_part_number, 0, sizeof(m_part_number));
    memset(m_accel_SN,    0, sizeof(m_accel_SN));
    memset(m_date,        0, sizeof(m_date));
    memset(m_time,        0, sizeof(m_time));
    memset(m_reserved,    0, sizeof(m_reserved));
    memset(m_data,        0, sizeof(m_data));
}

//---------------------------------------------------------
CTable &CTable::operator=(const std::vector<byte> &bytes)
{
    if (bytes.size() < 80) {
         DBG(0, "Error: Input bytes container is too small for CTable");
         clear();
         return *this;
     }
     return *this = bytes.data();
}

//---------------------------------------------------------
CTable &CTable::operator=(const byte *data)
{
    if (!data) return *this;
    memcpy(m_data, data, sizeof(m_data));
    m_check_sum  = (data[0] << 8) | data[1];
    memcpy(m_part_number, data+2, sizeof(m_part_number));

    m_scale_Ksfz0 = Adam::decode_ieee754(data+13, 4);
    m_scale_Ksfz1 = Adam::decode_ieee754(data+17, 4);
    m_scale_Ksfz2 = Adam::decode_ieee754(data+21, 4);
    m_scale_Ksfz3 = Adam::decode_ieee754(data+25, 4);

    m_bias_Kbz0   = Adam::decode_ieee754(data+29, 4);
    m_bias_Kbz1   = Adam::decode_ieee754(data+33, 4);
    m_bias_Kbz2   = Adam::decode_ieee754(data+37, 4);
    m_bias_Kbz3   = Adam::decode_ieee754(data+41, 4);

    memcpy(m_accel_SN, data+45, sizeof(m_accel_SN));
    memcpy(m_date,     data+52, sizeof(m_date));
    memcpy(m_time,     data+60, sizeof(m_time));
    memset(m_reserved, 0, sizeof(m_reserved));
    return *this;
}

//---------------------------------------------------------
//! зполнение данными
ushort CTable::update_crc()
{
    char *p = m_data + 2;
    memcpy(p, m_part_number, sizeof(m_part_number));
    p += sizeof(m_part_number);
    memcpy(p, Adam::encode_ieee754(m_scale_Ksfz0).data(), 4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_scale_Ksfz1).data(), 4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_scale_Ksfz2).data(), 4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_scale_Ksfz3).data(), 4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_bias_Kbz0).data(),   4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_bias_Kbz1).data(),   4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_bias_Kbz2).data(),   4);
    p += 4;
    memcpy(p, Adam::encode_ieee754(m_bias_Kbz3).data(),   4);
    p += 4;
    memcpy(p, m_accel_SN, sizeof(m_accel_SN));
    p += sizeof(m_accel_SN);
    memcpy(p, m_date, sizeof(m_date));
    p += sizeof(m_date);
    memcpy(p, m_time, sizeof(m_time));
    p += sizeof(m_time);
    memcpy(p, m_reserved, sizeof(m_reserved));
    p += sizeof(m_reserved);

    if (p - m_data <= 78) {
        m_check_sum = Adam::calc_checksum(std::vector<unsigned char>(m_data + 2, m_data + 80 - 2));
        std::memcpy(m_data, &m_check_sum, sizeof(m_check_sum));
        // Контрольная сумма пишется в начало m_data[0]/m_data[1]
    }
    return m_check_sum;
}

std::vector<byte> CTable::serialize() const
{
    std::vector<byte> bytes;
    bytes.reserve(80);

    bytes.insert(bytes.end(), (char*)&m_check_sum, (char*)&m_check_sum + sizeof(m_check_sum));
    bytes.insert(bytes.end(), m_part_number, m_part_number + sizeof(m_part_number));

    std::vector<byte> v = Adam::encode_ieee754(m_scale_Ksfz0);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_scale_Ksfz1);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_scale_Ksfz2);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_scale_Ksfz3);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_bias_Kbz0);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_bias_Kbz1);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_bias_Kbz2);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    v = Adam::encode_ieee754(m_bias_Kbz3);
    bytes.insert(bytes.end(), v.data(), v.data() + sizeof(float));

    bytes.insert(bytes.end(), m_accel_SN, m_accel_SN + sizeof(m_accel_SN));
    bytes.insert(bytes.end(), m_date, m_date + sizeof(m_date));
    bytes.insert(bytes.end(), m_time, m_time + sizeof(m_time));
    bytes.insert(bytes.end(), m_reserved, m_reserved + sizeof(m_reserved));
    return bytes;
}

void CTable::deserialize(std::vector<byte> &bytes)
{
    //// NodeE4::Ctable ctable;
    //// ::deserialize(ctable, serialized.begin(), serialized.end());
    //// return ctable;

    const byte *p = bytes.data();
    std::memcpy(&m_check_sum, p, sizeof(m_check_sum));
    //// m_check_sum = *(ushort*)p;
    p += sizeof(ushort);

    memcpy(m_part_number, p, sizeof(m_part_number));
    p += sizeof(m_part_number);

    memcpy((char*)&m_scale_Ksfz0, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_scale_Ksfz1, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_scale_Ksfz2, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_scale_Ksfz3, p, sizeof(float));
    p += sizeof(float);

    memcpy((char*)&m_bias_Kbz0, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_bias_Kbz1, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_bias_Kbz2, p, sizeof(float));
    p += sizeof(float);
    memcpy((char*)&m_bias_Kbz3, p, sizeof(float));
    p += sizeof(float);

    memcpy(m_accel_SN, p, sizeof(m_accel_SN));
    p += sizeof(m_accel_SN);
    memcpy(m_date, p, sizeof(m_date));
    p += sizeof(m_date);
    memcpy(m_time, p, sizeof(m_time));
    p += sizeof(m_time);
    memcpy(m_reserved, p, sizeof(m_reserved));
}

ushort CTable::get_checksum() const
{
    return m_check_sum;
}

void CTable::set_checksum(ushort value)
{
    m_check_sum = value;
}

const char *CTable::get_partnumber() const
{
    return m_part_number;
}

void CTable::set_partnumber(char *value)
{
    if (!value) return;
    memcpy(m_part_number, value, sizeof(m_part_number));
}

const char *CTable::get_AccSn() const
{
    return m_accel_SN;
}

void CTable::set_AccSn(char *value)
{
    if (!value) return;
    memcpy(m_accel_SN, value, sizeof(m_accel_SN));
}

const char *CTable::get_date() const
{
    return m_date;
}

void CTable::set_date(char *value)
{
    if (!value) return;
    memcpy(m_date, value, sizeof(m_date));
}

const char *CTable::get_time() const
{
    return m_time;
}

void CTable::set_time(char *value)
{
    if (!value) return;
    memcpy(m_time, value, sizeof(m_time));
}

float CTable::get_Ksfz0() const
{
    return m_scale_Ksfz0;
}

void CTable::set_Ksfz0(float value)
{
    m_scale_Ksfz0 = value;
}

float CTable::get_Ksfz1() const
{
    return m_scale_Ksfz1;
}

void CTable::set_Ksfz1(float value)
{
    m_scale_Ksfz1 = value;
}

float CTable::get_Ksfz2() const
{
    return m_scale_Ksfz2;
}

void CTable::set_Ksfz2(float value)
{
    m_scale_Ksfz2 = value;
}

float CTable::get_Ksfz3() const
{
    return m_scale_Ksfz3;
}

void CTable::set_Ksfz3(float value)
{
    m_scale_Ksfz3 = value;
}

float CTable::get_Kbz0() const
{
    return m_bias_Kbz0;
}

void CTable::set_Kbz0(float value)
{
    m_bias_Kbz0 = value;
}

float CTable::get_Kbz1() const
{
    return m_bias_Kbz1;
}

void CTable::set_Kbz1(float value)
{
    m_bias_Kbz1 = value;
}

float CTable::get_Kbz2() const
{
    return m_bias_Kbz2;
}

void CTable::set_Kbz2(float value)
{
    m_bias_Kbz2 = value;
}

float CTable::get_Kbz3() const
{
    return m_bias_Kbz3;
}

void CTable::set_Kbz3(float value)
{
    m_bias_Kbz3 = value;
}
