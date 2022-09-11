#pragma once
#include "Include/BitMatrix.h"
#include "Base64.h"
#include "LayoutData.hpp"
#include <string>
#include <memory>
#include <cmath>


//struct WorkspaceCoords
//{
//	Coord leftTop, rightBot;
//
//public:
//	WorkspaceCoords():leftTop({0,0}), rightBot({ 0,0 }) {}
//	bool setAngleCoords(const Coord& leftTop, const Coord& rightBot);
//	bool isInWorkspace(int32_t x, int32_t y);
//};

class LayoutMatrix:public BitMatrix
{
private:
	std::string         hash;
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
	
private:
	std::string EncodeMatrix();
	BitMatrix DecodeHash();

	std::pair<std::string, double> RLE_encode();
	void RLE_decode(const std::string& str);


	std::string Base64_encode(const std::string& str,bool RLE);
	std::string Base64_decode(const std::string& str);
	inline char unsafeWriteLastBit(char byte,bool value);
};



