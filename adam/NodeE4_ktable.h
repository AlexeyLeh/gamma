/*!
  \file   NodeE4_ktable.h
  \author Aleksey Lekhtin
  \brief  Gamma KTable
  \date   10.05.2023
*/

#ifndef __NODE_E4_KTABLE_H__
#define __NODE_E4_KTABLE_H__

#include <vector>

#include "adam.h"

class KTable
{

public:
    KTable();
    ~KTable();

    KTable operator=(const std::vector<byte> &bytes);
    KTable operator=(const byte *data);

    void   clear();
    uint16_t update_crc();

public:
    uint16_t get_checksum() const;
    void   set_checksum(uint16_t value);

    const char *get_partnumber() const;
    int   partnumber_size() const;
    void  set_partnumber(char *value);

    const uint8_t *get_AccSn() const;
    void  set_AccSn(uint8_t *value);

    uint8_t get_Medium_bucket_size() const;
    void set_Medium_bucket_size(uint8_t value);

    uint8_t get_Long_bucket_size() const;
    void set_Long_bucket_size(uint8_t value);

    uint16_t get_Calibration_time() const;
    void set_Calibration_time(uint16_t value);

    uint8_t get_Acquire_frequency() const;
    void set_Acquire_frequency(uint8_t value);

    uint16_t get_Short_bucket_size() const;
    void set_Short_bucket_size(uint16_t value);

public:
    std::vector<byte> serialize() const;
    //void deserialize(std::vector<byte> &bytes);

private:
    char        m_data[128]; // байтовое содержимое таблицы

private:
    uint16_t    m_check_sum;
    char        m_part_number[11];      // def: 77557TD100
    uint8_t     m_Medium_bucket_size;   // def: 4
    uint8_t     m_Long_bucket_size;     // def: 8
    uint16_t    m_Calibration_time;     // def: 30
    uint8_t     m_Acquire_frequency;    // def: 20
    uint16_t    m_Short_bucket_size;    // def: 300
    uint8_t     m_reserved[44];
};

#endif // __NODE_E4_KTABLE_H__
