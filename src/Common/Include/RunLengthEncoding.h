#ifndef __RLE_H__
#define __RLE_H__
#include <string>
#include <string_view>

#include "Include/LayoutMatrix.h"

namespace Rle
{
   double CalcCompressionCoeffitient(std::string_view Str, bool ByteInterpretation);

   std::string Encode(std::string_view Str);//todo:допустимый коэфф сжатия

   std::string Decode(std::string_view EncodedStr);

   std::string ByteEncode(std::string_view Str);

   std::string ByteDecode(std::string_view EncodedStr);

   double CalcCompressionCoeffitient(const LayoutMatrix &Matrix);

   std::string Encode(const LayoutMatrix &Matrix);

   LayoutMatrix DecodeMatrix(std::string_view Str, uint32_t RowCount, uint32_t ColCount);

}

#endif //!__RLE_H__