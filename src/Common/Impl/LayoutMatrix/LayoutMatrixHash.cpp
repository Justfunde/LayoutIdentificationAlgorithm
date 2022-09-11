#include "Include/LayoutMatrix.h"

std::string LayoutMatrix::EncodeMatrix()
{
   
}


BitMatrix LayoutMatrix::DecodeHash()
{

}

//________________________RLE_________________________
std::pair<std::string,double> LayoutMatrix::RLE_encode()
{
	std::string encodedStr;
   constexpr uint8_t maxRleValue = 0b01111111; 

	char key = 0;
	bool curr_value = Get(0, 0);
	for (size_t i = 0; i < Isize; i++)
	{
		for (size_t j = 0; j < Jsize; j++)
		{
			if (curr_value == Get(i, j))
			{
				key++;
			}
			else {
				encodedStr += unsafeWriteLastBit(key, curr_value);
				key = 1;
				curr_value = !curr_value;
			}
			if (key == 0b01111111)
			{
				encodedStr += unsafeWriteLastBit(key, curr_value);
				key = 0;
			}
		}
	}

	encodedStr += unsafeWriteLastBit(key, curr_value);
	double compression_coeff = static_cast<double>(ColCnt * RowCnt) / static_cast<double>(encodedStr.length()) ;
	return std::make_pair(encodedStr, compression_coeff);
}

void LayoutMatrix::RLE_decode(const std::string& str)
{
	if (str.empty())
		return;
	size_t j = 0, k = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		char tmp = str[i];
		bool value = (tmp & BIT(7)) >> 7;
		tmp = unsafeWriteLastBit(tmp, 0);

		while (tmp != 0)
		{
			Set(k, j, value);
			j++;
			if (j == Jsize)
			{
				k++;
				j = 0;
			}
			tmp--;
		}
	}
}