/**
* @file     Base64.h
* @brief    Объявление функций для работы с Base64
* @ingroup  BDMAPI
*/
#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <string>
#include <string_view>


namespace Base64
{
   /*! Enum для определения класса Base64*/
   enum class EncodeType
   {
      standard,
      url,
      radix64,
      mime,
      pem
   };


   std::string
   Base64Encode(
      std::string_view Buf2Encode,
      EncodeType EncodingType);

   std::string
   Base64Decode(
      std::string_view EncodedStr,
      EncodeType DecodingType);
}

#endif // !__BASE_64_H__
