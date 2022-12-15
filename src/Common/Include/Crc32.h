#ifndef __CRC_32_H__
#define __CRC_32_H__

#include <cstdint>

using Crc32Hash = uint32_t;

/*! Namespace for crc32 hash funtions*/
namespace Crc32
{
   /**
    * @brief Function to calculate crc32 hash. LittleEndian only.
    * 
    * @param Data Data to calc hash
    * @param DataSz Data size
    * @return Crc32Hash 
    */
   Crc32Hash
   CalcHash(
      const char* Data,
      uint32_t DataSz);
}

#endif//!__CRC_32_H__