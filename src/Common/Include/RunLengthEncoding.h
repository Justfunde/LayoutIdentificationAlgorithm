#ifndef __RLE_H__
#define __RLE_H__
#include <string>

namespace Rle
{
   double CalcCompressionCoeffitient(bool ByteInterpretation);

   std::string Encode(const std::string &Str);

   std::string Decode(const std::string &EncodedStr);

   std::string ByteEncode(const std::string &Str);

   std::string ByteDecode(const std::string &EncodedStr);
}





#endif //!__RLE_H__