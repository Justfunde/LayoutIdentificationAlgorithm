#include <stdexcept>

#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutConverter.h"

LayoutBitmapGenerator::LayoutBitmapGenerator() 
: fragmentMatrix(2, 2)
, dx(0)
, dy(0)
{
}


//Public methods

void
LayoutBitmapGenerator::Init(
	LayoutData* Data,
	const std::vector <int16_t>& Layers,
	const Coord& Min = { std::numeric_limits<int32_t>().min(), std::numeric_limits<int32_t>().min()},
	const Coord& Max = { std::numeric_limits<int32_t>().max(), std::numeric_limits<int32_t>().max()})
{
	if (!Data || Layers.empty() || !fragmentMatrix) throw std::invalid_argument("Invalid LayoutBitmapGenerator initialization parameters");

	Reset();

	const Coord leftTop  = { Min.x, Max.y };
	const Coord rightBot = { Max.x, Min.y };
	if (!preloadedData.bitmapCoords.SetAngleCoords(leftTop, rightBot)) throw std::invalid_argument("Invalid LayoutBitmapGenerator initialization parameters");
		
	preloadedData.data = Data;
	preloadedData.layers = Layers;
}


bool
LayoutBitmapGenerator::Process(
	size_t RowCount,
	size_t ColumnCount)
{		
	if (0 == RowCount) throw std::invalid_argument("Invalid row count");
	if (0 == ColumnCount) throw std::invalid_argument("Invalid column count");

	//if (0 != RowCount % fragmentMatrix.RowCount()  || 0 != ColumnCount % fragmentMatrix.ColCount()) { return false; }

	bitmap->Resize(RowCount, ColumnCount);

	for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
	{
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			fragmentMatrix[i][j].SetMatrix(bitmap);
		}
	}

	dx = СalcDelta(preloadedData.bitmapCoords.leftTop.x, preloadedData.bitmapCoords.rightBot.x, fragmentMatrix.ColCount());
	dy = СalcDelta(preloadedData.bitmapCoords.leftTop.y, preloadedData.bitmapCoords.rightBot.y, fragmentMatrix.RowCount());

	InitGeometryItems();
	InitFragmentsWorkspaces();
	DistributeGeometries();
	InitFragmentsIndicies();
	FirstMatrixInit();

	//bitmap.print();
	//cout << "\n\n\nFilling matrix:\n";
	for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
	{
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			//cout << "\nFragment[" << i << "][" << j << "]\n";
			fragmentMatrix[i][j].Process();
		}
	}
	//cout << endl << endl << endl << endl;
	bitmap->Print();
	return true;	
}


inline
double
СalcDelta(
	int32_t N1,
	int32_t N2,
	uint32_t PartCnt)
{
	return fabs(N2 - N1) / PartCnt;
}

LayoutMatrixPtr
LayoutBitmapGenerator::GetMatrix() const
{
	return bitmap;
}


//First matrix initialization

void LayoutBitmapGenerator::FirstMatrixInit()
{
	dx = СalcDelta(preloadedData.bitmapCoords.leftTop.x, preloadedData.bitmapCoords.rightBot.x,bitmap->GetRowCount());
	dy = СalcDelta(preloadedData.bitmapCoords.leftTop.y, preloadedData.bitmapCoords.rightBot.y, bitmap->GetColumnCount());
	

	for (auto it : geometryList)
	{
		switch (it->type)
		{
			case GeometryType::polygon:
			{
				ZondPolygon(it.get());
				break;
			}
			case GeometryType::rectangle:
			{
				ZondRectangle(it.get());
				break;
			}
			default: {throw std::runtime_error("Invalid geometry type");}
		}
	}

	for (auto it : legacyGeometryList)
	{
		switch (it->type)
		{
			case GeometryType::polygon:
			{
				ZondPolygon(it);
				break;
			}
			case GeometryType::rectangle:
			{
				ZondRectangle(it);
				break;
			}
			default: { throw std::runtime_error("Invalid geometry type"); }
		}
	}
}


