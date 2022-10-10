#ifndef __BIT_MATRIX_H
#define __BIT_MATRIX_H


#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <ctime>
#include <string_view>


#define BIT(n)         (1U << (n))

class BitMatrix{
protected:
	int8_t**      	              Bitmap;
	uint32_t                     Jsize, Isize;
	uint32_t                     ColCnt, RowCnt;
	bool                         IsAlloced;
	
public:
	BitMatrix() :Bitmap(nullptr), ColCnt(0), RowCnt(0), Jsize(0), Isize(0), IsAlloced(false) {}
	BitMatrix(size_t RowCnt, size_t ColCnt);
	BitMatrix(const BitMatrix& Rhs);
	BitMatrix(BitMatrix&& Rhs) noexcept;
	virtual ~BitMatrix();


	bool             Get(size_t i, size_t j) const;
	bool             IsAllocated() const;
	uint32_t           GetIsize() const;
	uint32_t           GetJsize() const;
	std::string      ToString() const;

	static BitMatrix FromString(std::string_view Str, uint32_t RowCnt, uint32_t ColCnt);

	void             Set(size_t i, size_t j, bool Value);
	void 				  SetByte(size_t iBytePos, size_t jBytePos, char Value);
	void             SetRange(size_t iStart, size_t jStart, size_t iEnd, size_t jEnd, bool Value);


	void             Ones();
	void             Zeros();
	void             Randm();
	void             resize(size_t RowCnt, size_t ColCnt);
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
	void             AllocMatrix();
	void             Reset();

public:
	friend BitMatrix  operator^(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr);
	friend bool       operator==(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept;
	friend bool operator!=(const BitMatrix& FirstMatr, const BitMatrix& SecondMatr) noexcept;
	friend class LayoutMatrix;
};

#endif //!__BIT_MATRIX_H