/**
 * @file LayoutMatrix.h
 * @author Mikhail Kotlyarov  ((m.kotlyarov@elvis.ru))
 * @brief Matrix processing layout data
 * @version 0.1
 * @date 2023-02-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LAYOUT_MATRIX_H__
#define __LAYOUT_MATRIX_H__

#include "Include/BitMatrix.h"
#include "Include/Base64.h"
#include "Include/LayoutData.hpp"


#include <string>
#include <string_view>
#include <memory>
#include <cmath>


class LayoutMatrix;
using LayoutMatrixPtr = std::shared_ptr<LayoutMatrix>;

/**
 * @brief Struct for desctiprion of workspace coordinates
 * @class WorkspaceCoords
 */
struct WorkspaceCoords
{
	Coord leftTop, rightBot; ///< Coordinates of workspace

public:
	/**
	 * @brief Construct a new Workspace Coords object
	 * 
	 */
	WorkspaceCoords():leftTop({0,0}), rightBot({ 0,0 }) {}

	/**
	 * @brief Set angle coordinates of workspace
	 * 
	 * @param leftTop Left top coordinates of workspace
	 * @param rightBot Right bot coordinates of workspace
	 * @return true 
	 * @return false 
	 */
	bool
	SetAngleCoords(
		const Coord& LeftTop,
		const Coord& RightBot);


	/**
	 * @brief Check if point inside workspace
	 * 
	 * @param Point Point coordinates
	 * @return true 
	 * @return false 
	 */
	bool
	isInWorkspace(
		const Coord& Point);
};



/**
 * @brief Class for handling and hashing layout zonding info
 * @class LayoutMatrix
 */
class LayoutMatrix : public BitMatrix
{
public:
	
	/**
	 * @brief Construct a new Layout Matrix object
	 */
	LayoutMatrix():BitMatrix() {}


	/**
	 * @brief Construct a new Layout Matrix object
	 * 
	 * @param RowCnt Row count 
	 * @param ColCnt Column count
	 */
	LayoutMatrix(
		size_t RowCnt,
		size_t ColCnt) :BitMatrix(RowCnt, ColCnt) {}


	/**
	 * @brief Construct a new Layout Matrix object(copy)
	 * 
	 * @param Matrix Matrix object
	 */
	LayoutMatrix(
		const BitMatrix& Matrix):BitMatrix(Matrix) {}

	
	/**
	 * @brief Construct a new Layout Matrix object(copy)
	 * 
	 * @param Matrix Matrix object
	 */
	LayoutMatrix(
		const LayoutMatrix& Matrix);


	/**
	 * @brief Construct a new Layout Matrix object(move)
	 * 
	 * @param Matrix Matrix object
	 */
	LayoutMatrix(
		LayoutMatrix&& Matrix) noexcept;


	/**
	 * @brief Construct a new Layout Matrix object(move)
	 * 
	 * @param Matrix Matrix object
	 */
	LayoutMatrix(
		BitMatrix&& Matrix) noexcept;


public:

	/**
	 * @brief Calc matrix hash
	 * 
	 * @return std::string Hashing result
	 */
	std::string
	GetHash() const;


	/**
	 * @brief Matrix initialization by hash decoding
	 * 
	 * @param Hash 
	 */
	void
	InitMatrFromHash(
		std::string_view Hash);



	/**
	 * @brief Encoding and flushing matrix into file
	 * 
	 * @param FileName Name of file
	 * @return true 
	 * @return false 
	 */
	bool
	FlushFile(
		const std::string& FileName);

	/**
	 * @brief Reading file and hash decoding
	 * 
	 * @param FileName Name of file
	 * @return true 
	 * @return false 
	 */
	bool
	ReadFile(
		const std::string& FileName);


public:

	/**
	 * @brief Copy overloaded operator=
	 * 
	 * @param Matrix Matrix to copy
	 * @return LayoutMatrix& 
	 */
	LayoutMatrix&
	operator=(const BitMatrix& Matrix);


	/**
	 * @brief Move overloaded operator=
	 * 
	 * @param Matrix Matrix to move
	 * @return LayoutMatrix& 
	 */
	LayoutMatrix&
	operator=(BitMatrix&& Matrix) noexcept;


	/**
	 * @brief Copy overloaded operator=
	 * 
	 * @param Matrix Matrix to copy
	 * @return LayoutMatrix& 
	 */
	LayoutMatrix&
	operator=(const LayoutMatrix& Matrix);


	/**
	 * @brief Move overloaded operator=
	 * 
	 * @param Matrix Matrix to move
	 * @return LayoutMatrix& 
	 */
	LayoutMatrix&
	operator=(LayoutMatrix&& Matrix) noexcept;


	/**
	 * @brief Overloaded operator !
	 * 
	 * @return true 
	 * @return false 
	 */
	bool
	operator!() const  { return !static_cast<BitMatrix>(*this); }



	//static methods

	/**
	 * @brief Creating matrix from hash
	 * 
	 * @param Hash Matrix hash
	 * @return LayoutMatrix 
	 */
	static
	LayoutMatrix
	DecodeHash(
		std::string_view Hash);


	/**
	 * @brief Calc matrix hash
	 * 
	 * @param Matrix Matrix to hash
	 * @return std::string 
	 */
	static
	std::string
	EncodeHash(
		const LayoutMatrix& Matrix);


	/**
	 * @brief Encode size of matrix
	 * 
	 * @param RowCnt Row count
	 * @param ColCnt Column count
	 * @return std::string 
	 */
	static
	std::string
	EncodeSz(
		uint32_t RowCnt,
		uint32_t ColCnt);


	/**
	 * @brief Encode used encodings
	 * 
	 * @param Rle Flag of Rle
	 * @param Base64 Flag of Base64
	 * @return char 
	 */
	static
	char
	EncodeEncodings(
		bool Rle,
		bool Base64);


	/**
	 * @brief Decode matrix sizes
	 * 
	 * @param EncodedSzStr Encoded str contains sizes
	 * @param ColCnt Decoded column count
	 * @param RowCnt Decoded row count
	 */
	static
	void
	DecodeSz(
		std::string_view EncodedSzStr,
		uint32_t &ColCnt,
		uint32_t &RowCnt);


	/**
	 * @brief Decode used encodings
	 * 
	 * @param EncodedEncodings Encoded str contains encodings flag
	 * @param Rle Decoded RLE use flag
	 * @param Base64 Decoded Base64 use flag
	 */
	static
	void
	DecodeEncodings(
		char EncodedEncodings,
		bool &Rle,
		bool &Base64);
};

#endif //!__LAYOUT_MATRIX_H__

