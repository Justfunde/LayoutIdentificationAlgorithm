/*
 * LayoutReader.hpp
 *
 * Layout formats implementation:
 * GDSII Binary - Dmitry A. Bulakh
 * MSK          - Mikhail S. Kotlyarov 
 */
#ifndef __MSK_READER_H__
#define __MSK_READER_H__

#include <string>


#include "LayoutData.hpp"
#include "LayoutReader.hpp"

using LayerNum = int16_t;

class MskReader:public LayoutReader
{
private:
	Library*			p_activeLibrary;
	Element*			p_activeElement;
	Geometry*      	 	p_active_geometry_item;

public:
	MskReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_active_geometry_item(nullptr) {}

	bool
	IsMyFormat(
		const std::wstring& fName) override final;


	bool
	Read(
		LayoutData* layout) override final;

private:

	/**
	 * @brief Reading title section of file
	 * 
	 * @param FileLine Line contains title
	 */
	void
	ReadSectionTitle(
		const std::string& FileLine);


	/**
	 * @brief Reading BB section of file
	 * 
	 * @param FileLine Line contains BB
	 */
	void
	ReadSectionBoundingBox(
		const std::string& FileLine);


	/**
	 * @brief Reading Rec section of file
	 * 
	 * @param FileLine Line contains Rec
	 */
	void
	ReadSectionRectangle(
		const std::string& FileLine);


	/**
	 * @brief Parsing rectangle file line
	 * 
	 * @param Line File line contains rec
	 * @param LeftBot LeftTop coord of rec
	 * @param RightTop RigthBot coord of rec
	 * @param LayerName Layer name of rec
	 * @return true 
	 * @return false 
	 */
	inline
	bool
	ParseRecLine(
		const std::string& Line,
		Coord& LeftBot,
		Coord& RightTop,
		std::string& LayerName);


	/**
	 * @brief Filling Box Geometry
	 * 
	 * @param Box2Fill Box to fill
	 * @param RightTop RigthBot coord of Box
	 * @param LeftBot LeftBot coord of Box
	 * @param LNum Layer number
	 */
	void
	FillBox(
		Geometry* Box2Fill,
		const Coord& RightTop,
		const Coord& LeftBot,
		LayerNum LNum);


	/**
	 * @brief Substr elem name from full file path
	 * 
	 * @return std::string 
	 */
	std::string
	GetElemName();

	/**
	 * @brief Conversion LayerName to number
	 * 
	 * @param LayerName Name of Layer
	 * @return LayerNum 
	 */
	LayerNum
	LayerNameToLayerNum(
		const std::string& LayerName);

	
	/**
	 * @brief Calculating delta(sec - first)
	 * 
	 * @param first 
	 * @param second 
	 * @return int32_t 
	 */
	inline
	int32_t
	CalcDelta(
		int32_t first,
		int32_t second);
};

#endif //!__MSK_READER_H__