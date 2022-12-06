#ifndef __CRC_32_H__
#define __CRC_32_H__

#include <cstdint>

using Crc32Hash = uint32_t;

namespace Crc32
{
   //only for LittleEndian
   Crc32Hash CalcHash(const char* Data, uint32_t DataSz);
}

#endif//!__CRC_32_H__