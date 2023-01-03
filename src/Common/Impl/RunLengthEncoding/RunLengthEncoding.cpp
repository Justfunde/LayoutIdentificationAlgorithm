#include "Include/ByteHandler.h"
#include "Include/RunLengthEncoding.h"

constexpr uint8_t g_maxRleCounterValue = 0b01111111;
constexpr uint8_t g_lastBitPos = g_bitsInByte - 1;

 
double 
Rle::CalcCompressionCoeffitient(
	std::string_view Str,
	bool ByteInterpretation)
{
	if(Str.empty()) { return 0;}

	const double strLen = Str.length();
	if(ByteInterpretation) { return strLen / static_cast<double>(ByteEncode(Str).length());}
	return strLen / static_cast<double>(Encode(Str).length());
}


std::string
Rle::Encode(
	std::string_view Str)
{
   return "";
}


std::string
Rle::Decode(
	std::string_view EncodedStr)
{
   return "";
}


std::string
Rle::ByteEncode(
	std::string_view Str)
{
   std::string encodedStr;
   encodedStr.reserve(Str.length());

	uint8_t key = 0;
	bool currBitValue = ByteHandler::GetBit(Str[0], g_lastBitPos);
	for (size_t i = 0; i < Str.length(); i++)
	{
      for(int8_t j = 7; j >= 0; j--)
      {
         if (currBitValue == ByteHandler::GetBit(Str[i],j))
			{
				key++;
			}
			else {
				ByteHandler::SetBit(key, g_lastBitPos, currBitValue);
				encodedStr += key;
				key = 1;
				currBitValue = !currBitValue;
			}
			if (g_maxRleCounterValue == key)
			{
				ByteHandler::SetBit(key, g_lastBitPos, currBitValue);
				encodedStr += key;
				key = 0;
			}
       }
	}
	ByteHandler::SetBit(key, g_lastBitPos, currBitValue);
	encodedStr += key;

   encodedStr.shrink_to_fit();
   return encodedStr;
}


std::string
 Rle::ByteDecode(
	std::string_view EncodedStr)
{
   if (EncodedStr.empty()) { return "";}

   std::string decodedStr;
   decodedStr.reserve(EncodedStr.length());

	size_t j = 0, k = 0;
	for (size_t i = 0; i < EncodedStr.length(); i++)
	{
		uint8_t counter = EncodedStr[i];
		bool value = ByteHandler::GetBit(EncodedStr[i], 7);
		ByteHandler::SetBit(counter, g_lastBitPos, 0);
      uint8_t byte = 0;
      uint8_t bitPosition = 7;
		while (counter != 0)
		{
			ByteHandler::SetBit(byte, bitPosition, value);
			if (0 == g_maxRleCounterValue)
			{
				k++;
				j = 0;
			}
			counter--;
		}
	}
   decodedStr.shrink_to_fit();
   return decodedStr;
}

double
 Rle::CalcCompressionCoeffitient(
	const LayoutMatrix &Matrix)
{
	if(!Matrix) { return 0;}
	return static_cast<double>(Matrix.Serialize().size()) / static_cast<double>(Encode(Matrix).length());
}


std::string
 Rle::Encode(
	const LayoutMatrix &Matrix)
{
	if(!Matrix) { return "";}

	std::string encoded;
	encoded.reserve(Matrix.GetRowCount() * Matrix.GetRowCount());

	bool curr_value = Matrix.Get(0, 0);
	uint8_t key = 0;

	for (size_t i = 0; i < Matrix.GetRowCount(); i++)
	{
		for (size_t j = 0; j < Matrix.GetColumnCount(); j++)
		{
			if (curr_value == Matrix.Get(i, j))
			{
				key++;
			}
			else {
				ByteHandler::SetBit(key, g_lastBitPos, curr_value);
				encoded += key;
				key = 1;
				curr_value = !curr_value;
			}
			if (key == g_maxRleCounterValue)
			{
				ByteHandler::SetBit(key, g_lastBitPos, curr_value);
				encoded += key;
				key = 0;
			}
		}
	}

	ByteHandler::SetBit(key, g_lastBitPos, curr_value);
	encoded += key;
	encoded.shrink_to_fit();
	return encoded;
}


LayoutMatrix 
Rle::DecodeMatrix(
	std::string_view Str,
	uint32_t RowCount,
	uint32_t ColCount)
{

	if (Str.empty()) { return LayoutMatrix();}

	LayoutMatrix matrix(RowCount,ColCount);

	size_t j = 0, k = 0;
	for (size_t i = 0; i < Str.length(); i++)
	{
		try
		{
			uint8_t tmp = Str[i];
			bool value = ByteHandler::GetBit(tmp, g_lastBitPos);
			ByteHandler::SetBit(tmp, g_lastBitPos, 0);
		
		while (tmp != 0)
		{
			matrix.Set(k, j, value);
			j++;
			if (j == matrix.GetColumnCount())
			{
				k++;
				j = 0;
			}
			tmp--;
		}
		}
		catch (...)
		{
			int k = 5;
			k++;
		}
	}
	return matrix;
}