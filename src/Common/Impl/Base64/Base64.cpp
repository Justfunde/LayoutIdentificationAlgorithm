/**
* @file     Base64.c
* @brief    Определение функций для работы с Base64
*/

#include <algorithm>
#include <stdexcept>


#include "Include/Crc32.h"
#include "Include/Base64.h"

static const char* g_Base64ClassicDictionary = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                 "abcdefghijklmnopqrstuvwxyz"
                                                 "0123456789"
                                                 "+/" };///<Стандартный словарь

static const char* g_Base64UrlDictionary = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                              "abcdefghijklmnopqrstuvwxyz"
                                              "0123456789"
                                              "-_" }; ///<Словарь для URL сообщений


constexpr size_t g_radixHashLen = 8;
constexpr size_t g_pemStrLen = 64;
constexpr size_t g_mimeStrLen = 76;
constexpr std::string_view g_endlineStr = "\r\n";


#define CALC_APPROX_ENCODED_STR_LEN(Buf2EncodeLen) ( ( ( (Buf2EncodeLen) * 4 / 3 + 3) < g_pemStrLen) ? ((Buf2EncodeLen) * 4 / 3 + (g_pemStrLen / 2) + g_radixHashLen) : ( (Buf2EncodeLen) * 4 / 3 + (g_pemStrLen / 2) + ((Buf2EncodeLen) * 4 / 3 + 2) / g_pemStrLen * 3))


/**
* @brief       Получение позиции символа в Base64
* @param[in]   Symb    Символ
* @retval      Позиция символа в закодированной Base64 последовательности
**/
static
char
InGetSymbPosition(
   char Symb)
{
   //См схему соответствия "Символ - значение" в Base64
   if (Symb >= 'A' && Symb <= 'Z') return Symb - 'A';
   else if (Symb >= 'a' && Symb <= 'z') return Symb - 'a' + ('Z' - 'A') + 1;
   else if (Symb >= '0' && Symb <= '9') return Symb - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
   else if (Symb == '+' || Symb == '-') return 62;
   else if (Symb == '/' || Symb == '_') return 63;
   else return -1;
}

static
std::string
InBase64Encode(
   std::string_view Buf2Encode,
   bool IsUrl)
{
    if (Buf2Encode.empty()) { throw std::invalid_argument("Invalid buffer to encode"); }

    std::string encodedStr;
    encodedStr.reserve(CALC_APPROX_ENCODED_STR_LEN(Buf2Encode.length()));

    char trailingChar = (IsUrl) ? '.' : '=';
    const char* dictionary = (IsUrl) ? g_Base64UrlDictionary : g_Base64ClassicDictionary;

    for (size_t i = 0, Buf2EncodeLen = Buf2Encode.length(); i < Buf2EncodeLen; i += 3)
    {
       encodedStr.push_back(dictionary[(Buf2Encode[i] & 0xFC) >> 2]);
       if (i + 1 < Buf2EncodeLen)
       {
          encodedStr.push_back(dictionary[((Buf2Encode[i] & 0x03) << 4) | ((Buf2Encode[i + 1] & 0xF0) >> 4)]);
          if (i + 2 < Buf2EncodeLen)
          {
             encodedStr.push_back(dictionary[((Buf2Encode[i + 1] & 0x0F) << 2) | ((Buf2Encode[i + 2] & 0xC0) >> 6)]);
             encodedStr.push_back(dictionary[(Buf2Encode[i + 2] & 0x3F)]);
          }
          else
          {
             encodedStr.push_back(dictionary[((Buf2Encode[i + 1] & 0x0F) << 2)]);
             encodedStr.push_back(trailingChar);
          }
       }
       else
       {
          encodedStr.push_back(dictionary[((Buf2Encode[i] & 0x03) << 4)]);
          encodedStr.push_back(trailingChar);
          encodedStr.push_back(trailingChar);
       }
    }
    return encodedStr;
}


static
std::string
InBase64StandardDecode(
    std::string_view  EncodedStr)
{
    if (0 == EncodedStr.size() ) { throw std::invalid_argument("Invalid buffer to decode"); }
    
    std::string decodedStr(EncodedStr.length(),0);

    size_t j = 0;
    for (size_t i = 0 ; i < EncodedStr.length(); i += 4)
    {
       if (i >= EncodedStr.length()) { break; }
       decodedStr[j] = InGetSymbPosition(EncodedStr[i]) << 2;
       decodedStr[j++] |= (InGetSymbPosition(EncodedStr[i + 1]) >> 4);
       if ('=' != EncodedStr[i + 2] && '.' != EncodedStr[i + 2])
       {
          decodedStr[j] = (InGetSymbPosition(EncodedStr[i + 1]) & 0x0F) << 4;
          decodedStr[j++] |= (InGetSymbPosition(EncodedStr[i + 2]) & 0x3C) >> 2;
          if ('=' != EncodedStr[i + 3] && '.' != EncodedStr[i + 3])
          {
             decodedStr[j] = (InGetSymbPosition(EncodedStr[i + 2]) & 0x03) << 6;
             decodedStr[j++] |= InGetSymbPosition(EncodedStr[i + 3]) & 0x3F;
          }
          else break;
       }
       else break;
    }
    decodedStr.resize(j);
    return decodedStr;
}


