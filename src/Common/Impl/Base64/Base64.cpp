/**
* @file     Base64.c
* @brief    Определение функций для работы с Base64
* @ingroup  BDMAPI
*/
#include "Include/Base64.h"
#include <stdexcept>

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


///**
//* @brief           Рассчет CRC32 хеш суммы от буфера
//* @param[in]      Buf               Буфер
//* @param[in]      BufSize           Размер буфера
//* @param[out]     Crc32CheckSum     CRC32 hash
//* @retval         status Статус операции
//**/
//static
//BDM_STATUS
//InCalcCrc32CheckSum(
//   IN UINT8*   Buf,
//   IN UINT32   BufSize,
//   OUT UINT32* Crc32CheckSum)
//{
//   BDM_STATUS status = BDM_SUCCESS;
//   do
//   {
//      if (NULL == Buf || 0 == BufSize || NULL == Crc32CheckSum) { SET_STATUS(BDM_INVALID_PARAMETER); break; }
//
//      BDM_HASH_PRM hashPrm = { TRUE };
//      BDM_HASH_32_CRC32* crcData = BdmCreateHashProtocol(BDM_HASH_32_CRC32_SIG, PROP_HASH_CRC32, &hashPrm, sizeof(BDM_HASH_PRM));
//      if (crcData == NULL) { SET_STATUS(BDM_SECURITY_VIOLATION); break; }
//      BDM_HASH* crc = &crcData->VTable;
//
//      status = crc->Hash(crc, Buf, BufSize, Crc32CheckSum);
//      if (BDM_ERROR(status)) { break; }
//
//      crcData->Hdr.Delete(crc);
//   } while (FALSE);
//   return status;
//}


///**
//* @brief       Кодирование буфера с помощью Radix64.
//* @param[out]      EncodedStr        Результирующая строка
//* @param[out]      EncodedStrLen     Длина результирующей строки
//* @param[in]       Buf2Encode        Буфер для кодирования
//* @param[in]       Buf2EncodeLen     Длина буфера для кодирования
//* @retval         status Статус операции
//**/
//UNSAFE
//static
//BDM_STATUS
//InRadix64Encode(
//   OUT CHAR** EncodedStr,
//   OUT UINT32* EncodedStrLen,
//   IN  UINT8* Buf2Encode,
//   IN  UINT32  Buf2EncodeLen)
//{
//   BDM_STATUS status = BDM_SUCCESS;
//   CHAR* encodedCheckSum = NULL;
//   do
//   {
//      if (NULL == EncodedStr || NULL == EncodedStrLen || 0 == Buf2EncodeLen) { SET_STATUS(BDM_INVALID_PARAMETER); break; }
//
//      UINT32 checkSum;
//      status = InCalcCrc32CheckSum(Buf2Encode, Buf2EncodeLen, &checkSum);
//      if (BDM_ERROR(status)) { break; }
//      
//      UINT32 encodedCheckSumLen;
//      status = InBase64Encode(&encodedCheckSum, &encodedCheckSumLen, (UINT8*)&checkSum, sizeof(checkSum), FALSE);
//      if (BDM_ERROR(status)) { break; }
//
//      status = InBase64EncodeMime(EncodedStr, EncodedStrLen, Buf2Encode, Buf2EncodeLen);
//      if (BDM_ERROR(status)) { break; }
//
//      CHAR* pEncodedStr = *EncodedStr;
//      pEncodedStr[(*EncodedStrLen)++] = '=';
//      BdmMemCopy(&pEncodedStr[*EncodedStrLen], encodedCheckSum, encodedCheckSumLen);
//      (*EncodedStrLen) += encodedCheckSumLen;
//      pEncodedStr[*EncodedStrLen] = '\0';
//   } while (FALSE);
//   return status;
//}


///**
//* @brief                             Декодирование Radix64 строки
//* @param[out]      DecodedBuf        Результирующий буфер
//* @param[out]      DecodedBufLen     Размер результирующего буфера
//* @param[in]       EncodedStr        Строка для кодирования
//* @param[in]       EncodedStrLen     Длина строки для декодирования
//* @retval         status Статус операции
//**/
//UNSAFE
//static
//BDM_STATUS
//InRadix64Decode(
//   OUT UINT8** DecodedBuf,
//   OUT UINT32* DecodedBufLen,
//   IN  CHAR* EncodedStr,
//   IN  UINT32  EncodedStrLen)
//{
//   BDM_STATUS status = BDM_SUCCESS;
//
//   UINT8* decodedCheckSum = NULL;
//   UINT32 decodedCheckSumLen;
//   CHAR encodedCheckSum[RADIX64_HASH_LEN] = { '\0' };
//   UINT32 calcedCheckSum;
//   do
//   {
//      if (NULL == DecodedBuf  || NULL == EncodedStr || 0 == EncodedStrLen || NULL == DecodedBufLen) { SET_STATUS(BDM_INVALID_PARAMETER); break; }
//      if ('=' != EncodedStr[EncodedStrLen - RADIX64_HASH_LEN - 1]) { SET_STATUS(BDM_INVALID_PARAMETER); break; }
//
//      BdmMemCopy(encodedCheckSum, &EncodedStr[EncodedStrLen - RADIX64_HASH_LEN], RADIX64_HASH_LEN);
//      EncodedStrLen -= (RADIX64_HASH_LEN + 1);
//
//      status = Base64Decode(&decodedCheckSum, &decodedCheckSumLen, encodedCheckSum, RADIX64_HASH_LEN, Base64Type_standard);
//      if (BDM_ERROR(status)) { break; }
//      
//      if (sizeof(calcedCheckSum) != decodedCheckSumLen) { SET_STATUS(BDM_SECURITY_VIOLATION); break; }
//      
//      status = InBase64DecodeWithSeparators(DecodedBuf, DecodedBufLen, EncodedStr, EncodedStrLen);
//      if (BDM_ERROR(status)) { break; }
//
//      status = InCalcCrc32CheckSum(*DecodedBuf, *DecodedBufLen,&calcedCheckSum);
//      if (BDM_ERROR(status)) { break; }
//
//      if (BdmMemCmp(&calcedCheckSum, decodedCheckSum, sizeof(calcedCheckSum)) != 0) { SET_STATUS(BDM_SECURITY_VIOLATION); break; }
//   } while (FALSE);
//   return status;
//}



std::string
Base64Encode(
   const std::string& Buf2Encode,
   Base64EncodeType EncodingType)
{
    switch (EncodingType)
    {
    case Base64EncodeType::standard: return InBase64Encode(Buf2Encode.c_str(), Buf2Encode.length(), false); break;
    case Base64EncodeType::url: return InBase64Encode(Buf2Encode.c_str(), Buf2Encode.length(), true); break;
    //case Base64EncodeType::radix64:return InRadix64Encode(EncodedStr, EncodedStrLen, Buf2Encode, Buf2EncodeLen); break;
    case Base64EncodeType::mime: return InBase64EncodeMime(Buf2Encode); break;
    case Base64EncodeType::pem:return InBase64EncodePem(Buf2Encode); break;
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
    case Base64EncodeType::url:return InBase64StandardDecode(EncodedStr); break;
   // case Base64EncodeType::radix64:status = InRadix64Decode(DecodedBuf, DecodedBufLen, encodedStrCpy, EncodedStrLen); break;
    case Base64EncodeType::mime:
    case Base64EncodeType::pem:return InBase64DecodeWithSeparators(EncodedStr); break;
    default: return "";
    }
}