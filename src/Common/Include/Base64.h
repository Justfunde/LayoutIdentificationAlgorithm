/**
* @file     Base64.h
* @brief    Объявление функций для работы с Base64
* @ingroup  BDMAPI
*/
#ifndef __BASE_64_H__
#define __BASE_64_H__

#include <string>


/*! Enum для определения класса Base64*/
enum class Base64EncodeType
{
   standard,
   url,
   radix64,
   mime,
   pem
};


std::string
Base64Encode(
   const std::string& Buf2Encode,
   Base64EncodeType EncodingType);

std::string
Base64Decode(
   const std::string &EncodedStr,
   Base64EncodeType DecodingType);

#endif // !__BASE_64_H__
