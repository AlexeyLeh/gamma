/*!
  \file   NodeE4_ctable.h
  \author Aleksey Lekhtin
  \brief  Gamma CTable
  \date   10.05.2023
*/

#ifndef __NODE_E4_CTABLE_H__
#define __NODE_E4_CTABLE_H__

#include <vector>

#include "adam.h"

class CTable
{

public:
    CTable();
    ~CTable();

    CTable &operator=(const std::vector<byte> &bytes);
    CTable &operator=(const byte *data);

    void   clear();
    ushort update_crc();

public:
    ushort get_checksum() const;
    void   set_checksum(ushort value);

    const char *get_partnumber() const;
    void  set_partnumber(char *value);

    const char *get_AccSn() const;
    void  set_AccSn(char *value);

    const char *get_date() const;
    void  set_date(char *value);

    const char *get_time() const;
    void  set_time(char *value);

    float get_Ksfz0() const;
    void  set_Ksfz0(float value);

    float get_Ksfz1() const;
    void  set_Ksfz1(float value);

    float get_Ksfz2() const;
    void  set_Ksfz2(float value);

    float get_Ksfz3() const;
    void  set_Ksfz3(float value);

    float get_Kbz0() const;
    void  set_Kbz0(float value);

    float get_Kbz1() const;
    void  set_Kbz1(float value);

    float get_Kbz2() const;
    void  set_Kbz2(float value);

    float get_Kbz3() const;
    void  set_Kbz3(float value);

public:
    std::vector<byte> serialize() const;
    void deserialize(std::vector<byte> &bytes);

private:
    char   m_data[80]; // байтовое содержимое таблицы

public:
    ushort m_check_sum;
    char   m_part_number[11];
    float  m_scale_Ksfz0;
    float  m_scale_Ksfz1;
    float  m_scale_Ksfz2;
    float  m_scale_Ksfz3;
    float  m_bias_Kbz0;
    float  m_bias_Kbz1;
    float  m_bias_Kbz2;
    float  m_bias_Kbz3;
    char   m_accel_SN[7];
    char   m_date[8];
    char   m_time[5];
    char   m_reserved[15];
};

#endif // __NODE_E4_CTABLE_H__
