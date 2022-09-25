#pragma warning(disable:4293)
#include "Include/LayoutMatrix.h"
#include "Include/RunLengthEncoding.h"
#include "Include/ByteHandler.h"

#include <string_view>
#include <sstream>

constexpr std::string_view file_section_param = "__PARAMETERS__";
constexpr std::string_view file_size_param = "__SIZE__";
constexpr std::string_view file_encode_param = "__ENCODE_TYPE__";

//CoordinateWorkspace

//bool WorkspaceCoords::setAngleCoords(const Coord& leftTop, const Coord& rightBot)
//{
//	if (leftTop.x > rightBot.x || leftTop.y < rightBot.y)
//		return false;
//	this->leftTop = leftTop;
//	this->rightBot = rightBot;
//	return true;
//}
//
//bool WorkspaceCoords::isInWorkspace(int32_t x, int32_t y)
//{
//	if (x >leftTop.x && x < rightBot.x)
//		if (y < leftTop.y && y > rightBot.y)
//			return true;
//	return false;
//}


//LayoutMatrix

//Constrictors

LayoutMatrix::LayoutMatrix(const std::string& in_hash)
{
	try {
		RLE_decode(Base64_decode(in_hash));
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nLayoutMatrix constructor error:" << exception.what();
	}
}

LayoutMatrix::LayoutMatrix(const LayoutMatrix& matrix) :BitMatrix(matrix)
{
}

LayoutMatrix::LayoutMatrix(LayoutMatrix&& matrix) noexcept
{
	ColCnt = matrix.ColCnt;
	RowCnt = matrix.RowCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	IsAlloced = matrix.IsAlloced;
	Bitmap = matrix.Bitmap;
	matrix.Bitmap = nullptr;
}

LayoutMatrix::LayoutMatrix(BitMatrix&& matrix) noexcept
{
	ColCnt = matrix.ColCnt;
	RowCnt = matrix.RowCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	IsAlloced = matrix.IsAlloced;
	Bitmap = matrix.Bitmap;
	matrix.Bitmap = nullptr;
}


//Operators

LayoutMatrix& LayoutMatrix::operator=(const BitMatrix& matrix)
{
	if (!matrix.IsAlloced)
		return *this;
	if (&matrix == this)
		return *this;
	try {
		Reset();
		RowCnt = matrix.RowCnt;
		ColCnt = matrix.ColCnt;
		Isize = matrix.Isize;
		Jsize = matrix.Jsize;

		AllocMatrix();
		CpyBitmap(Bitmap, matrix.Bitmap, ColCnt, RowCnt);
	}
	catch (std::exception& exception)
	{
		std::cerr << "\nLayoutMatrix operator= error:" << exception.what();
		Reset();
	}
	return *this;
}

LayoutMatrix& LayoutMatrix::operator=(const LayoutMatrix& matrix)
{
	if (!matrix.IsAlloced)
		return *this;
	if (&matrix == this)
		return *this;
	try {
	Reset();
	RowCnt = matrix.RowCnt;
	ColCnt = matrix.ColCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	
		AllocMatrix();
		CpyBitmap(Bitmap, matrix.Bitmap, ColCnt, RowCnt);
	}
	catch (std::exception& exception)
	{
		std::cerr << "\nLayoutMatrix operator= error:" << exception.what();
		Reset();
	}
	return *this;
}
	
LayoutMatrix& LayoutMatrix::operator=(BitMatrix&& matrix) noexcept
{
	Reset();

	Bitmap = matrix.Bitmap;
	matrix.Bitmap = nullptr;
	ColCnt = matrix.ColCnt;
	RowCnt = matrix.RowCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	IsAlloced = matrix.IsAlloced;
	return *this;
}

LayoutMatrix& LayoutMatrix::operator=(LayoutMatrix&& matrix) noexcept
{
	Reset();
	Bitmap = matrix.Bitmap;
	matrix.Bitmap = nullptr;
	ColCnt = matrix.ColCnt;
	RowCnt = matrix.RowCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	IsAlloced = matrix.IsAlloced;
	return *this;
}