//Fragment initialization

void LayoutBitmapGenerator::InitFragmentsWorkspaces()
{
	for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
	{
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			WorkspaceCoords tmp;
			tmp.SetAngleCoords(Coord(preloadedData.bitmapCoords.leftTop.x + j * dx, preloadedData.bitmapCoords.leftTop.y - i * dy),
							   Coord(preloadedData.bitmapCoords.leftTop.x + (j + 1) * dx, preloadedData.bitmapCoords.leftTop.y - (i + 1) * dy));
			fragmentMatrix[i][j].SetWorkspaceCoords(tmp);
			fragmentMatrix[i][j].SetMatrix(bitmap);
			//printf("[%d][%d]:\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", i, j, fragments[i][j].angleCoords.leftTop.x, fragments[i][j].angleCoords.leftTop.y, fragments[i][j].angleCoords.rightBot.x, fragments[i][j].angleCoords.rightBot.y);
		}
	}

}

uint32_t
LayoutBitmapGenerator::GetFragmentIntersectionCnt(
	Geometry* Geom)
{
	if(nullptr == Geom) { return 0;}

	uint32_t intersectionCnt = 0;
	for(size_t i = 0; i < fragmentMatrix.RowCount(); ++i)
	{
		for(size_t j = 0; j < fragmentMatrix.ColCount(); ++j)
		{
			if(fragmentMatrix[i][j].GeometryWorkspaceIntersection(Geom))
			{
				intersectionCnt++;
			}
		}
	}
	return intersectionCnt;
}

void LayoutBitmapGenerator::DistributeGeometries()
{
	constexpr uint32_t intersectionCntEtalon = 2;
	for(GeometryList::iterator geomIter = geometryList.begin(); geomIter != geometryList.end(); ++geomIter)
	{
		if(intersectionCntEtalon < GetFragmentIntersectionCnt((*geomIter).get())) { continue; } // GetFragmentIntersectionCnt will not return 0

		for(size_t i = 0; i < fragmentMatrix.RowCount(); i++)
		{
			for(size_t j = 0; j < fragmentMatrix.ColCount(); j++)
			{
				if(GeometryWorkspaceIntersection((*geomIter).get()))
				{
					fragmentMatrix[i][j].PushGeometry(*geomIter);
				}
			}
		}
		geomIter = geometryList.erase(geomIter);
		std::prev(geomIter);
	}

	for(LegacyGeometryList::iterator legacyGeomIter = legacyGeometryList.begin(); legacyGeomIter != legacyGeometryList.end(); ++legacyGeomIter)
	{
		if(intersectionCntEtalon < GetFragmentIntersectionCnt(*legacyGeomIter)) { continue; } // GetFragmentIntersectionCnt will not return 0

		for(size_t i = 0; i < fragmentMatrix.RowCount(); i++)
		{
			for(size_t j = 0; j < fragmentMatrix.ColCount(); j++)
			{
				if(GeometryWorkspaceIntersection(*legacyGeomIter))
				{
					fragmentMatrix[i][j].PushGeometry(*legacyGeomIter);
				}
			}
		}
		legacyGeomIter = legacyGeometryList.erase(legacyGeomIter);
		std::prev(legacyGeomIter);
	}
}

void
LayoutBitmapGenerator::InitFragmentsIndicies()
{
	//std::cout << "\nInit fragment indicies\n";
	const size_t i_add = bitmap->GetRowCount() / fragmentMatrix.RowCount();
	const size_t j_add = bitmap->GetColumnCount() / fragmentMatrix.ColCount();
	//std::cout << "i_add = " << i_add << "\nj_add = " << j_add << std::endl;
	for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			fragmentMatrix[i][j].SetIndicies(i * i_add, j * j_add, (i + 1) * i_add - 1, (j + 1) * j_add - 1);
			//printf("fragment[%d][%d]:\nmin = [%d,%d]\tmax = [%d,%d]\n ", i, j, fragments[i][j].boundIndicies.iBegin, fragments[i][j].boundIndicies.jBegin, fragments[i][j].boundIndicies.iEnd, fragments[i][j].boundIndicies.jEnd);
		}

}

