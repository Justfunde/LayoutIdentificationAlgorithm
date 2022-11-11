#ifndef __MSK_READER_H__
#define __MSK_READER_H__
/*
 * LayoutReader.hpp
 *
 * Layout formats implementation:
 * GDSII Binary - Dmitry A. Bulakh
 * MSK          - Mikhail S. Kotlyarov 
 */
#include <string>


#include "LayoutData.hpp"
#include "LayoutReader.hpp"


class MskReader:public LayoutReader
{
private:
	Library*			p_activeLibrary;
	Element*			p_activeElement;
	Geometry*      	 	p_active_geometry_item;

public:
	MskReader() :p_activeLibrary(nullptr), p_activeElement(nullptr), p_active_geometry_item(nullptr) {}

	bool            IsMyFormat(const std::wstring& fName) override final;
	bool            Read(LayoutData* layout) override final;

private:
	void ReadSectionRectangle(const std::string& FileLine);
	void ReadBoundingBox(const std::string& FileLine);
	void ReadTitle(const std::string& FileLine);

	inline bool     ReadRecCoords(const std::string& line, Coord& left_bot, Coord& right_top, std::string& layer_name);
	void            FillBox(Geometry* filling_box, const Coord& right_top, const Coord& left_bot, uint16_t layer_num);
	std::string     GetElemName();

	int16_t         ConvertMskLayerNum(const std::string& layer_name);
	std::vector<Layer>::iterator FindByLayerNum(std::vector <Layer>& all_layers, int16_t layer_num);
	inline std::string GetFname(std::wstring Path);

	inline int32_t  calcDelta(int32_t first, int32_t second);
};

#endif //!__MSK_READER_H__