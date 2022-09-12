#include "Include/ByteHandler.h"
#include "Include/RunLengthEncoding.h"

constexpr uint8_t g_maxRleCounterValue = 0b01111111;
constexpr uint8_t g_lastBitPos = BITS_IN_BYTE - 1;

 
double Rle::CalcCompressionCoeffitient(bool ByteInterpretation);


std::string Rle::Encode(const std::string &Str)
{
   return "";
}


std::string Rle::Decode(const std::string &EncodedStr)
{
   return "";
}


std::string Rle::ByteEncode(const std::string &Str)
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


std::string Rle::ByteDecode(const std::string &EncodedStr)
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