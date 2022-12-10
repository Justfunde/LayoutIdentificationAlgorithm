#ifndef __BIT_MATRIX_H__
#define __BIT_MATRIX_H__

#include <string_view>


#include "Vector2D.h"


#define BIT(n)         (1U << (n))

class BitMatrix{
protected:
	Vector2D<uint8_t>  	         Bitmap;
	size_t                       Jsize, Isize;
	
public:
	BitMatrix() : Jsize(0), Isize(0) { }

	BitMatrix(size_t RowCnt, size_t ColCnt);

	BitMatrix(const BitMatrix& Rhs);

	BitMatrix(BitMatrix&& Rhs) noexcept;

	virtual ~BitMatrix();


	bool             Get(size_t i, size_t j) const;
	bool             IsAllocated() const;
	uint32_t         GetIsize() const;
	uint32_t         GetJsize() const;
	std::string      ToString() const;

	static BitMatrix FromString(std::string_view Str, uint32_t RowCnt, uint32_t ColCnt);

	void             Set(size_t i, size_t j, bool Value);
	void 				  SetByte(size_t iBytePos, size_t jBytePos, char Value);
	void             SetRange(size_t iStart, size_t jStart, size_t iEnd, size_t jEnd, bool Value);


	void             Ones();
	void             Zeros();
	void             Randm();
	void             Resize(size_t RowCnt, size_t ColCnt);
	void             Print() const noexcept;
	double           OneRatio() const noexcept;
	double           zeroRatio() const noexcept;
	


   BitMatrix&       operator=(const BitMatrix& Rhs);
	BitMatrix&       operator=(BitMatrix&& Rhs) noexcept;
	bool             operator!() noexcept;

	static inline double    Compare(const BitMatrix& FirstMatrix, const BitMatrix& SecondMatrix);

protected:
	//utility methods
	double CalcRatio(bool Value) const noexcept;

	inline void      unsafeSetByte(size_t i, size_t j, bool value);
	inline bool      UnsafeGet(size_t i, size_t j) const;
	inline void      UnsafeSet(size_t i, size_t j, bool Value);
	bool             CpyBitmap(int8_t** Dest, int8_t** Src,size_t ColCnt, const size_t RowCnt);
	void             Reset();

public:
	friend BitMatrix  operator^(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr);
	friend bool       operator==(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept;
	friend bool operator!=(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept;
	friend class LayoutMatrix;
};

#endif //!__BIT_MATRIX_H__