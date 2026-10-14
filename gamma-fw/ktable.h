/*!
  \file   ktable.h
  \author Aleksey Lekhtin
  \date   created 07.05.2025
  \brief  KTable
*/

#ifndef _KTABLE_H_
#define _KTABLE_H_

#include <QtGlobal>

class ktable_t
{
public:
    explicit    ktable_t();

    void        clear();
    ktable_t    operator=( const char *data );
    quint16     update_crc();

    quint16     m_check_sum;
    char        m_part_number[11];      // def: 77557TD100
    quint8      m_Medium_bucket_size;   // def: 4
    quint8      m_Long_bucket_size;     // def: 8
    quint16     m_Calibration_time;     // def: 30
    quint8      m_Acquire_frequency;    // def: 20
    quint16     m_Short_bucket_size;    // def: 300
    char        m_reserved[44];

    char        m_data[64];

    // typedef union ktable_t
    // {
    //     struct
    //     {
    //         uint16_t Check_sum;
    //         char     Part_number[11];
    //         uint8_t  Medium_bucket_size;
    //         uint8_t  Long_bucket_size;
    //         uint16_t Calibration_time;
    //         uint8_t  Acquire_frequency;
    //         uint16_t Short_bucket_size;
    //     } attribute ((packed,aligned(1)));
    //     uint8_t data[40];
    // } attribute ((packed,aligned(1))) ktable_t;
};

#endif // _KTABLE_H_

