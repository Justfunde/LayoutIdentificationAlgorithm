#include <stdexcept>

#include "Include/ByteHandler.h"

#define MAX_BIT_POS(ArrSz) (BITS_IN_BYTE * (ArrSz) - 1)

constexpr size_t g_ptrSz = sizeof(void*); 

void
ByteHandler::SetBit(
   uint8_t *Arr,
   size_t ArrSz,
   size_t Pos,
   bool Value)
{
   if(nullptr == Arr || g_ptrSz > ArrSz || MAX_BIT_POS(ArrSz) < Pos ) { throw std::invalid_argument("Invalid bit position");}
   const size_t byteIndex = ArrSz * BITS_IN_BYTE / Pos;
   const uint8_t bitIndex = BITS_IN_BYTE % Pos;
   switch (Value)
   {
      case true: Arr[byteIndex] |= BIT(bitIndex);
      case false: Arr[byteIndex] &= (~BIT(bitIndex));
   }
}


void
ByteHandler::SetBit(
   uint8_t &Byte,
   uint8_t Pos,
   bool Value)
{
   if(BITS_IN_BYTE <= Pos) { throw std::invalid_argument("Invalid bit position");}
   switch (Value)
   {
      case true: Byte |= BIT(Pos); break;
      case false: Byte &= (~BIT(Pos)); break;
   }
}


bool
ByteHandler::GetBit(
   uint8_t *Arr,
   size_t ArrSz,
   size_t Pos)
{
   if(nullptr == Arr || g_ptrSz > ArrSz || MAX_BIT_POS(ArrSz) < Pos ) { throw std::invalid_argument("Invalid bit position");}
   const size_t byteIndex = ArrSz * BITS_IN_BYTE / Pos;
   const uint8_t bitIndex = BITS_IN_BYTE % Pos;
   return (Arr[byteIndex] & bitIndex) >> bitIndex;
}


bool
ByteHandler::GetBit(
   uint8_t Byte,
   uint8_t Pos)
{
   if(BITS_IN_BYTE <= Pos) 
   {
      throw std::invalid_argument("Invalid bit position");
   }
   return (Byte & BIT(Pos)) >> Pos;
}


uint8_t
ByteHandler::GetHigherBitPos(
   uint8_t Byte)
{
   uint32_t cnt = 0;
   while (Byte >>= 1) { cnt++; }
   return cnt;
}

uint8_t
ByteHandler::GetLowerBitPos(
   uint8_t Byte)
{
   uint32_t cnt = 0;
   if(0 == Byte) { return static_cast<uint8_t>(-1);}

   while (!(Byte & BIT(0))  ) { Byte >>= 1; cnt++; }
   return cnt;
}