//pushing items into fragments

bool LayoutBitmapGenerator::pushRectangle(std::list<Geometry*>::const_iterator rect)
{
	const Coord& leftTop = (*rect)->coords[0];
	const Coord& rightBot = (*rect)->coords[2];
	//printf("type:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	size_t i_begin(0), i_end(fragmentMatrix.RowCount() - 1), j_begin(0), j_end(fragmentMatrix.ColCount() - 1);


	if (int32_t tmp = leftTop.x - preloadedData.bitmapCoords.leftTop.x; tmp > 0)
		j_begin = tmp / dx;
	//added - preloadedData.bitmapCoords.leftTop.x
	if (int32_t tmp = rightBot.x - preloadedData.bitmapCoords.leftTop.x; tmp >= 0 && tmp < preloadedData.bitmapCoords.rightBot.x - preloadedData.bitmapCoords.leftTop.x)
		j_end = tmp / dx;

	if (int32_t tmp = preloadedData.bitmapCoords.leftTop.y - leftTop.y; tmp > 0)
		i_begin = tmp / dy;

	if (int32_t tmp = preloadedData.bitmapCoords.leftTop.y - rightBot.y; tmp > 0 && tmp < preloadedData.bitmapCoords.leftTop.y - preloadedData.bitmapCoords.rightBot.y)
		i_end = tmp / dy;
	//printf("begin = [%d,%d]\t end = [%d,%d]\n", i_begin, j_begin, i_end, j_end);
	
	/*if (i_begin == i_end && j_begin == j_end)
	{
		fragments[i_begin][j_begin].geometries.pushPrealoadedGeometry(*rect);
		return true;
	}*/
	return false;
}
bool LayoutBitmapGenerator::pushRectangle(std::list<std::shared_ptr<Geometry>>::const_iterator rect)
{
	const Coord& leftTop = (*rect)->coords[0];
	const Coord& rightBot = (*rect)->coords[2];
	//printf("type:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	size_t i_begin(0), i_end(fragmentMatrix.RowCount() - 1), j_begin(0), j_end(fragmentMatrix.ColCount() - 1);


	if (int32_t tmp = leftTop.x - preloadedData.bitmapCoords.leftTop.x; tmp > 0)
		j_begin = tmp / dx;
	//added - preloadedData.bitmapCoords.leftTop.x
	if (int32_t tmp = rightBot.x - preloadedData.bitmapCoords.leftTop.x; tmp >= 0 && tmp < preloadedData.bitmapCoords.rightBot.x - preloadedData.bitmapCoords.leftTop.x)
		j_end = tmp / dx;

	if (int32_t tmp = preloadedData.bitmapCoords.leftTop.y - leftTop.y; tmp > 0)
		i_begin = tmp / dy;

	if (int32_t tmp = preloadedData.bitmapCoords.leftTop.y - rightBot.y; tmp > 0 && tmp < preloadedData.bitmapCoords.leftTop.y - preloadedData.bitmapCoords.rightBot.y)
		i_end = tmp / dy;
	//printf("begin = [%d,%d]\t end = [%d,%d]\n", i_begin, j_begin, i_end, j_end);

	if (i_begin == i_end && j_begin == j_end)
	{
		//fragments[i_begin][j_begin].geometries.pushSelfGeneratedGeometry(*rect);
		return true;
	}
	return false;
}


//Initialization vector of elements

inline
bool 
LayoutBitmapGenerator::GeometryWorkspaceIntersection(
	Geometry* item)
{
	if (!item) { return false;}

	//Checking if rectangle faces are located outside workspace
	//printf("type:rectangle\nmin = (%d,%d)\nmax = (%d,%d)\n", item->min.x, item->min.y, item->max.x, item->max.y);
	if (item->min.x > preloadedData.bitmapCoords.rightBot.x || item->max.x < preloadedData.bitmapCoords.leftTop.x || item->max.y<preloadedData.bitmapCoords.rightBot.y || item->min.y>preloadedData.bitmapCoords.leftTop.y)
	{
	//	std::cout << "outside\n";
		return false;
	}
	//std::cout << "inside\n";
	return true;
}

