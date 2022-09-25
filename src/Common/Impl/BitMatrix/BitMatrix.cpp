#include "Include/BitMatrix.h"


constexpr uint8_t g_BitsInByte = 8;


BitMatrix::BitMatrix(size_t RowCnt, size_t ColCnt): Isize(RowCnt),Jsize(ColCnt)
{
	try{
	RowCnt = RowCnt;
	ColCnt = (Jsize % (sizeof(int8_t) * g_BitsInByte)) ? (Jsize / (sizeof(int8_t) * g_BitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_BitsInByte));
	
	AllocMatrix();
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nBitMatrix constructor error:" << exception.what();
		Reset();
	}
}


BitMatrix::BitMatrix(const BitMatrix& Rhs) :IsAlloced(false)
{
	try {
		RowCnt = Rhs.RowCnt;
		ColCnt = Rhs.ColCnt;
		Isize = Rhs.Isize;
		Jsize = Rhs.Jsize;

		if (Rhs.IsAlloced) { AllocMatrix();}

		if (!CpyBitmap(Bitmap, Rhs.Bitmap, ColCnt, RowCnt)) { throw std::runtime_error("Matrix copy constructor error"); }
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nBitMatrix constructor error:" << exception.what();
		Reset();
	}
}

BitMatrix::BitMatrix(BitMatrix&& Rhs) noexcept
{
	ColCnt = Rhs.ColCnt;
	RowCnt = Rhs.RowCnt;
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	IsAlloced = Rhs.IsAlloced;
	Bitmap = Rhs.Bitmap;
	Rhs.Bitmap = nullptr;
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
	
	BitMatrix tmp = Rhs;
	*this = std::move(tmp);
	return *this;
}

BitMatrix& BitMatrix::operator=(BitMatrix&& Rhs) noexcept
{
	Reset();
	Bitmap = Rhs.Bitmap;
	Rhs.Bitmap = nullptr;
	ColCnt = Rhs.ColCnt;
	RowCnt = Rhs.RowCnt;
	Isize = Rhs.Isize;
	Jsize = Rhs.Jsize;
	IsAlloced = Rhs.IsAlloced;
	return *this;
}

BitMatrix operator^(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr)
{
	if (!FirstMatr.IsAlloced || !FirstMatr.Bitmap)   { throw std::invalid_argument("First argument is not allocated!"); }
	if (!SecondMatr.IsAlloced || !SecondMatr.Bitmap) { throw std::invalid_argument("Second argument is not allocated!");}
	if (FirstMatr.Jsize != SecondMatr.Jsize || FirstMatr.Isize != SecondMatr.Isize) { throw std::invalid_argument("Objects have uncomparable sizes!");}

	BitMatrix result(FirstMatr.Isize, FirstMatr.Jsize);
	if (!result) { throw std::runtime_error("Result XOR object was not constructed");}

	for (size_t i = 0; i < FirstMatr.RowCnt; i++)
	{
		for (size_t j = 0; j < FirstMatr.ColCnt; j++)
		{
			result.Bitmap[i][j] = FirstMatr.Bitmap[i][j] ^ SecondMatr.Bitmap[i][j];
		}
	}
	return result;
}

bool operator==(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept
{
	return static_cast<bool>(BitMatrix::Compare(FirstMatr, SecondMatr)) == 1;
}

bool BitMatrix::operator!() noexcept
{
	if (!IsAlloced||!Bitmap) { return true;}
	if (Isize == 0 || Jsize == 0) { return true;}
	return false;
}

//Getters

bool BitMatrix::Get(size_t i, size_t j) const 
{
	if (!IsAlloced || !Bitmap) { throw std::runtime_error("Matrix is not alloced!");}
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
	return IsAlloced; 
}

size_t BitMatrix::GetIsize() const
{
	return Isize;
}

size_t BitMatrix::GetJsize() const
{
	return Jsize;
}

//Setters

void BitMatrix::Set(size_t i, size_t j, bool Value)
{
	if (!IsAlloced || !Bitmap) { throw std::runtime_error("Matrix is not alloced!"); }
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
	if (!IsAlloced||!Bitmap) throw std::runtime_error("Matrix is not alloced!");
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
	if (!IsAlloced || !Bitmap) { std::cout << "NULL"; }

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
	if (!IsAlloced || !Bitmap) throw std::runtime_error("Matrix is not alloced!");

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
	if (!IsAlloced || !Bitmap) throw std::runtime_error("Matrix is not alloced!");
	for (size_t i = 0; i < RowCnt; i++)
		for (size_t j = 0; j < ColCnt; j++)
			Bitmap[i][j] = 0;
}

void BitMatrix::Randm()
{
	if (!IsAlloced || !Bitmap) throw std::runtime_error("Matrix is not alloced!");
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

void BitMatrix::resize(size_t RowCnt, size_t ColCnt)
{
	if (RowCnt <= 0 || ColCnt <= 0) throw std::runtime_error("Matrix resize parameters error!");
	
	Reset();
	RowCnt = Isize = RowCnt;
	Jsize = ColCnt;
	ColCnt = (Jsize % (sizeof(int8_t) * g_BitsInByte)) ? (Jsize / (sizeof(int8_t) * g_BitsInByte) + 1) : (Jsize / (sizeof(int8_t) * g_BitsInByte));
	try {
		AllocMatrix();
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\nStandard error:" << exception.what();
		Reset();
	}
}


std::string BitMatrix::ToString() const 
{
	if (!IsAlloced || !Bitmap) { return std::string(); }
	if (Isize == 0 || Jsize == 0) { return std::string(); }

	std::string str;
	str.reserve(ColCnt * RowCnt);
	for (size_t i = 0; i < RowCnt; i++)
	{
		for (size_t j = 0; j < ColCnt; j++)
		{
			str += Bitmap[i][j];
		}
	}
	return str;
}

//utility methods


bool BitMatrix::CpyBitmap(int8_t** Dest, int8_t** Src,size_t ColCnt, const size_t RowCnt)
{
	if (!Dest || !Src) {return false;}
		
	for (size_t i = 0; i < RowCnt; i++)
	{
		for (size_t j = 0; j < ColCnt; j++)
			Dest[i][j] = Src[i][j];
	}
	return true;
}

void BitMatrix::AllocMatrix()
{
	Bitmap = new int8_t * [RowCnt];
	for (size_t i = 0; i < RowCnt; i++)
	{
		Bitmap[i] = new int8_t[ColCnt];
	}
	for (size_t i = 0; i < RowCnt; i++)
	{
		for (size_t j = 0; j < ColCnt; j++)
		{
			Bitmap[i][j] = 0;
		}
	}
	IsAlloced = true;
}

void BitMatrix::Reset()
{
	if (Bitmap)
	{
		for (size_t i = 0; i < RowCnt; i++)
		{
			delete[] Bitmap[i];
			Bitmap[i] = nullptr;
		}
		delete[] Bitmap;
		Bitmap = nullptr;
	}
	IsAlloced = false;
	Isize = 0;
	Jsize = 0;
	ColCnt = 0;
	RowCnt = 0;
}