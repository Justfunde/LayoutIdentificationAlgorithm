#include "Include/BitMatrix.h"

#include <iostream>
#include <random>
#include <ctime>


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
		
		const size_t colCnt = (Jsize % g_bitsInByte) ? (Jsize / g_bitsInByte + 1) : (Jsize / g_bitsInByte);

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
		for (size_t j = 0; j < FirstMatr.Bitmap.ColCount(); j++)
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
	const size_t jBytePos = j / g_bitsInByte;
	const size_t jAdditBitPos = g_bitsInByte - 1 - j % g_bitsInByte;
	return ByteHandler::GetBit(Bitmap[iBytePos][jBytePos], jAdditBitPos);
}


bool
BitMatrix::UnsafeGet(
	size_t i,
	size_t j) const
{
	return static_cast<bool>((Bitmap[i][j / g_bitsInByte] & BIT(g_bitsInByte - 1 - j % g_bitsInByte)) >> (g_bitsInByte - 1 - j % g_bitsInByte));
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
	const size_t jBytePos = j / g_bitsInByte;
	const size_t jAdditBitPos = g_bitsInByte - 1 - j % g_bitsInByte;

	ByteHandler::SetBit(Bitmap[iBytePos][jBytePos], jAdditBitPos, Value);
}


void
BitMatrix::SetByte(
	size_t iBytePos,
	size_t jBytePos,
	Byte Value)
{
	if(iBytePos >= Bitmap.RowCount() || jBytePos > Bitmap.ColCount()) { throw std::invalid_argument("Invalid index");}

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
		Bitmap[i][j / g_bitsInByte] |= BIT(g_bitsInByte - 1 - j % g_bitsInByte);
	}
	else
	{
		Bitmap[i][j / g_bitsInByte] &= ~BIT(g_bitsInByte - 1 - j % g_bitsInByte);
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
		for (size_t j = 0; j < Bitmap.ColCount(); j++)
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
	const size_t colCnt = (Jsize % (sizeof(int8_t) * g_bitsInByte)) ? (Jsize / (sizeof(int8_t) * g_bitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_bitsInByte));
	Bitmap.Resize(RowCnt, colCnt);
}


ByteVector
BitMatrix::Serialize() const
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	ByteVector resVector = SerializeSizes();
	ByteVector matrVector = SerializeMatrix();

	resVector.reserve(matrVector.size());
	for(auto it : matrVector)
	{
		resVector.push_back(it);
	}

	return resVector;
}


ByteVector
BitMatrix::SerializeSizes() const
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	ByteVector resultBuf(sizeof(uint8_t) + sizeof(Isize) + sizeof(uint8_t) + sizeof(Jsize), 0);
	ByteVector::iterator it = resultBuf.begin(); 

	*it = sizeof(Isize);
	++it;

	const uint8_t* pBuf = reinterpret_cast<const uint8_t*>(&Isize);
	for(uint8_t i = 0; i < sizeof(Isize); i++)
	{
		*(it++) = pBuf[i]; 
	}

	*it = sizeof(Jsize);
	++it;
	pBuf = reinterpret_cast<const uint8_t*>(&Jsize);
	for(uint8_t i = 0; i < sizeof(Jsize); i++)
	{
		*(it++) = pBuf[i]; 
	}
	return resultBuf;
}


ByteVector
BitMatrix::SerializeMatrix() const 
{
	if((*this).operator!()) { throw std::runtime_error("Cannot serialize. Invalid bitmap");}

	ByteVector byteVec;
	byteVec.reserve(Bitmap.RowCount() * Bitmap.ColCount());

	uint8_t bitCnt = 0, currValue = 0;
	for (size_t i = 0; i < GetRowCount(); i++)
	{
		for (size_t j = 0, colCnt = GetColumnCount(); j < colCnt ; j++)
		{
			ByteHandler::SetBit(currValue, bitCnt, UnsafeGet(i,j));
			bitCnt++;
			if(8 == bitCnt)
			{
				byteVec.push_back(currValue);
				currValue = 0;
				bitCnt = 0;
			}
		}
	}
	byteVec.shrink_to_fit();
	return byteVec;
}


BitMatrix
BitMatrix::DeserializeMatrix(
	const ByteVector& MatrStr)
{
	if(MatrStr.empty()) { throw std::invalid_argument("Invalid string for deserialization"); }

	ByteVector::const_iterator iter = MatrStr.begin();

	uint8_t rowByteCnt = *iter;
	++iter;

	size_t iSize = 0;
	uint8_t* pBuf = reinterpret_cast<uint8_t*>(&iSize);
	for(uint8_t i = 0; i < rowByteCnt; ++i)
	{
		pBuf[i] = *iter;
		++iter;
	}

	uint8_t colByteCnt = *iter;
	++iter;

	size_t jSize = 0;
	pBuf = reinterpret_cast<uint8_t*>(&jSize);
	for(uint8_t i = 0; i < rowByteCnt; ++i)
	{
		pBuf[i] = *iter;
		++iter;
	}

	BitMatrix retVal(iSize, jSize);

	uint8_t bitCnt = 0;
	for(size_t i = 0; i < iSize; ++i)
	{
		for(size_t j = 0; j < jSize; ++j )
		{
			retVal.Set(i,j,ByteHandler::GetBit(*iter,bitCnt));
			bitCnt++;

			if(8 == bitCnt)
			{
				++iter;
				bitCnt = 0;
			}
		}
	}
	return retVal;
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