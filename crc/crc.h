/*!
   \file   crc.h
   \brief  Расчет контрольной суммы CRC16 CCITT
   \author Aleksey Lekhtin
   \date   created 06.03.2015
 */

#ifndef __CRC_XMODEM__H_
#define __CRC_XMODEM__H_


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// #ifdef WIN32
// #define snprintf _snprintf
// #endif

#include <stdint.h>

uint16_t calc_crc( const char *data, int data_size );


#endif // __CRC_XMODEM__H_
