#ifndef __RLE_H__
#define __RLE_H__
#include <string>
#include <string_view>

#ifdef MATRIX_IMPLEMENTED
#include "Include/LayoutMatrix.h"
#endif //!MATRIX_IMPLEMENTED

namespace Rle
{
   double CalcCompressionCoeffitient(std::string_view Str, bool ByteInterpretation);

   std::string Encode(std::string_view Str);

   std::string Decode(std::string_view EncodedStr);

   std::string ByteEncode(std::string_view Str);

   std::string ByteDecode(std::string_view EncodedStr);

   #ifdef MATRIX_IMPLEMENTED
   double CalcCompressionCoeffitient(const LayoutMatrix &Matrix);

   std::string Encode(const LayoutMatrix &Matrix);

   LayoutMatrix DecodeMatrix(std::string_view Str, uint32_t RowCount, uint32_t ColCount);
   
   #endif //!MATRIX_IMPLEMENTED
}

#endif //!__RLE_H__