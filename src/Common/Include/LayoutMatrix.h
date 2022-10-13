#ifndef __LAYOUT_MATRIX_H__
#define __LAYOUT_MATRIX_H__

#include "Include/BitMatrix.h"
#include "Include/Base64.h"
#include "Include/LayoutData.hpp"


#include <string>
#include <string_view>
#include <memory>
#include <cmath>


struct WorkspaceCoords
{
	Coord leftTop, rightBot;

public:
	WorkspaceCoords():leftTop({0,0}), rightBot({ 0,0 }) {}
	bool setAngleCoords(const Coord& leftTop, const Coord& rightBot);
	bool isInWorkspace(int32_t x, int32_t y);
};

class LayoutMatrix:public BitMatrix
{
public:
	//Constructors
	LayoutMatrix():BitMatrix() {}
	LayoutMatrix(const std::string& in_hash);
	LayoutMatrix(size_t i, size_t j) :BitMatrix(i, j) {}
	LayoutMatrix(const BitMatrix& matrix):BitMatrix(matrix) {}
	LayoutMatrix(const LayoutMatrix& matrix);
	LayoutMatrix(LayoutMatrix&& matrix) noexcept;
	LayoutMatrix(BitMatrix&& matrix) noexcept;


	//Matrix based methods
	std::string GetHash();
	void Hash2Matrix(const std::string &Hash);




	//Hash based methods
	void setHash(const std::string& hash);
	BitMatrix decodeHash();

	
	LayoutMatrix& operator=(const BitMatrix& matrix);
	LayoutMatrix& operator=(BitMatrix&& matrix) noexcept;
	LayoutMatrix& operator=(const LayoutMatrix& matrix);
	LayoutMatrix& operator=(LayoutMatrix&& matrix) noexcept;
	bool operator!() const  { return !static_cast<BitMatrix>(*this);}

	//static methods
	static LayoutMatrix DecodeHash(std::string_view Hash);
	static std::string  EncodeHash(const LayoutMatrix &Matrix);

	static std::string EncodeSz(uint32_t RowCnt, uint32_t ColCnt);
	static char EncodeEncodings(bool Rle, bool Base64);
	static void DecodeSz(std::string_view EncodedSzStr, uint32_t &ColCnt, uint32_t &RowCnt);
	static void DecodeEncodings(char EncodedEncodings, bool &Rle, bool &Base64);
private:
	

	std::string EncodeMatrix();
	BitMatrix DecodeHash();



};

#endif //!__LAYOUT_MATRIX_H__

