#include "Include/LayoutMatrix.h"
#include "Include/RunLengthEncoding.h"
#include "Include/ByteHandler.h"

#include <string_view>
#include <sstream>
#include <iostream>


namespace MatrixEncodingParamters
{
	constexpr std::string_view sectionParameters = "__PARAMETERS__";
	constexpr std::string_view sectionSize = "__SIZE__";
	constexpr std::string_view sectionEncodeType = "__ENCODE_TYPE__";

	constexpr std::string_view strBegin = "---------BEGIN_MATRIX---------\r\n";
	constexpr std::string_view strEnd = "\r\n---------END_MATRIX---------";

	const size_t parametersLen = sectionParameters.length() + sectionSize.length() + 24 + sectionEncodeType.length() + sizeof(char);
	constexpr uint8_t rleBitPos = 1;
	constexpr uint8_t base64BitPos = 0;
}



//CoordinateWorkspace

bool WorkspaceCoords::setAngleCoords(const Coord& leftTop, const Coord& rightBot)
{
	if (leftTop.x > rightBot.x || leftTop.y < rightBot.y)
		return false;
	this->leftTop = leftTop;
	this->rightBot = rightBot;
	return true;
}

bool WorkspaceCoords::isInWorkspace(int32_t x, int32_t y)
{
	if (x >leftTop.x && x < rightBot.x)
		if (y < leftTop.y && y > rightBot.y)
			return true;
	return false;
}


//LayoutMatrix

//Constrictors

