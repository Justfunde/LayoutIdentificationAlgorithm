/**
 * @file ByteHandler.h
 * @author Mikhail Kotlyarov  ((m.kotlyarov@elvis.ru))
 * @brief Declaration of functions working operating bits in byte
 * @version 1
 * @date 2022-12-15
 * @copyright Copyright (c) 2022
 */

#ifndef __BYTE_HANDLER_H__
#define __BYTE_HANDLER_H__
#include <stdint.h>
#include <cstddef>

constexpr uint8_t g_bitsInByte = 8U;///< bits in byte count

using Byte = uint8_t;

/*! Namespace for bit working functions*/
namespace ByteHandler
{
   /**
    * @brief Set bit at byte array
    * 
    * @param Arr Byte array
    * @param ArrSz Byte array size
    * @param Pos Bit pos in Arr
    * @param Value Value to set
    */
   void
   SetBit(
      Byte* Arr,
      size_t ArrSz,
      size_t Pos,
      bool Value);


   /**
    * @brief Get bit from byte array
    * 
    * @param Arr Byte array
    * @param ArrSz Byte array size
    * @param Pos Bit pos in Arr
    * @return true 
    * @return false 
    */
   bool
   GetBit(
      Byte *Arr,
      size_t ArrSz,
      size_t Pos);


   /**
    * @brief Set the Bit in Byte
    * 
    * @param Byte Byte 
    * @param Pos Bit pos in Arr
    * @param Value Value to set
    */
   void
   SetBit(
      Byte &Byte,
      uint8_t Pos,
      bool Value);


   /**
    * @brief Get the Bit from Byte
    * 
    * @param Byte Byte 
    * @param Pos Bit pos
    * @return true 
    * @return false 
    */
   bool
   GetBit(
      Byte Byte,
      uint8_t Pos);
   
   
   /**
    * @brief Get higher 1 value bit from byte
    * 
    * @param Byte Byte
    * @return uint8_t Position
    */
   uint8_t
   GetHigherBitPos(
      Byte Byte);

   /**
    * @brief Get the Lower Bit Pos from byte
    * 
    * @param Byte Byte
    * @return uint8_t Position
    */
   uint8_t
   GetLowerBitPos(
      Byte Byte);

   /**
    * @brief Real byte count calculation
    * Example: 0000000011111111 --> 1 byte
    *          1000000011111111 --> 2 byte
    * 
    * @param Num 
    * @return uint8_t 
    */
   uint8_t
   CalcRealByteCnt(
      size_t Num);
}

#endif //!__BYTE_HANDLER_H__