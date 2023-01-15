/**
 * @file Base64.h
 * @author Mikhail Kotlyarov  ((m.kotlyarov@elvis.ru))
 * @brief Function declaration for base64 algh
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <string>
#include <string_view>
#include <stdexcept>
namespace Base64
{
   /*! Enum for type Base64 definition*/
   enum class EncodeType
   {
      standard,
      url,
      radix64,
      mime,
      pem
   };


   /**
    * @brief Encoding buf using base64 algh
    * 
    * @param Buf2Encode Buffer to encode
    * @param EncodingType Base64 type
    * @return std::string 
    */
   std::string
   Base64Encode(
      std::string_view Buf2Encode,
      EncodeType EncodingType);

   /**
    * @brief Decoding buf using base64 algh
    * 
    * @param Buf2Decode Buffer to decode
    * @param DecodingType Base64 type
    * @return std::string 
    */
   std::string
   Base64Decode(
      std::string_view Buf2Decode,
      EncodeType DecodingType);
}

#endif // !__BASE_64_H__