LayoutMatrix::LayoutMatrix(const std::string& in_hash)
{
	try {
		//RLE_decode(Base64_decode(in_hash));
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
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	Bitmap = matrix.Bitmap;
}

LayoutMatrix::LayoutMatrix(BitMatrix&& matrix) noexcept
{
	Isize = matrix.Isize;
	Bitmap = matrix.Bitmap;
}


//Operators

LayoutMatrix& LayoutMatrix::operator=(const BitMatrix& matrix)
{
	if (&matrix == this)
		return *this;
	try {
		Reset();
		Isize = matrix.Isize;
		Jsize = matrix.Jsize;
		Bitmap = matrix.Bitmap;
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
	if (&matrix == this)
		return *this;
	try {
	Reset();
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	Bitmap = matrix.Bitmap;
	
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
	matrix.Bitmap = std::move(matrix.Bitmap);
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	return *this;
}

LayoutMatrix& LayoutMatrix::operator=(LayoutMatrix&& matrix) noexcept
{
	Reset();
	Bitmap = std::move(matrix.Bitmap);
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	return *this;
}


void
LayoutMatrix::DecodeSz(
	std::string_view EncodedSzStr,
	uint32_t &ColCnt,
	uint32_t &RowCnt)
{
	if(EncodedSzStr.length() != 16) { throw std::invalid_argument("Invalid size section!");}

	size_t sz = 0;
	for(int32_t i = 0, offs = 0; i < EncodedSzStr.length(); i++, offs++)
	{
		sz |= (size_t)EncodedSzStr[i] << (offs * 8);
	}
	ColCnt = sz & std::numeric_limits<uint32_t>::max();
	RowCnt = (sz >> 8 * sizeof(uint32_t)) & std::numeric_limits<uint32_t>::max();
}


void 
LayoutMatrix::DecodeEncodings(
	char EncodedEncodings,
	bool& Rle,
	bool& Base64)
{
	Rle = ByteHandler::GetBit(static_cast<uint8_t>(EncodedEncodings), MatrixEncodingParamters::rleBitPos);
	Base64 = ByteHandler::GetBit(static_cast<uint8_t>(EncodedEncodings), MatrixEncodingParamters::rleBitPos);
}

LayoutMatrix
LayoutMatrix::DecodeHash(
	std::string_view Hash)
{
	if(Hash.empty()) { throw std::invalid_argument("Invalid hash");}

	if(Hash.substr(0,MatrixEncodingParamters::strBegin.length()) != MatrixEncodingParamters::strBegin) { throw std::invalid_argument("No begin section!"); }
	if(const size_t endStrLen = MatrixEncodingParamters::strEnd.length();
		 Hash.substr(Hash.length() - endStrLen,endStrLen) != MatrixEncodingParamters::strEnd) { throw std::invalid_argument("No end section");}
	
	bool isBase64 = false;
	bool isRle = false;
	uint32_t rowCnt = 0;
	uint32_t colCnt = 0;

	Hash = Hash.substr(MatrixEncodingParamters::strBegin.length(), Hash.length() - MatrixEncodingParamters::strBegin.length() - MatrixEncodingParamters::strEnd.length());


	std::string decodedHash = Base64::Base64Decode(Hash, Base64::EncodeType::radix64);
	Hash = decodedHash;

	std::string_view parameters = Hash.substr(Hash.length() - MatrixEncodingParamters::parametersLen);
	if(parameters.find(MatrixEncodingParamters::sectionParameters) == std::string::npos) { throw std::invalid_argument("No parameters section");}


	parameters = parameters.substr(MatrixEncodingParamters::sectionParameters.length());
	if(parameters.substr(0,MatrixEncodingParamters::sectionSize.length()) != MatrixEncodingParamters::sectionSize) { throw std::invalid_argument("No size section");}


	parameters = parameters.substr(MatrixEncodingParamters::sectionSize.length());
	LayoutMatrix::DecodeSz(Base64::Base64Decode(parameters.substr(0,24),Base64::EncodeType::standard),colCnt,rowCnt);
	
	parameters = parameters.substr(24);

	if(parameters.substr(0,MatrixEncodingParamters::sectionEncodeType.length()) != MatrixEncodingParamters::sectionEncodeType) { throw std::invalid_argument("No encoding rule section");}
	
	parameters = parameters.substr(MatrixEncodingParamters::sectionEncodeType.length());
	
	LayoutMatrix::DecodeEncodings(parameters[0], isRle, isBase64);

	Hash = Hash.substr(0, Hash.length() - MatrixEncodingParamters::parametersLen);
	if(isRle)
	{
		return Rle::DecodeMatrix(Base64::Base64Decode(Hash,Base64::EncodeType::standard), rowCnt, colCnt);
	}
	return LayoutMatrix::FromString(Hash,rowCnt,colCnt);
}

std::string 
LayoutMatrix::EncodeSz(
	uint32_t RowCnt,
	uint32_t ColCnt)
{
	size_t encodedSz = ColCnt;
	encodedSz |= static_cast<size_t>(RowCnt) << 32;
	//size_t encodedSz = (((size_t)RowCnt << (32)) & ((size_t)ColCnt) | 0xFFFFFFFF00000000);
	std::string encodedSzStr(sizeof(encodedSz), 0);

	char* pData = reinterpret_cast<char*>(&encodedSz);
	for(uint8_t i = 0; i < sizeof(encodedSz); i++)
	{
		encodedSzStr += pData[i];
	}
	return Base64::Base64Encode(encodedSzStr,Base64::EncodeType::standard);
}

char
LayoutMatrix::EncodeEncodings (
	bool Rle,
	bool Base64) 
{
	uint8_t retVal = 0;
	if(Rle) { ByteHandler::SetBit(retVal,MatrixEncodingParamters::rleBitPos, 1); }
	if(Base64) { ByteHandler::SetBit(retVal,MatrixEncodingParamters::base64BitPos, 1); }
	return static_cast<char>(retVal);
}

std::string 
LayoutMatrix::EncodeHash(
	const LayoutMatrix &Matrix)	
{
	if(!Matrix) { throw std::invalid_argument("Invalid matrix");}

	bool isRle = false;
	std::string resultStr;
	//if(Rle::CalcCompressionCoeffitient(Matrix) > 1) { 
	resultStr = Rle::Encode(Matrix); 
	isRle = true;//}
	//else { resultStr = Matrix.Serialize();}


	resultStr = Base64::Base64Encode(resultStr, Base64::EncodeType::standard);

	resultStr += MatrixEncodingParamters::sectionParameters;
	resultStr += MatrixEncodingParamters::sectionSize;
	resultStr += LayoutMatrix::EncodeSz(Matrix.GetRowCount(), Matrix.GetColumnCount());
	resultStr += MatrixEncodingParamters::sectionEncodeType;
	resultStr += LayoutMatrix::EncodeEncodings(isRle,true);

	resultStr = Base64::Base64Encode(resultStr, Base64::EncodeType::radix64);
	resultStr.reserve(MatrixEncodingParamters::strBegin.length() + resultStr.length() + MatrixEncodingParamters::strEnd.length());
	
	resultStr.insert(0,MatrixEncodingParamters::strBegin);
	resultStr += MatrixEncodingParamters::strEnd;
	return resultStr;
}