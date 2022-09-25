/**
* @file     Base64.c
* @brief    Определение функций для работы с Base64
* @ingroup  BDMAPI
*/

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
   const char* Buf2Encode,
   size_t  Buf2EncodeLen,
   bool IsUrl)
{
    if (nullptr == Buf2Encode || 0 == Buf2EncodeLen ) { throw std::invalid_argument("Invalid buffer to encode"); }

    std::string encodedStr;
    encodedStr.reserve(CALC_APPROX_ENCODED_STR_LEN(Buf2EncodeLen));

    char trailingChar = (IsUrl) ? '.' : '=';
    const char* dictionary = (IsUrl) ? g_Base64UrlDictionary : g_Base64ClassicDictionary;

    for (size_t i = 0; i < Buf2EncodeLen; i += 3)
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
    const std::string &EncodedStr)
{
    if (0 == EncodedStr.size() ) { throw std::invalid_argument("Invalid buffer to decode"); }
    
    std::string decodedStr;
    decodedStr.reserve(EncodedStr.length());

    size_t j = 0;
    for (size_t i = 0 ; i < EncodedStr.length(); i += 4)
    {
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
    decodedStr.shrink_to_fit();
    return decodedStr;
}


static
std::string
InBase64DecodeWithSeparators(
  std::string EncodedStr)
{
    EncodedStr.erase(std::remove(EncodedStr.begin(),EncodedStr.end(),'\n'));
    EncodedStr.erase(std::remove(EncodedStr.begin(),EncodedStr.end(),'\r'));
    return InBase64StandardDecode(EncodedStr);
}

static
std::string
InBase64EncodeWithSeparator(
   const char* Buf2Encode,
   size_t  Buf2EncodeLen,
   const std::string &SeparatorStr,
   size_t EachStrLen)
{
    if (0 == SeparatorStr.length() || 0 == EachStrLen) { throw std::invalid_argument("Invalid separator encode parameters"); }
    
    std::string encodedStr =  InBase64Encode(Buf2Encode,Buf2EncodeLen,false);

    for (size_t i = 1 , additionalOffset = 0; i < encodedStr.length() / EachStrLen + 1; additionalOffset+= SeparatorStr.length())
    {
        encodedStr.insert(i * EachStrLen + additionalOffset, SeparatorStr);
    }
    return encodedStr;
}

static
std::string
InBase64EncodePem(
  const std::string &Str2Encode)
{
    return InBase64EncodeWithSeparator(Str2Encode.c_str(),Str2Encode.length(), "\r\n", g_pemStrLen);
}


std::string
InBase64EncodeMime(
const std::string &Str2Encode)
{
   return InBase64EncodeWithSeparator(Str2Encode.c_str(),Str2Encode.length(), "\r\n", g_mimeStrLen);
}



static
std::string
InRadix64Encode(
   const std::string &Str2Encode)
{
   if(Str2Encode.empty()) { return "";}

   std::string encodedStr = InBase64EncodeMime(Str2Encode);
   CRC32_HASH hash = Crc32HashCalc(encodedStr.c_str(),encodedStr.length());
   if(0 == hash) { throw std::runtime_error("Crc32 hash was not calced!");}

   return encodedStr + InBase64Encode(reinterpret_cast<char*>(&hash), sizeof(hash), false);
}


static
std::string
InRadix64Decode(
   const std::string &Str2Decode)
{
   if(Str2Decode.empty()) { return "";}
   if(Str2Decode.length() <= g_radixHashLen) { throw std::invalid_argument("Invalid Radix64 str length!");}

   const std::string oldHashStr = Str2Decode.substr(Str2Decode.length() - g_radixHashLen - 1);
   const std::string noHashStr = Str2Decode.substr(0,Str2Decode.length() - g_radixHashLen);

   CRC32_HASH calcedHash = Crc32HashCalc(noHashStr.c_str(),noHashStr.length());
   const std::string calcedHashStr = InBase64Encode(reinterpret_cast<char*>(&calcedHash), sizeof(calcedHash), false);
   if(calcedHashStr != oldHashStr) { throw std::runtime_error("Crc32 hash did not match!");}
   return InBase64DecodeWithSeparators(noHashStr);
}


std::string
Base64Encode(
   const std::string& Buf2Encode,
   Base64EncodeType EncodingType)
{
    switch (EncodingType)
    {
    case Base64EncodeType::standard: return InBase64Encode(Buf2Encode.c_str(), Buf2Encode.length(), false); 
    case Base64EncodeType::url: return InBase64Encode(Buf2Encode.c_str(), Buf2Encode.length(), true); 
    case Base64EncodeType::radix64:return InRadix64Encode(Buf2Encode); 
    case Base64EncodeType::mime: return InBase64EncodeMime(Buf2Encode); 
    case Base64EncodeType::pem:return InBase64EncodePem(Buf2Encode); 
    default: return "";
    }
}


std::string
Base64Decode(
   const std::string &EncodedStr,
   Base64EncodeType DecodingType)
{
    switch (DecodingType)
    {
    case Base64EncodeType::standard: 
    case Base64EncodeType::url:return InBase64StandardDecode(EncodedStr);
    case Base64EncodeType::radix64:return InRadix64Decode(EncodedStr); 
    case Base64EncodeType::mime:
    case Base64EncodeType::pem:return InBase64DecodeWithSeparators(EncodedStr);
    default: return "";
    }
}