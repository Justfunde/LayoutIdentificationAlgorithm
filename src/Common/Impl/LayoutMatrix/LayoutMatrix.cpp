#pragma warning(disable:4293)
#include "Include/LayoutMatrix.h"
#include "Include/RunLengthEncoding.h"
#include "Include/ByteHandler.h"

#include <string_view>
#include <sstream>

namespace MatrixEncodingParamters
{
	constexpr std::string_view sectionParameters = "__PARAMETERS__";
	constexpr std::string_view sectionSize = "__SIZE__";
	constexpr std::string_view sectionEncodeType = "__ENCODE_TYPE__";

	constexpr std::string_view strBegin = "---------BEGIN_MATRIX---------\r\n";
	constexpr std::string_view strEnd = "\r\n---------END_MATRIX---------";

	const size_t parametersLen = sectionParameters.length() + sectionSize.length() + sizeof(size_t) + sectionEncodeType.length() + sizeof(char);

}



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
//	return Base64::Base64Encode(str + std::string(sectionParameters) + std::string(sectionSize) + std::to_string(Isize) + std::string(file_Jsize_param) + std::to_string(Jsize) + std::string(sectionEncodeType) + is_RLE,
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
//	const std::string_view parameters = std::string_view(encoded_str).substr(encoded_str.find(sectionParameters), encoded_str.length() - encoded_str.find(sectionParameters));
//	const std::string_view Isize = parameters.substr(parameters.find(sectionSize) + sectionSize.length(), parameters.find(file_Jsize_param)- parameters.find(sectionSize) - sectionSize.length());
//	const std::string_view Jsize = parameters.substr(parameters.find(file_Jsize_param) + file_Jsize_param.length(), parameters.find(sectionEncodeType)- parameters.find(file_Jsize_param) - file_Jsize_param.length());
//
//	resize(std::stoull(std::string(Isize)), std::stoull(std::string(Jsize)));
//
//	if (parameters.find("RLE") == std::string::npos)
//	{
//		const std::string_view str_matr = std::string_view(encoded_str).substr(0, encoded_str.find(sectionSize));
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

void
LayoutMatrix::DecodeSz(
	std::string_view EncodedSzStr,
	uint32_t &ColCnt,
	uint32_t &RowCnt)
{
	if(EncodedSzStr.length() != sizeof(size_t)) { throw std::invalid_argument("Invalid size section!");}

	size_t sz = 0;
	for(int32_t i = EncodedSzStr.length() - 1, offs = 0; 0 != i; i--, offs++)
	{
		sz |= EncodedSzStr[i] << offs * 8;
	}
	ColCnt = sz & std::numeric_limits<uint32_t>::max();
	RowCnt = (sz >> 8 * sizeof(uint32_t)) & std::numeric_limits<uint32_t>::max();
}


void 
LayoutMatrix::DecodeEncodings(
	char EncodedEncodings,
	bool &Rle,
	bool &Base64)
{
	

}

LayoutMatrix
LayoutMatrix::DecodeHash(
	std::string_view Hash)
{
	if(Hash.empty()) { throw std::invalid_argument("Invalid hash");}

	if(Hash.substr(0,MatrixEncodingParamters::strBegin.length()) != MatrixEncodingParamters::strBegin) { throw std::invalid_argument("No begin section!"); }
	if(const size_t endStrLen = MatrixEncodingParamters::strEnd.length();
		 Hash.substr(Hash.length() - endStrLen - 1,endStrLen) != MatrixEncodingParamters::strEnd) { throw std::invalid_argument("No end section");}
	

	Hash = Hash.substr(MatrixEncodingParamters::strBegin.length(), Hash.length() - MatrixEncodingParamters::strBegin.length() - MatrixEncodingParamters::strEnd.length());

	std::string_view parameters = Hash.substr(Hash.length() - MatrixEncodingParamters::parametersLen);
	if(parameters.find(MatrixEncodingParamters::sectionParameters) == std::string::npos) { throw std::invalid_argument("No parameters section");}

	parameters = parameters.substr(MatrixEncodingParamters::sectionParameters.length());
	if(parameters.substr(MatrixEncodingParamters::sectionSize.length()) != MatrixEncodingParamters::sectionSize) { throw std::invalid_argument("No size section");}

	
		
	bool isRle = false;
	bool isBase64 = false;
	uint32_t colCnt = 0;
	uint32_t rowCnt = 0;

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
	std::string resultStr;
	if(Rle::CalcCompressionCoeffitient(Matrix) > 1) { resultStr = Rle::Encode(Matrix); isRle = true;}
	else { resultStr = Matrix.ToString();}

	resultStr.reserve( resultStr.length() + MatrixEncodingParamters::sectionParameters.length() + MatrixEncodingParamters::parametersLen);

	resultStr += MatrixEncodingParamters::sectionParameters;
	resultStr += MatrixEncodingParamters::sectionSize;
	resultStr += Matrix.EncodeSz();
	resultStr += MatrixEncodingParamters::sectionEncodeType;
	resultStr += Matrix.EncodeEncodings(isRle,true);

	resultStr = Base64::Base64Encode(resultStr, Base64::EncodeType::radix64);
	resultStr.reserve(MatrixEncodingParamters::strBegin.length() + resultStr.length() + MatrixEncodingParamters::strEnd.length());
	
	resultStr.insert(0,MatrixEncodingParamters::strBegin);
	resultStr += MatrixEncodingParamters::strEnd;
	return resultStr;
}