#ifndef __BYTE_HANDLER_H__
#define __BYTE_HANDLER_H__
#include <stdint.h>
#include <cstddef>

#define BIT(n) (1U << (n))
#define BITS_IN_BYTE (8U)

namespace ByteHandler
{

   void SetBit(uint8_t *Arr, size_t ArrSz, size_t Pos, bool Value);
   bool GetBit(uint8_t *Arr, size_t ArrSz, size_t Pos);
   void SetBit(uint8_t &Byte, uint8_t Pos, bool Value);
   bool GetBit(uint8_t Byte, uint8_t Pos);
   uint8_t GetHigherBitPos(uint8_t Byte);
   uint8_t GetLowerBitPos( uint8_t Byte);
}

#endif //!__BYTE_HANDLER_H__