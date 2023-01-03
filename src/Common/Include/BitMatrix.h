/**
 * @file BitMatrix.h
 * @author Mikhail Kotlyarov  ((m.kotlyarov@elvis.ru))
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef __BIT_MATRIX_H__
#define __BIT_MATRIX_H__

#include "Vector2D.h"
#include "ByteHandler.h"

#include <string_view>


/**
 * @class BitMatrix
 * @brief Class that can bit matrix data
 */
class BitMatrix{
protected:
	Vector2D<Byte>  	         Bitmap;       ///<Matrix data
	size_t                       Jsize, Isize; ///<Row and col bit count
	
public:

	/**
	 * @brief Construct a new Bit Matrix object
	 * 
	 */
	BitMatrix() : Jsize(0), Isize(0) { }

	/**
	 * @brief Construct a new Bit Matrix object
	 * 
	 * @param RowCnt Row count 
	 * @param ColCnt Column count
	 */
	BitMatrix(
		size_t RowCnt,
	 	size_t ColCnt);


	/**
	 * @brief Copy construct a new Bit Matrix object
	 * 
	 * @param Rhs Object to copy
	 */
	BitMatrix(
		const BitMatrix& Rhs);


	/**
	 * @brief Move construct a new Bit Matrix object
	 * 
	 * @param Rhs 
	 */
	BitMatrix(
		BitMatrix&& Rhs) noexcept;


	/**
	 * @brief Virtual distructor the Bit Matrix object
	 * 
	 */
	virtual ~BitMatrix();


	/**
	 * @brief Bit getter
	 * 
	 * @param i Row number
	 * @param j Column number
	 * @return true 
	 * @return false 
	 */
	bool
	Get(
		size_t i,
		size_t j) const;

	/**
	 * @brief Get the Row Count
	 * 
	 * @return size_t 
	 */
	size_t
	GetRowCount() const;

	
	/**
	 * @brief Get the Column Count
	 * 
	 * @return size_t 
	 */
	size_t
	GetColumnCount() const;

	//____________BEGIN Matrix serialization and deserialization methods____________

	/**
	 * @brief Matrix serialization. Includes sizes
	 * 
	 * @return ByteVector
	 */
	ByteVector
	Serialize() const;


	static
	BitMatrix
	DeserializeMatrix(
		const ByteVector& MatrStr);


	/**
	 * @brief Conversion string to bitMatrix
	 * 
	 * @param Str 
	 * @param RowCnt 
	 * @param ColCnt 
	 * @return BitMatrix 
	 */
	static
	BitMatrix
	FromString(
		std::string_view Str,
		uint32_t RowCnt,
		uint32_t ColCnt);

	//____________END atrix serialization and deserialization methods____________

	/**
	 * @brief Bit setter bit at pos [i][j]
	 * 
	 * @param i Row number
	 * @param j Column number
	 * @param Value Value to set
	 */
	void
	Set(
		size_t i,
		size_t j,
		bool Value);


	/**
	 * @brief Byte setter
	 * 
	 * @param iBytePos Row byte pos(NOT BIT!!!) 
	 * @param jBytePos Column byte pos(NOT BIT!!!)
	 * @param Value Byte value to set
	 */
	void
	SetByte(
		size_t iBytePos,
		size_t jBytePos,
		Byte Value);


	/**
	 * @brief Set Bits in range 
	 * 
	 * @param iStart y0
	 * @param jStart x0
	 * @param iEnd y1
	 * @param jEnd x1
	 * @param Value Bit value to set
	 */
	void
	SetRange(
		size_t iStart,
		size_t jStart,
		size_t iEnd,
		size_t jEnd,
		bool Value);


	/**
	 * @brief Fill matrix with 1
	 * 
	 */
	void 
	Ones();


	/**
	 * @brief Fill matrix with 0
	 * 
	 */
	void
	Zeros();


	/**
	 * @brief Random matrix
	 * 
	 */
	void
	Randm();

	/**
	 * @brief Resize matrix
	 * 
	 * @param RowCnt New row count
	 * @param ColCnt New column count
	 */
	void
	Resize(size_t RowCnt, size_t ColCnt);


