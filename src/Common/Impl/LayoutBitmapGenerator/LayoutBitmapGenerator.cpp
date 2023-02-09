/**
 * @file LayoutBitmapGenerator.cpp
 * @author Mikhail Kotlyarov  (m.kotlyarov@elvis.ru)
 * @brief Definition of methods for layout BitMatrix generation
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdexcept>

#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutConverter.h"

//Public methods

void
LayoutBitmapGenerator::Init(
	LayoutData* Data,
	const std::set <int16_t>& Layers,
	const Coord& Min,
	const Coord& Max)
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
	geometryList.clear();
	legacyGeometryList.clear();

	for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
	{
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			fragmentMatrix[i][j].SetMatrix(bitmap);
		}
	}

	dx = СalcDelta(preloadedData.bitmapCoords.leftTop.x, preloadedData.bitmapCoords.rightBot.x, fragmentMatrix.ColCount());
	dy = СalcDelta(preloadedData.bitmapCoords.leftTop.y, preloadedData.bitmapCoords.rightBot.y, fragmentMatrix.RowCount());

	Indicies boundIndicies;
	boundIndicies.iBegin = boundIndicies.jBegin = 0;
	boundIndicies.iEnd = RowCount - 1;
	boundIndicies.jEnd = ColumnCount - 1;

	const double curr_dx = СalcDelta(preloadedData.bitmapCoords.leftTop.x, preloadedData.bitmapCoords.rightBot.x, bitmap->GetColumnCount());
	const double curr_dy = СalcDelta(preloadedData.bitmapCoords.leftTop.y, preloadedData.bitmapCoords.rightBot.y, bitmap->GetRowCount());
	zond.SetDeltas(curr_dx, curr_dy);
	
	zond.SetBoundIndicies(boundIndicies);
	zond.SetWorkspaceCoord(preloadedData.bitmapCoords);

	InitGeometryItems();
	//InitFragmentsWorkspaces();
	//DistributeGeometries();
	//InitFragmentsIndicies();
	ZondMatrix();

	//bitmap.print();
	//cout << "\n\n\nFilling matrix:\n";
	/*for (size_t i = 0; i < fragmentMatrix.RowCount(); i++)
	{
		for (size_t j = 0; j < fragmentMatrix.ColCount(); j++)
		{
			//cout << "\nFragment[" << i << "][" << j << "]\n";
			fragmentMatrix[i][j].Process();
		}
	}*/
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



void
LayoutBitmapGenerator::ZondMatrix()
{
	dx = СalcDelta(preloadedData.bitmapCoords.leftTop.x, preloadedData.bitmapCoords.rightBot.x,bitmap->GetRowCount());
	dy = СalcDelta(preloadedData.bitmapCoords.leftTop.y, preloadedData.bitmapCoords.rightBot.y, bitmap->GetColumnCount());
	

	for (auto it : geometryList)
	{
		zond.ZondGeometry(it.get(), bitmap);
	}

	for (auto it : legacyGeometryList)
	{
		zond.ZondGeometry(it, bitmap);
	}
}


//Fragment initialization

void
LayoutBitmapGenerator::InitFragmentsWorkspaces()
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


void
LayoutBitmapGenerator::DistributeGeometries()
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

bool
LayoutBitmapGenerator::pushRectangle(
	std::list<Geometry*>::const_iterator rect)
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


bool
LayoutBitmapGenerator::pushRectangle(
	std::list<std::shared_ptr<Geometry>>::const_iterator rect)
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

				//todo:refactoring
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
		for (auto& currLayer: currLib->layers)
		{
			if (preloadedData.layers.count(currLayer.layer))
			{
				ProcessGeometries(currLayer.geometries);
				break;
			}
		}
	}
}


void
LayoutBitmapGenerator::SetLayerArray(
	const std::set <int16_t>& Layers)
{
	preloadedData.layers = Layers;
}


void LayoutBitmapGenerator::Reset()
{
	preloadedData.data = nullptr;
	preloadedData.bitmapCoords.leftTop = preloadedData.bitmapCoords.leftTop = {0, 0};
	preloadedData.layers.clear();

	geometryList.clear();
	legacyGeometryList.clear();
	dx = dy = 0;
	fragmentMatrix.Reset();
}


//Constructors and destructors

LayoutBitmapGenerator::~LayoutBitmapGenerator(){ }