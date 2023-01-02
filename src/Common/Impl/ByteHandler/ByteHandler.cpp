#include "Include/ByteHandler.h"

#include <stdexcept>
#include <limits>


#define MAX_BIT_POS(ArrSz) (g_bitsInByte * (ArrSz) - 1)
#define BIT(n) (1U << (n))


constexpr size_t g_ptrSz = sizeof(void*); 

void
ByteHandler::SetBit(
   Byte *Arr,
   size_t ArrSz,
   size_t Pos,
   bool Value)
{
   if(nullptr == Arr || g_ptrSz > ArrSz || MAX_BIT_POS(ArrSz) < Pos ) { throw std::invalid_argument("Invalid bit position");}
   const size_t byteIndex = ArrSz * g_bitsInByte / Pos;
   const uint8_t bitIndex = g_bitsInByte % Pos;
   switch (Value)
   {
      case true: Arr[byteIndex] |= BIT(bitIndex);
      case false: Arr[byteIndex] &= (~BIT(bitIndex));
   }
}


void
ByteHandler::SetBit(
   Byte &Byte,
   uint8_t Pos,
   bool Value)
{
   if(g_bitsInByte <= Pos) { throw std::invalid_argument("Invalid bit position");}
   switch (Value)
   {
      case true: Byte |= BIT(Pos); break;
      case false: Byte &= (~BIT(Pos)); break;
   }
}


bool
ByteHandler::GetBit(
   Byte *Arr,
   size_t ArrSz,
   size_t Pos)
{
   if(nullptr == Arr || g_ptrSz > ArrSz || MAX_BIT_POS(ArrSz) < Pos ) { throw std::invalid_argument("Invalid bit position");}
   const size_t byteIndex = ArrSz * g_bitsInByte / Pos;
   const uint8_t bitIndex = g_bitsInByte % Pos;
   return (Arr[byteIndex] & bitIndex) >> bitIndex;
}


bool
ByteHandler::GetBit(
   Byte Byte,
   uint8_t Pos)
{
   if(g_bitsInByte <= Pos) 
   {
      throw std::invalid_argument("Invalid bit position");
   }
   return (Byte & BIT(Pos)) >> Pos;
}


uint8_t
ByteHandler::GetHigherBitPos(
   Byte Byte)
{
   uint32_t cnt = 0;
   while (Byte >>= 1) { cnt++; }
   return cnt;
}


uint8_t
ByteHandler::GetLowerBitPos(
   Byte Byte)
{
   uint32_t cnt = 0;
   if(0 == Byte) { return static_cast<uint8_t>(-1);}

   while (!(Byte & BIT(0))  ) { Byte >>= 1; cnt++; }
   return cnt;
}


uint8_t
ByteHandler::CalcRealByteCnt(
   size_t Num)
{
   if(std::numeric_limits<uint8_t>().max() >= Num) { return 1;} //if only one byte

   uint8_t higherBitPos = 0;
   while (Num >>= 1) { higherBitPos++; }

   return higherBitPos / 8 + 1;
}