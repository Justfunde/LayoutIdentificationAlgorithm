#include <iostream>
#include <random>
#include <ctime>

#include "Include/BitMatrix.h"


constexpr uint8_t g_BitsInByte = 8;

BitMatrix::BitMatrix(const BitMatrix& Rhs)
{
	try {
		Bitmap = Rhs.Bitmap;
		Isize = Rhs.Isize;
		Jsize = Rhs.Jsize;
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nBitMatrix constructor error:" << exception.what();
		Reset();
	}
}


BitMatrix::BitMatrix(size_t RowCnt, size_t ColCnt): Isize(RowCnt),Jsize(ColCnt)
{
	try{
		
		const size_t colCnt = (Jsize % (sizeof(int8_t) * g_BitsInByte)) ? (Jsize / (sizeof(int8_t) * g_BitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_BitsInByte));

		Bitmap.Resize(RowCnt, colCnt);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nBitMatrix constructor error:" << exception.what();
		Reset();
	}
}




BitMatrix::BitMatrix(BitMatrix&& Rhs) noexcept
{
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	Bitmap = std::move(Rhs.Bitmap);
}

BitMatrix::~BitMatrix()
{
	Reset();
}


//overloaded operators
BitMatrix& BitMatrix::operator=(const BitMatrix& Rhs)
{
	if (&Rhs == this)
		return *this;
	
	Bitmap = Rhs.Bitmap;
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	return *this;
}

BitMatrix& BitMatrix::operator=(BitMatrix&& Rhs) noexcept
{
	Reset();
	Bitmap = std::move(Rhs.Bitmap);
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	return *this;
}

BitMatrix operator^(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr)
{
	if (FirstMatr.Jsize != SecondMatr.Jsize || FirstMatr.Isize != SecondMatr.Isize) { throw std::invalid_argument("Objects have uncomparable sizes!");}

	BitMatrix result(FirstMatr.Isize, FirstMatr.Jsize);
	if (!result) { throw std::runtime_error("Result XOR object was not constructed");}

	for (size_t i = 0; i < FirstMatr.Bitmap.RowCount(); i++)
	{
		for (size_t j = 0; j < FirstMatr.Bitmap.ColCnt(); j++)
		{
			result.Bitmap[i][j] = FirstMatr.Bitmap[i][j] ^ SecondMatr.Bitmap[i][j];
		}
	}
	return result;
}

