#include "Include/RunLengthEncoding.h"

constexpr uint8_t g_maxRleCounterValue = 0b01111111;

#define BIT(n)         (1U << (n))
#define GET_BIT(value, n) (((value) & (n)) >> n)


char unsafeWriteLastBit(char byte, bool value)
{
	switch (value)
	{
	case true:
		return byte | BIT(7);
	case false:
		return byte & ~BIT(7);
	}
	return -1;
}

void InSetBit(char &Byte, uint8_t Pos, bool Value)
{
   if(7 > Pos) { return;}

   switch (Value)
   {
   case true: Byte |= BIT(Pos);
   case false: Byte &=~BIT(Pos);
   }
}


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

	char key = 0;
	bool currBitValue = GET_BIT(Str[0],7);
	for (size_t i = 0; i < Str.length(); i++)
	{
      for(int8_t j = 7; j >= 0; j--)
      {
         if (currBitValue == GET_BIT(Str[i],j))
			{
				key++;
			}
			else {
				encodedStr += unsafeWriteLastBit(key, currBitValue);
				key = 1;
				currBitValue = !currBitValue;
			}
			if (g_maxRleCounterValue == key)
			{
				encodedStr += unsafeWriteLastBit(key, currBitValue);
				key = 0;
			}

       }
	}
	encodedStr += unsafeWriteLastBit(key, currBitValue);
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
		char counter = EncodedStr[i];
		bool value = GET_BIT(EncodedStr[i], 7);
		counter = unsafeWriteLastBit(counter, 0);
      char byte = 0;
      uint8_t bitPosition = 7;
		while (counter != 0)
		{
			InSetBit(byte,bitPosition,value);
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