static
std::string
InBase64DecodeWithSeparators(
  std::string EncodedStr)
{
    EncodedStr.erase(std::remove(EncodedStr.begin(),EncodedStr.end(),'\n'),EncodedStr.end());
    EncodedStr.erase(std::remove(EncodedStr.begin(),EncodedStr.end(),'\r'), EncodedStr.end());
    return InBase64StandardDecode(EncodedStr);
}


static
std::string
InBase64EncodeWithSeparator(
   std::string_view Buf2Encode,
   std::string_view SeparatorStr,
   size_t EachStrLen)
{
    if (0 == SeparatorStr.length() || 0 == EachStrLen) { throw std::invalid_argument("Invalid separator encode parameters"); }
    
    std::string encodedStr =  InBase64Encode(Buf2Encode,false);
    std::string_view encodedStrView = encodedStr;
    std::string resStr;
    resStr.reserve(CALC_APPROX_ENCODED_STR_LEN(Buf2Encode.length()));

   for (; encodedStrView.length() > EachStrLen;)
   {
      resStr += encodedStrView.substr(0, EachStrLen);
      resStr += SeparatorStr;
      encodedStrView = encodedStrView.substr(EachStrLen);
   }
    resStr += encodedStrView;
    
    resStr.shrink_to_fit();
    return resStr;
}

static
std::string
InBase64EncodePem(
  std::string_view Str2Encode)
{
    return InBase64EncodeWithSeparator(Str2Encode, g_endlineStr, g_pemStrLen);
}


static
std::string
InBase64EncodeMime(
   std::string_view Str2Encode)
{
   return InBase64EncodeWithSeparator(Str2Encode, g_endlineStr, g_mimeStrLen);
}



static
std::string
InRadix64Encode(
   std::string_view Str2Encode)
{
   if(Str2Encode.empty()) { return "";}

   std::string encodedStr = InBase64EncodeMime(Str2Encode);
   CRC32_HASH hash = Crc32::CalcHash(encodedStr.c_str(),encodedStr.length());
   if(0 == hash) { throw std::runtime_error("Crc32 hash was not calced!");}

   std::string str = InBase64Encode(std::string(reinterpret_cast<char*>(&hash),sizeof(hash)), false);
   return encodedStr + InBase64Encode(std::string(reinterpret_cast<char*>(&hash),sizeof(hash)), false);
}


static
std::string
InRadix64Decode(
   const std::string &Str2Decode)
{
   if(Str2Decode.empty()) { return "";}
   if(Str2Decode.length() <= g_radixHashLen) { throw std::invalid_argument("Invalid Radix64 str length!");}

   const std::string crc32HashStr = Str2Decode.substr(Str2Decode.length() - g_radixHashLen);
   const std::string noHashStr = Str2Decode.substr(0,Str2Decode.length() - g_radixHashLen);

   CRC32_HASH calcedHash = Crc32::CalcHash(noHashStr.c_str(),noHashStr.length());
   const std::string calcedHashStr = InBase64Encode(std::string_view(reinterpret_cast<char*>(&calcedHash),sizeof(calcedHash)), false);
   if(calcedHashStr != crc32HashStr) { throw std::runtime_error("Crc32 hash did not match!");}
   return InBase64DecodeWithSeparators(noHashStr);
}


namespace Base64
{
   std::string
   Base64Encode(
      std::string_view Buf2Encode,
      EncodeType EncodingType)
   {
       switch (EncodingType)
       {
       case EncodeType::standard: return InBase64Encode(Buf2Encode, false); 
       case EncodeType::url: return InBase64Encode(Buf2Encode, true); 
       case EncodeType::radix64:return InRadix64Encode(Buf2Encode); 
       case EncodeType::mime: return InBase64EncodeMime(Buf2Encode); 
       case EncodeType::pem:return InBase64EncodePem(Buf2Encode); 
       default: return "";
       }
   }


   std::string
   Base64Decode(
      std::string_view EncodedStr,
      EncodeType DecodingType)
   {
       switch (DecodingType)
       {
       case EncodeType::standard: 
       case EncodeType::url:return InBase64StandardDecode(EncodedStr);
       case EncodeType::radix64:return InRadix64Decode(std::string(EncodedStr.begin(),EncodedStr.end())); 
       case EncodeType::mime:
       case EncodeType::pem:return InBase64DecodeWithSeparators(std::string(EncodedStr.begin(),EncodedStr.end()));
       default: return "";
       }
   }
}