	/**
	 * @brief Print matrix. Stdout stream.
	 * 
	 */
	void
	Print() const noexcept;


	/**
	 * @brief One value ratio. One bit count / matrix size
	 * 
	 * @return double 
	 */
	double
	OneRatio() const noexcept;


	/**
	 * @brief One value ratio. One bit count / matrix size
	 * 
	 * @return double 
	 */
	double
	ZeroRatio() const noexcept;
	

	/**
	 * @brief Copy operator=
	 * 
	 * @param Rhs Obj to copy
	 * @return BitMatrix& 
	 */
   	BitMatrix&
	operator=(const BitMatrix& Rhs);

	
	/**
	 * @brief Move operator=
	 * 
	 * @param Rhs Obj to move
	 * @return BitMatrix& 
	 */
	BitMatrix&
	operator=(BitMatrix&& Rhs) noexcept;


	/**
	 * @brief Validity checker
	 * 
	 * @return true BitMatrix is valid
	 * @return false BitMatrix is invalid
	 */
	bool
	operator!() const noexcept;

	/**
	 * @brief Static method for matrix comparison
	 * 
	 * @param FirstMatrix Rhs value
	 * @param SecondMatrix Lhs value
	 * @return double 
	 */
	static
	inline
	double
	Compare(
		const BitMatrix& FirstMatrix,
		const BitMatrix& SecondMatrix);


protected:

	/**
	 * @brief Calc ratio by value
	 * 
	 * @param Value Value to calc
	 * @return double 
	 */
	double
	CalcRatio(
		bool Value) const noexcept;


	/**
	 * @brief Unsafe byte setter. No parameter validation.
	 * 
	 * @param i Byte row number
	 * @param j Byte column number
	 * @param value Value to set
	 */
	inline
	void
	UnsafeSetByte(
		size_t i,
		size_t j,
		bool value);


	/**
	 * @brief Unsafe byte getter. No parameter validation.
	 * 
	 * @param i Byte row number
	 * @param j Byte column number
	 * @return true 
	 * @return false 
	 */
	inline
	bool
	UnsafeGet(
		size_t i,
		size_t j) const;


	/**
	 * @brief Unsafe bit setter. No parameter validation.
	 * 
	 * @param i Byte row number
	 * @param j Byte column number
	 * @param value Value to set
	 */
	inline
	void
	UnsafeSet(
		size_t i,
		size_t j,
		bool Value);


	/**
	 * @brief Matrix reset
	 * 
	 */
	void 
	Reset();

	private:

	/**
	 * @brief Sizes serialization
	 * 
	 * @return ByteVector 
	 */
	ByteVector
	SerializeSizes() const;


	/**
	 * @brief Conversion bitmatrix to string(just concantecating bytes)
	 * 
	 * @return ByteVector 
	 */
	ByteVector
	SerializeMatrix() const;

public:

	/**
	 * @brief Xor overloading. Xoring bytes
	 * 
	 * @param FirstMatr Rhs matrix 
	 * @param SecondMatr Lhs matrix
	 * @return BitMatrix 
	 */
	friend
	BitMatrix 
	operator^(
		const BitMatrix& FirstMatr,
		const BitMatrix& SecondMatr);


	/**
	 * @brief operator== overloading. Xor inside
	 * 
	 * @param FirstMatr Rhs matrix
	 * @param SecondMatr Lhs matrix
	 * @return true 
	 * @return false 
	 */
	friend
	bool
	operator==(
		const BitMatrix& FirstMatr,
		const BitMatrix& SecondMatr) noexcept;


	/**
	 * @brief operator!= overloading. Xor inside
	 * 
	 * @param FirstMatr Rhs matrix
	 * @param SecondMatr Lhs matrix
	 * @return true 
	 * @return false 
	 */
	friend
	bool
	operator!=(
		const BitMatrix& FirstMatr,
		const BitMatrix& SecondMatr) noexcept;

	friend class LayoutMatrix;
};

#endif //!__BIT_MATRIX_H__