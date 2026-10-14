/*!
  \file   float.cpp
  \author Aleksey Lekhtin
  \brief  Конвертирование float из big endian в little endian
  \date   10.05.2023
*/

#include <cstdint>
#include <string>
#include <vector>

using byte = uint8_t;
using ushort = unsigned short;

namespace Adam
{
    float decode_ieee754(const std::vector<byte> bytes);
    float decode_ieee754(const byte *bytes, size_t size);
    float decode_ieee754(float value);

    std::vector<byte> encode_ieee754(float value);
    std::vector<byte> encode_to_big_endian(ushort value);

    inline ushort calc_checksum(const std::vector<byte>& bytes)
    {
        ushort csum = 0;
        for (const byte b : bytes) {
            csum += ushort(b);
        }
        return csum;
    }


} // namespace Adam