void
LayoutBitmapGenerator::ProcessGeometries( 
	const std::vector<Geometry*>& PreloadedGeometries)
{
	for (size_t i = 0; i < PreloadedGeometries.size(); i++)
	{
		if (GeometryWorkspaceIntersection(PreloadedGeometries[i]))//getting indecies of elements that are in workspace
		{
			switch (PreloadedGeometries[i]->type)
			{
				case GeometryType::rectangle:
				case GeometryType::polygon:
				{
					legacyGeometryList.push_back(PreloadedGeometries[i]);
					break;
				}

				case GeometryType::path:
				{
					GeometryList rectangles = GeometryConverter::PathToRectList(PreloadedGeometries[i]);
					for (auto it : rectangles)//no ref coz geometryList contains shared_ptr
					{
						if(GeometryWorkspaceIntersection(it.get()))
						{
							geometryList.push_back(it);
						}
					}
					break;
				}

				case GeometryType::reference:
				{
					GeometryList rectangles = GeometryConverter::SplitSref(PreloadedGeometries[i]);
					for (auto it : rectangles)//no ref coz geometryList contains shared_ptr
					{
						if(GeometryWorkspaceIntersection(it.get()))
						{
							geometryList.push_back(it);
						}
					}
					break;
				}
			}
		}
	}
}


void
LayoutBitmapGenerator::InitGeometryItems()
{
	for (auto& currLib : preloadedData.data->libraries)
	{
		for (size_t j = 0; j < preloadedData.layers.size(); j++)
		{
			for (auto& currLayer: currLib->layers)
			{
				if (preloadedData.layers[j] == currLayer.layer)
				{
					ProcessGeometries(currLayer.geometries);
					break;
				}
			}
		}
	}
}


//utility methods



void LayoutBitmapGenerator::Reset()
{
	preloadedData.data = nullptr;
	fragmentMatrix.Reset();
}


//Constructors and destructors

LayoutBitmapGenerator::~LayoutBitmapGenerator()
{
	Reset();
}


void LayoutBitmapGenerator::ZondRectangle(Geometry* rect)
{
	const Coord& leftTop = rect->coords[0];
	const Coord& rightBot = rect->coords[2];
	//printf("\ntype:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	//Theoretical indicies
	double i_rect_begin;
	double i_rect_end;
	double j_rect_begin;
	double j_rect_end;
	
	j_rect_begin = (leftTop.x - preloadedData.bitmapCoords.leftTop.x) / dx;

	j_rect_end = (rightBot.x - preloadedData.bitmapCoords.leftTop.x) / dx;

	i_rect_begin = (preloadedData.bitmapCoords.leftTop.y - leftTop.y) / dy;

	i_rect_end = (preloadedData.bitmapCoords.leftTop.y - rightBot.y) / dy;

	//std::cout << "\Indicies before normalization:\n";
	//printf("begin = [%.2f,%.2f]\t end = [%.2f,%.2f]\n", i_rect_begin, j_rect_begin, i_rect_end, j_rect_end);

	//std::cout << "Indicies after normalization\n";
	//Indicies boundIndicies{ 0,bitmap.get_i_size() - 1,0,bitmap.get_j_size() - 1 };
	//Indicies normalIndicies = Indicies::normIndicies(i_rect_begin, i_rect_end, dy, j_rect_begin, j_rect_end, dx, boundIndicies);
	//printf("begin = [%d,%d]\t end = [%d,%d]\n", normalIndicies.iBegin, normalIndicies.jBegin, normalIndicies.iEnd, normalIndicies.jEnd);

	/*for (size_t i = normalIndicies.iBegin; i <= normalIndicies.iEnd && i >= boundIndicies.iBegin && i <= boundIndicies.iEnd; i++)
		for (size_t j = normalIndicies.jBegin; j <= normalIndicies.jEnd && j >= boundIndicies.jBegin && j <= boundIndicies.jEnd; j++)
		{
			try {
				bitmap.set(i, j, 1);
			}
			catch (...)
			{
				std::cout << "err";
			}
		}*/

	//std::cout << std::endl << std::endl;
}

