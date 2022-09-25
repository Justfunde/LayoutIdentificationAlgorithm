#include "Include/LayoutMatrix.h"

constexpr std::string_view file_section_param = "__PARAMETERS__";
constexpr std::string_view file_Isize_param = "__I_SIZE__";
constexpr std::string_view file_Jsize_param = "__J_SIZE__";
constexpr std::string_view file_encode_param = "__ENCODE__";

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
		hash = in_hash;
		RLE_decode(Base64_decode(in_hash));
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nLayoutMatrix constructor error:" << exception.what();
	}
}

LayoutMatrix::LayoutMatrix(const LayoutMatrix& matrix) :BitMatrix(matrix)
{
	hash = matrix.hash;
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
	hash = std::move(matrix.hash);
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
	hash = matrix.hash;
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
		hash.clear();
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
	hash = std::move(matrix.hash);
	Bitmap = matrix.Bitmap;
	matrix.Bitmap = nullptr;
	ColCnt = matrix.ColCnt;
	RowCnt = matrix.RowCnt;
	Isize = matrix.Isize;
	Jsize = matrix.Jsize;
	IsAlloced = matrix.IsAlloced;
	return *this;
}






void LayoutMatrix::setHash(const std::string& hash)
{
	this->hash = hash;
	Reset();
}



std::string LayoutMatrix::GetHash()
{
	if (!(*this)) return std::string();
	std::pair <std::string, double> RLE = RLE_encode();
	if (RLE.second < 1)
		hash = Base64_encode(ToString(), 0);
	else hash = Base64_encode(RLE.first, 1);
	return hash;
}

BitMatrix LayoutMatrix::decodeHash()
{
	RLE_decode(Base64_decode(hash));
	return static_cast<BitMatrix>(*this);
}



std::string LayoutMatrix::Base64_encode(const std::string& str,bool RLE)
{
	if (str.empty()) throw std::runtime_error("String for Base64_encode is empty!");
	std::string is_RLE;
	if (RLE)
		is_RLE = "RLE";
	return Base64::Base64Encode(str + std::string(file_section_param) + std::string(file_Isize_param) + std::to_string(Isize) + std::string(file_Jsize_param) + std::to_string(Jsize) + std::string(file_encode_param) + is_RLE,
	Base64::EncodeType::standard);
}



char LayoutMatrix::unsafeWriteLastBit(char byte, bool value)
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

std::string LayoutMatrix::Base64_decode(const std::string& str)
{
	std::string encoded_str = Base64::Base64Decode(str,Base64::EncodeType::standard);
	if(encoded_str.empty()) throw std::runtime_error("String for Base64_decode is empty!");
	const std::string_view parameters = std::string_view(encoded_str).substr(encoded_str.find(file_section_param), encoded_str.length() - encoded_str.find(file_section_param));
	const std::string_view Isize = parameters.substr(parameters.find(file_Isize_param) + file_Isize_param.length(), parameters.find(file_Jsize_param)- parameters.find(file_Isize_param) - file_Isize_param.length());
	const std::string_view Jsize = parameters.substr(parameters.find(file_Jsize_param) + file_Jsize_param.length(), parameters.find(file_encode_param)- parameters.find(file_Jsize_param) - file_Jsize_param.length());

	resize(std::stoull(std::string(Isize)), std::stoull(std::string(Jsize)));

	if (parameters.find("RLE") == std::string::npos)
	{
		const std::string_view str_matr = std::string_view(encoded_str).substr(0, encoded_str.find(file_Isize_param));
		std::string_view::const_iterator it = str_matr.begin();
		for (size_t i = 0; i < RowCnt; i++)
			for (size_t j = 0; j < ColCnt; j++)
			{
				Bitmap[i][j] = *it;
				it++;
			}
		return std::string();
	}
	std::string tmp = encoded_str.substr(0, encoded_str.find(parameters));
	return tmp;
}

