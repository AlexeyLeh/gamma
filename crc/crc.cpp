/*!
   \file   crc.cpp
   \brief  Расчет контрольной суммы
   \author Aleksey Lekhtin
   \date   created 24.06.2015
 */

#include "crc.h"

//-----------------------------------------------------------
extern "C"
{
    #include "lib_crc/lib_crc.h"
}

//-----------------------------------------------------------
uint16_t calc_crc( const char *data, int data_size )
{
   uint16_t crc_ccitt_0000;
   //char prev_byte = 0;
   // crc_16         = 0;
   // crc_16_modbus  = 0xffff;
   // crc_dnp        = 0;
   // crc_sick       = 0;
   crc_ccitt_0000 = 0;
   // crc_ccitt_ffff = 0xffff;
   // crc_ccitt_1d0f = 0x1d0f;
   // crc_kermit     = 0;
   // crc_32         = 0xffffffffL;
   for( int i = 0; i < data_size; ++i )
   {
      // crc_16         = update_crc_16(     crc_16,         data[i]            );
      // crc_16_modbus  = update_crc_16(     crc_16_modbus,  data[i]            );
      // crc_dnp        = update_crc_dnp(    crc_dnp,        data[i]            );
      // crc_sick       = update_crc_sick(   crc_sick,       data[i], prev_byte );
      crc_ccitt_0000 = update_crc_ccitt( crc_ccitt_0000, data[i] ); //xmodem
      // crc_ccitt_ffff = update_crc_ccitt(  crc_ccitt_ffff, data[i]            );
      // crc_ccitt_1d0f = update_crc_ccitt(  crc_ccitt_1d0f, data[i]            );
      // crc_kermit     = update_crc_kermit( crc_kermit,     data[i]            );
      // crc_32         = update_crc_32(     crc_32,         data[i]            );

      //prev_byte = data[i];
   }
   return crc_ccitt_0000;
}
