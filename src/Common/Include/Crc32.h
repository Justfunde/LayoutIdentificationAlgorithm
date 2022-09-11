#ifndef __CRC_32_H__
#define __CRC_32_H__

#include <cstdint>

using CRC32_HASH = uint32_t;

//only for LittleEndian
CRC32_HASH Crc32HashCalc(const char* Data, uint32_t DataSz);

#endif//!__CRC_32_H__