/*!
  \file   float.cpp
  \author Aleksey Lekhtin
  \brief  Конвертирование float из big endian в little endian
  \date   10.05.2023
*/

#include "unmarshal_float.h"

#include "dbg.h"

namespace Adam
{
inline bool is_little_endian()
{
    static const int n = 1;
    return (*(char*)&n == 1); // little endian if true
}

static const bool little_endian = is_little_endian();

//-----------------------------------------------------
float decode_ieee754(const byte *bytes, size_t size)
{
    if (size != 4) return 0.0;
    float value;
    byte *dst = reinterpret_cast<byte*>(&value);
    for(size_t i = 0; i < size; ++i) {
        dst[size-1-i] = bytes[i];
    }
    return value;
}

//-----------------------------------------------------
float decode_ieee754(const std::vector<byte> bytes)
{
    return decode_ieee754(bytes.data(), bytes.size());
}

//-----------------------------------------------------
float decode_ieee754(float value)
{
    const byte *ptr = reinterpret_cast<byte*>(&value);
    return decode_ieee754(ptr, sizeof(float));
}

//-----------------------------------------------------
std::vector<byte> encode_ieee754(float value)
{
    //// if (little_endian)
    //// {
        // convert to big endian
        std::vector<byte> bytes{0,0,0,0};
        byte *src = reinterpret_cast<byte*>(&value);
        for(size_t i = 0; i < 4; ++i)
        {
            bytes[i] = src[4-1-i];
        }
        return bytes;
    //// }
    ////
    //// // stay as is - little endian
    //// byte *src = reinterpret_cast<byte*>(&value);
    //// std::vector<byte> bytes(src, src+4);
    //// return bytes;
}

//-----------------------------------------------------
std::vector<byte> encode_to_big_endian(ushort value)
{
    std::vector<byte> bytes{0,0};
    //// if (little_endian)
    //// {
        // convert to big endian
        bytes[0] = (value >> 8) & 0x00FF;
        bytes[1] = (value & 0x00FF);
    //// }
    //// else
    //// {
    ////     // stay as is - little endian
    ////     bytes[1] = (value >> 8) & 0x00FF;
    ////     bytes[0] = (value & 0x00FF);
    //// }
    return bytes;
}


} // namespace Adam

