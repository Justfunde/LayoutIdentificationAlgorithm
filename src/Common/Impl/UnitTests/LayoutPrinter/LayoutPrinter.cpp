#include "Include/LayoutPrinter.h"

#include <stdexcept>
#include <string_view>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <boost/format.hpp>


constexpr std::string_view g_invalidGeometryMsg = "Invalid Geometry";
constexpr size_t g_spaceSz = 4;

namespace
{

	std::string
	InGetGeometryDataStr(
		const Geometry* Geom,
		size_t Depth = 0)
	{
		if(nullptr == Geom) { throw std::invalid_argument(g_invalidGeometryMsg.data());}

		const size_t currSpace = g_spaceSz * Depth;

		std::string_view res;
		boost::format format ("%1%Geometry {\n"
								 "%2%Layer = %3%\n"
								 "%4%Min = {%5%, %6%}\n"
								 "%7%Max = {%8%, %9%}\n"
								 "%10%}\n");

		const std::string headerSpacer(currSpace, ' ');
		const std::string dataSpacer(currSpace + 2, ' ');
		
		return (format % headerSpacer %
				dataSpacer % Geom->layer %
				dataSpacer %  Geom->min.x % Geom->min.y %
				dataSpacer %  Geom->max.x % Geom->max.y % 
				dataSpacer).str();
	}


	void 
	InPrintRectangle(
		const Geometry* Rect,
		size_t Depth = 0)
	{
		if(nullptr == Rect || GeometryType::rectangle != Rect->type) { throw std::invalid_argument(g_invalidGeometryMsg.data());}


		const Rectangle* pRect = static_cast<const Rectangle*>(Rect);

		const size_t currSpace = g_spaceSz * Depth;
		constexpr size_t leftTopInd = 0;
		constexpr size_t rightBotInd = 2;
		const std::string headerSpacer(currSpace, ' ');
		const std::string dataSpacer(currSpace + 2, ' ');

		const std::string headerStr = InGetGeometryDataStr(Rect, Depth + 1);
		boost::format format("%1%Rectangle{\n"
						  "%2%\n"
						  "%3%Type %4%\n"
						  "%5%LeftTop = {%6%, %7%}\n"
						  "%8%RightBot = {%9%, %10%}\n"
						  "%11%}\n");

		std::cout << (format % headerSpacer %
					  headerStr %
					  dataSpacer % pRect->rectType %
					  dataSpacer % pRect->coords[leftTopInd].x % pRect->coords[leftTopInd].y %
					  dataSpacer % pRect->coords[rightBotInd].x % pRect->coords[rightBotInd].y %
					  headerSpacer).str();
	}
}


void
LayoutPrinter::PrintGeometry(
	const Geometry* Geom,
	size_t Depth)
{
	try
	{
		if(nullptr == Geom) { throw std::invalid_argument(g_invalidGeometryMsg.data());}
		switch (Geom->type)
		{
			case GeometryType::path :   break;
			case GeometryType::rectangle : InPrintRectangle(Geom, Depth); break;
 		
			default: break;		
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}