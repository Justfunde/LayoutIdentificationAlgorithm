#include "Include/BitMatrix.h"

#include <iostream>
#include <random>
#include <ctime>

#define BIT(n)         (1U << (n))

constexpr uint8_t g_BitsInByte = 8;

BitMatrix::BitMatrix(
	const BitMatrix& Rhs)
{
	try 
	{
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


BitMatrix::BitMatrix(
	size_t RowCnt,
	size_t ColCnt)
	: Isize(RowCnt)
	, Jsize(ColCnt)
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


BitMatrix::BitMatrix(
	BitMatrix&& Rhs) noexcept
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
BitMatrix&
	BitMatrix::operator=(
		const BitMatrix& Rhs)
{
	if (&Rhs == this)
		return *this;
	
	Bitmap = Rhs.Bitmap;
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	return *this;
}


BitMatrix&
 	BitMatrix::operator=(
		BitMatrix&& Rhs) noexcept
{
	Bitmap = std::move(Rhs.Bitmap);
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	return *this;
}


BitMatrix
operator^(
	const BitMatrix& FirstMatr,
	const BitMatrix& SecondMatr)
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


bool
operator==(
	const BitMatrix& FirstMatr,
	const BitMatrix& SecondMatr) noexcept
{
	return BitMatrix::Compare(FirstMatr, SecondMatr) >= 0.9999;
}


bool
operator!=(
	const BitMatrix& FirstMatr,
	const BitMatrix& SecondMatr) noexcept
{
	return BitMatrix::Compare(FirstMatr, SecondMatr) <= 0.9999;
}


bool
BitMatrix::operator!() const noexcept
{
	if (!Bitmap) { return true;}
	if (0 == Isize || 0 ==  Jsize) { return true;}
	return false;
}


bool
BitMatrix::Get(
	size_t i,
	size_t j) const 
{
	if (i >= Isize || j >= Jsize) {throw std::runtime_error("Invalid index"); }

	// finding byte pos in arr
	const size_t iBytePos = i;
	const size_t jBytePos = j / g_BitsInByte;
	const size_t jAdditBitPos = g_BitsInByte - 1 - j % g_BitsInByte;
	return static_cast<bool>((Bitmap[iBytePos][jBytePos] & BIT(jAdditBitPos)) >> jAdditBitPos);
}


bool
BitMatrix::UnsafeGet(
	size_t i,
	size_t j) const
{
	return static_cast<bool>((Bitmap[i][j / g_BitsInByte] & BIT(g_BitsInByte - 1 - j % g_BitsInByte)) >> (g_BitsInByte - 1 - j % g_BitsInByte));
}


size_t
BitMatrix::GetRowCount() const
{
	return Isize;
}

size_t
BitMatrix::GetColumnCount() const
{
	return Jsize;
}

//Setters

void
BitMatrix::Set(
	size_t i,
	size_t j,
	bool Value)
{
	if (i >= Isize || j >= Jsize) {throw std::runtime_error("Invalid index"); }
	
	//finding byte pos in arr
	const size_t iBytePos = i;
	const size_t jBytePos = j / g_BitsInByte;
	const size_t jAdditBitPos = g_BitsInByte - 1 - j % g_BitsInByte;

	if(Value)
	{
		Bitmap[iBytePos][jBytePos] |= BIT(jAdditBitPos);
	}
	else
	{
		Bitmap[iBytePos][jBytePos] &= ~BIT(jAdditBitPos);
	}
}


void
BitMatrix::SetByte(
	size_t iBytePos,
	size_t jBytePos,
	Byte Value)
{
	if(iBytePos >= Bitmap.RowCount() || jBytePos > Bitmap.ColCnt()) { throw std::invalid_argument("Invalid index");}

	Bitmap[iBytePos][jBytePos] = Value;
}


void
BitMatrix::UnsafeSet(
	size_t i,
	size_t j,
	bool Value)
{
	if(Value)
	{
		Bitmap[i][j / g_BitsInByte] |= BIT(g_BitsInByte - 1 - j % g_BitsInByte);
	}
	else
	{
		Bitmap[i][j / g_BitsInByte] &= ~BIT(g_BitsInByte - 1 - j % g_BitsInByte);
	}
}


void
BitMatrix::UnsafeSetByte(
	size_t i,
	size_t j,
	bool value)
{
	if(value)
	{
		Bitmap[i][j] = 0xFF;
	}
	else
	{
		Bitmap[i][j] = 0;
	}
}


void
BitMatrix::SetRange(
	size_t iStart,
	size_t jStart,
	size_t iEnd,
	size_t jEnd,
	bool Value)
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


void
BitMatrix::Print() const noexcept
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


double
BitMatrix::CalcRatio(
	bool Value) const noexcept
{
	double unit = 0;
	try {

		for (size_t i = 0; i < Isize; i++)
		{
			for (size_t j = 0; j < Jsize; j++)
			{
				if (UnsafeGet(i, j) == Value)
				{
					unit++;
				}
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

double
BitMatrix::OneRatio() const noexcept
{
	return CalcRatio(1);
}


double  BitMatrix::ZeroRatio() const noexcept 
{
	return CalcRatio(0);
}


inline
double
BitMatrix::Compare(
	const BitMatrix& FirstMatrix,
	const BitMatrix& SecondMatrix)
{
	double res = 0;
	try
	{
		res = (FirstMatrix ^ SecondMatrix).OneRatio();
		if (res == -1)
		{
			return res;
		}
		return 1 - res;
	}
	catch (std::exception& exception)
	{
		std::cerr << "\nCompare error:" << exception.what();
		return -1;
	}
}


void
BitMatrix::Ones()
{
	SetRange(0, 0, Isize - 1, Jsize - 1, 1);
}


void
BitMatrix::Zeros()
{
	for (size_t i = 0; i < Bitmap.RowCount(); i++)
	{
		for (size_t j = 0; j < Bitmap.ColCnt(); j++)
		{
			Bitmap[i][j] = 0;
		}
	}
}

void
BitMatrix::Randm()
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

void 
BitMatrix::Resize(
	size_t RowCnt,
	size_t ColCnt)
{
	if (RowCnt <= 0 || ColCnt <= 0) throw std::runtime_error("Matrix resize parameters error!");
	
	Reset();
	Isize = RowCnt;
	Jsize = ColCnt;
	const size_t colCnt = (Jsize % (sizeof(int8_t) * g_BitsInByte)) ? (Jsize / (sizeof(int8_t) * g_BitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_BitsInByte));
	Bitmap.Resize(RowCnt, colCnt);
}


std::string
BitMatrix::SerializeAll() const
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	return SerializeSizes() + SerializeMatrix();
}


std::string
BitMatrix::SerializeSizes() const
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	char resultBuf[2 * sizeof(size_t) + sizeof(uint8_t)];// max buf size. rowCnt + colCnt + byte count of row(4 high bits) + ByteCount of column(4 low bits)
	uint8_t resBufSz = 1;
	constexpr uint8_t sizesPos = 0;


	size_t rowCnt = Bitmap.RowCount();
	size_t colCnt = Bitmap.ColCnt();
	
	const uint8_t rowByteCnt = ByteHandler::CalcRealByteCnt(rowCnt);
	const uint8_t colByteCnt = ByteHandler::CalcRealByteCnt(colCnt);

	//row and col byte count insert
	resultBuf[sizesPos] |= (rowByteCnt << 4);
	resultBuf[sizesPos] |= (colByteCnt & 0x0F); 

	//row serialization
	const uint8_t* pBuf = reinterpret_cast<uint8_t*>(&rowCnt);
	for(uint8_t i = 0; i < rowByteCnt; i++)
	{
		resultBuf[resBufSz++] = pBuf[i];
	}

	//column serialization
	pBuf = reinterpret_cast<uint8_t*>(&colCnt);
	for(uint8_t i = 0; i < colByteCnt; i++)
	{
		resultBuf[resBufSz++] = pBuf[i];
	}

	//resultBuf[resBufSz] = '\0';

	return std::string(resultBuf, resBufSz);

}


std::string
BitMatrix::SerializeMatrix() const 
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	std::string str(Bitmap.RowCount() * Bitmap.ColCnt(), 0);
	
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