//
//
//
//
//
//
//std::string LayoutMatrix::GetHash()
//{
//	if (!(*this)) return std::string();
//	std::string hash;
//	std::pair <std::string, double> RLE = RLE_encode();
//	if (RLE.second < 1)
//		hash = Base64_encode(ToString(), 0);
//	else hash = Base64_encode(RLE.first, 1);
//	return hash;
//}
//
//BitMatrix LayoutMatrix::decodeHash()
//{
//	std::string hash;
//	RLE_decode(Base64_decode(hash));
//	return static_cast<BitMatrix>(*this);
//}
//
//
//
//std::string LayoutMatrix::Base64_encode(const std::string& str,bool RLE)
//{
//	if (str.empty()) throw std::runtime_error("String for Base64_encode is empty!");
//	std::string is_RLE;
//	if (RLE)
//		is_RLE = "RLE";
//	return Base64::Base64Encode(str + std::string(file_section_param) + std::string(file_size_param) + std::to_string(Isize) + std::string(file_Jsize_param) + std::to_string(Jsize) + std::string(file_encode_param) + is_RLE,
//	Base64::EncodeType::standard);
//}
//
//
//
//char LayoutMatrix::unsafeWriteLastBit(char byte, bool value)
//{
//	switch (value)
//	{
//	case true:
//		return byte | BIT(7);
//	case false:
//		return byte & ~BIT(7);
//	}
//	return -1;
//}
//
//std::string LayoutMatrix::Base64_decode(const std::string& str)
//{
//	std::string encoded_str = Base64::Base64Decode(str,Base64::EncodeType::standard);
//	if(encoded_str.empty()) throw std::runtime_error("String for Base64_decode is empty!");
//	const std::string_view parameters = std::string_view(encoded_str).substr(encoded_str.find(file_section_param), encoded_str.length() - encoded_str.find(file_section_param));
//	const std::string_view Isize = parameters.substr(parameters.find(file_size_param) + file_size_param.length(), parameters.find(file_Jsize_param)- parameters.find(file_size_param) - file_size_param.length());
//	const std::string_view Jsize = parameters.substr(parameters.find(file_Jsize_param) + file_Jsize_param.length(), parameters.find(file_encode_param)- parameters.find(file_Jsize_param) - file_Jsize_param.length());
//
//	resize(std::stoull(std::string(Isize)), std::stoull(std::string(Jsize)));
//
//	if (parameters.find("RLE") == std::string::npos)
//	{
//		const std::string_view str_matr = std::string_view(encoded_str).substr(0, encoded_str.find(file_size_param));
//		std::string_view::const_iterator it = str_matr.begin();
//		for (size_t i = 0; i < RowCnt; i++)
//			for (size_t j = 0; j < ColCnt; j++)
//			{
//				Bitmap[i][j] = *it;
//				it++;
//			}
//		return std::string();
//	}
//	std::string tmp = encoded_str.substr(0, encoded_str.find(parameters));
//	return tmp;
//}


LayoutMatrix
LayoutMatrix::DecodeHash(
	std::string_view Hash)
{
	if(Hash.empty()) { throw std::invalid_argument("Invalid hash");}
	return LayoutMatrix();
	

}

std::string 
LayoutMatrix::EncodeSz() const
{
	size_t encodedSz = ((GetIsize() << (sizeof(uint32_t) * 8)) & GetJsize());
	std::string encodedSzStr(sizeof(encodedSz), 0);

	char* pData = reinterpret_cast<char*>(&encodedSz);
	for(uint8_t i = 0; i < sizeof(encodedSz); i++)
	{
		encodedSzStr += pData[i];
	}
	return encodedSzStr;
}

char
LayoutMatrix::EncodeEncodings (
	bool Rle,
	bool Base64) const 
{
	uint8_t retVal = 0;
	if(Rle) { ByteHandler::SetBit(retVal,0,1); }
	if(Base64) { ByteHandler::SetBit(retVal,1,1); }
	return static_cast<char>(retVal);
}

std::string 
LayoutMatrix::EncodeHash(
	const LayoutMatrix &Matrix)	
{
	if(!Matrix) { throw std::invalid_argument("Invalid matrix");}
	
	bool isRle = false;
	std::stringstream encodedStr;
	if(Rle::CalcCompressionCoeffitient(Matrix) > 1) { encodedStr << Rle::Encode(Matrix); isRle = true;}
	else { encodedStr << Matrix.ToString();}

	encodedStr << file_section_param << file_size_param << Matrix.EncodeSz() << file_encode_param<< Matrix.EncodeEncodings(isRle,true);
	return Base64::Base64Encode(encodedStr.str(), Base64::EncodeType::radix64);
}