bool operator==(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept
{
	return BitMatrix::Compare(FirstMatr, SecondMatr) >= 0.9999;
}

bool operator!=(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept
{
	return BitMatrix::Compare(FirstMatr, SecondMatr) <= 0.9999;
}

bool BitMatrix::operator!() noexcept
{
	if (!Bitmap) { return true;}
	if (Isize == 0 || Jsize == 0) { return true;}
	return false;
}

//Getters

bool BitMatrix::Get(size_t i, size_t j) const 
{
	if (i >= Isize || j >= Jsize) {throw std::runtime_error("Invalid index"); }
	//finding byte pos in arr
	const size_t iBytePos = i;
	const size_t jBytePos = j / g_BitsInByte;
	const size_t jAdditBitPos = g_BitsInByte - 1 - j % g_BitsInByte;
	return static_cast<bool>((Bitmap[iBytePos][jBytePos] & BIT(jAdditBitPos)) >> jAdditBitPos);
}

bool BitMatrix::UnsafeGet(size_t i, size_t j) const
{
	return static_cast<bool>((Bitmap[i][j / g_BitsInByte] & BIT(g_BitsInByte - 1 - j % g_BitsInByte)) >> (g_BitsInByte - 1 - j % g_BitsInByte));
}

bool BitMatrix::IsAllocated() const 
{ 
	return true; 
}

uint32_t BitMatrix::GetIsize() const
{
	return Isize;
}

uint32_t BitMatrix::GetJsize() const
{
	return Jsize;
}

//Setters

void BitMatrix::Set(size_t i, size_t j, bool Value)
{
	if (i >= Isize || j >= Jsize) {throw std::runtime_error("Invalid index"); }
	
	//finding byte pos in arr
	const size_t iBytePos = i;
	const size_t jBytePos = j / g_BitsInByte;
	const size_t jAdditBitPos = g_BitsInByte - 1 - j % g_BitsInByte;

	switch (Value)
	{
	case true:
		Bitmap[iBytePos][jBytePos] |= BIT(jAdditBitPos);
		break;
	case false:
		Bitmap[iBytePos][jBytePos] &= ~BIT(jAdditBitPos);
		break;
	}
}

void BitMatrix::SetByte(size_t iBytePos, size_t jBytePos, char Value)
{
	if(iBytePos >= Bitmap.RowCount() || jBytePos > Bitmap.ColCnt()) { throw std::invalid_argument("Invalid index");}

	Bitmap[iBytePos][jBytePos] = Value;
}


void BitMatrix::UnsafeSet(size_t i, size_t j, bool Value)
{
	switch (Value)
	{
	case true:
		Bitmap[i][j / g_BitsInByte] |= BIT(g_BitsInByte - 1 - j % g_BitsInByte);
		break;
	case false:
		Bitmap[i][j / g_BitsInByte] &= ~BIT(g_BitsInByte - 1 - j % g_BitsInByte);
		break;
	}
}


void  BitMatrix::unsafeSetByte(size_t i, size_t j, bool value)
{
	switch (value)
	{
	case true:
		Bitmap[i][j] = 0xFF;
		break;
	case false:
		Bitmap[i][j] = 0;
	}
}


void BitMatrix::SetRange(size_t iStart, size_t jStart, size_t iEnd, size_t jEnd, bool Value)
{
	if (iStart > iEnd || iEnd > Isize) throw std::runtime_error("Invalid i one of parameter");
	if (jStart > jEnd || jEnd > Jsize) throw std::runtime_error("Invalid j one ofparameter");

	for(size_t i = iStart;i <= iEnd; i++)
	{
		for(size_t j = jStart; j <= jEnd; j++)
		{
			UnsafeSet(i, j, Value);
		}
	}
}


void BitMatrix::Print() const noexcept
{
	for (size_t i = 0; i < Isize; i++)
	{
		for (size_t j = 0; j < Jsize; j++)
		{
			std::cout <<  UnsafeGet(i, j);
		}
		std::cout << "\n";
	}
}

double BitMatrix::CalcRatio(bool Value) const noexcept
{
	double unit = 0;
	try {

		for (size_t i = 0; i < Isize; i++)
		{
			for (size_t j = 0; j < Jsize; j++)
			{
				if (UnsafeGet(i, j) == Value)
					unit++;
			}
		}
	}
	catch (const std::exception &exception)
	{
		std::cerr << "\nUnitRatio error:" << exception.what();
		return -1;
	}

	return unit / static_cast<double>(Isize * Jsize);
}

double BitMatrix::OneRatio() const noexcept
{
	return CalcRatio(1);
}


double  BitMatrix::zeroRatio() const noexcept 
{
	return CalcRatio(0);
}


inline double BitMatrix::Compare(const BitMatrix& FirstMatrix, const BitMatrix& SecondMatrix)
{
	double res = 0;
	try
	{
		res = (FirstMatrix ^ SecondMatrix).OneRatio();
		if (res == -1)
			return res;
		return 1 - res;
	}
	catch (std::exception& exception)
	{
		std::cerr << "\nCompare error:" << exception.what();
		return -1;
	}
}


void BitMatrix::Ones()
{
	SetRange(0, 0, Isize - 1, Jsize - 1, 1);
}

void BitMatrix::Zeros()
{
	for (size_t i = 0; i < Bitmap.RowCount(); i++)
		for (size_t j = 0; j < Bitmap.ColCnt(); j++)
			Bitmap[i][j] = 0;
}

void BitMatrix::Randm()
{
	std::mt19937 engine; 
	engine.seed(std::time(nullptr));
	for (size_t i = 0; i < Isize; i++)
	{
		for (size_t j = 0; j < Jsize; j++)
		{
			bool val = (engine() % 2 == 0) ? true : false;
			UnsafeSet(i, j, val);
		}
	}

}

void BitMatrix::Resize(size_t RowCnt, size_t ColCnt)
{
	if (RowCnt <= 0 || ColCnt <= 0) throw std::runtime_error("Matrix resize parameters error!");
	
	Reset();
	Isize = RowCnt;
	Jsize = ColCnt;
	const size_t colCnt = (Jsize % (sizeof(int8_t) * g_BitsInByte)) ? (Jsize / (sizeof(int8_t) * g_BitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_BitsInByte));
	Bitmap.Resize(RowCnt, colCnt);
}


std::string BitMatrix::ToString() const 
{
	if (Isize == 0 || Jsize == 0) { return std::string(); }

	std::string str;
	str.reserve(Bitmap.RowCount() * Bitmap.ColCnt());
	for (size_t i = 0; i < Bitmap.RowCount(); i++)
	{
		for (size_t j = 0; j < Bitmap.ColCnt(); j++)
		{
			str += Bitmap[i][j];
		}
	}
	return str;
}



//utility methods



void BitMatrix::Reset()
{
	Bitmap.Reset();
	Isize = 0;
	Jsize = 0;
}


BitMatrix
BitMatrix::FromString(
	std::string_view Str,
	uint32_t RowCnt,
	uint32_t ColCnt)
{
	if(Str.length() != RowCnt * ColCnt) { throw std::invalid_argument("Invalid strlen");}

	BitMatrix matrix(RowCnt,ColCnt);
	for(uint32_t i = 0, strIndex = 0; i < RowCnt / 8 + (RowCnt % 8 != 0 ); i++)
	{
		for(uint32_t j = 0; j < ColCnt / 8 + (ColCnt % 8 != 0 ); j++)
		{
			matrix.SetByte(i,j,Str[strIndex++]);
		}
	}
	return matrix;
}