/*Indicies Indicies::normIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx, const Indicies& boundIndicies)
{
	//checking possible situations
	constexpr double mid = 0.5;
	const double iError = eps * dy;
	const double jError = eps * dx;
	Indicies normalIndicies;


	//calculating i part
	if (static_cast<int32_t>(iEnd) - static_cast<int32_t>(iBegin) == 0)
	{

		const double iBeginMantissa = iBegin - trunc(iBegin);
		const double iEndMantissa = iEnd - trunc(iEnd);
		if ((iBeginMantissa >= mid - iError && iBeginMantissa <= mid + iError) || (iEndMantissa >= mid - iError && iEndMantissa <= mid + iError) ||
			(iBeginMantissa <= mid + iError && iEndMantissa >= mid - iError))
		{
			normalIndicies.iBegin = static_cast<size_t>(iBegin);
			normalIndicies.iEnd = static_cast<size_t>(iBegin);
		}
		else {
			normalIndicies.iBegin = 1;
			normalIndicies.iEnd = 0;
		}

	}
	else {
		if (iBegin <= boundIndicies.iBegin)
			normalIndicies.iBegin = boundIndicies.iBegin;
		else {
			const double iBeginMantissa = iBegin - trunc(iBegin);
			if (mid + iError >= iBeginMantissa)
				normalIndicies.iBegin = static_cast<size_t>(iBegin);
			else normalIndicies.iBegin = static_cast<size_t>(iBegin) + 1;
		}
		if (iEnd >= boundIndicies.iEnd)
			normalIndicies.iEnd = boundIndicies.iEnd;
		else
		{
			const double iEndMantissa = iEnd - trunc(iEnd);
			if (mid - iError <= iEndMantissa)
				normalIndicies.iEnd = static_cast<size_t>(round(iEnd));
			else normalIndicies.iEnd = static_cast<size_t>(iEnd) - 1;
		}
	}

	//calculating j part

	if (static_cast<int32_t>(jEnd) - static_cast<int32_t>(jBegin) == 0)
	{

		const double jBeginMantissa = jBegin - trunc(jBegin);
		const double jEndMantissa = jEnd - trunc(jEnd);
		if ((jBeginMantissa >= mid - jError && jBeginMantissa <= mid + jError) || (jEndMantissa >= mid - jError && jEndMantissa <= mid + jError) ||
			(jBeginMantissa <= mid + jError && jEndMantissa >= mid - jError))
		{
			normalIndicies.jBegin = static_cast<size_t>(jBegin);
			normalIndicies.jEnd = static_cast<size_t>(jBegin);
		}
		else {
			normalIndicies.jBegin = 1;
			normalIndicies.jEnd = 0;
		}

	}
	else {
		if (jBegin <= boundIndicies.jBegin)
			normalIndicies.jBegin = boundIndicies.jBegin;
		else {
			const double jBeginMantissa = jBegin - trunc(jBegin);
			if (mid + jError >= jBeginMantissa)
				normalIndicies.jBegin = static_cast<size_t>(jBegin);
			else normalIndicies.jBegin = static_cast<size_t>(jBegin) + 1;
		}
		if (jEnd >= boundIndicies.jEnd)
			normalIndicies.jEnd = boundIndicies.jEnd;
		else
		{
			const double jEndMantissa = jEnd - trunc(jEnd);
			if (mid - jError <= jEndMantissa)
				normalIndicies.jEnd = static_cast<size_t>(round(jEnd));
			else normalIndicies.jEnd = static_cast<size_t>(jEnd) - 1;
		}
	}

	return normalIndicies;
}*/