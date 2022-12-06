#include "Include/LayoutBitmapGenerator.h"
#include "Include/LayoutConverter.h"

LayoutBitmapGenerator::LayoutBitmapGenerator() 
: data(nullptr)
, fragments(nullptr)
, fragmentsSz(2)
, dx(0)
, dy(0)
, isCorrect(false) 
{
	allocFragments();
}


//Public methods

bool
LayoutBitmapGenerator::Init(
	LayoutData* data,
	const Coord& min,
	const Coord& max,
	const std::vector <int16_t>& layers)
{
	reset();
	const Coord leftTop  = { min.x, max.y };
	const Coord rightBot = { max.x, min.y };

	//Correct parameters checking
	if (!data || layers.empty() || 0 == fragmentsSz) { return false; }

	if (!bitmapCoords.setAngleCoords(leftTop, rightBot)) { return false; }
		
	this->data = data;
	this->layers = layers;
	isCorrect = true;

	return isCorrect;
}


bool
LayoutBitmapGenerator::Process(
	size_t iSize,
	size_t jSize)
{
	if (!isCorrect) { return false; }
		
	if (0 != iSize % fragmentsSz  || 0 != jSize % fragmentsSz) { return false; }
		
	bitmap->Resize(iSize, jSize);

	for (size_t i = 0; i < fragmentsSz; i++)
	{
		for (size_t j = 0; j < fragmentsSz; j++)
		{
			fragments[i][j].SetMatrix(bitmap);
		}
	}
	dx = СalcDelta(bitmapCoords.leftTop.x, bitmapCoords.rightBot.x, fragmentsSz);
	dy = СalcDelta(bitmapCoords.leftTop.y, bitmapCoords.rightBot.y, fragmentsSz);

	InitGeometryItems();
	InitFragmentsWorkspaces();
	DistributeGeometries();
	initFragmentsIndicies();
	firstMatrixInit();

	//bitmap.print();
	//cout << "\n\n\nFilling matrix:\n";
	for (size_t i = 0; i < fragmentsSz; i++)
	{
		for (size_t j = 0; j < fragmentsSz; j++)
		{
			//cout << "\nFragment[" << i << "][" << j << "]\n";
			fragments[i][j].ProcessMatrix();
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





LayoutMatrix LayoutBitmapGenerator::getMatrix() const
{
	//return bitmap->GetMatrix();
	return LayoutMatrix();
}


//First matrix initialization

void LayoutBitmapGenerator::firstMatrixInit()
{
	dx = СalcDelta(bitmapCoords.leftTop.x, bitmapCoords.rightBot.x,bitmap->GetIsize());
	dy = СalcDelta(bitmapCoords.leftTop.y, bitmapCoords.rightBot.y, bitmap->GetJsize());
	/*for (auto it = geometries.dataGeometries.begin(); it != geometries.dataGeometries.end(); it++)
	{
		switch ((*it)->type)
		{
		case GeometryType::polygon:
			break;
		case GeometryType::rectangle:
			zondRectangle(static_cast<Rectangle*>(*it));
			break;
		}
	}
	for (auto it = geometries.selfGeneratedGeometries.begin(); it != geometries.selfGeneratedGeometries.end(); it++)
	{
		zondRectangle(static_cast<Rectangle*>(it->get()));
	}*/
}

//Fragment initialization

void LayoutBitmapGenerator::InitFragmentsWorkspaces()
{
	//std::cout << "\n\n\nFragments angle coordinates : \n";

	for (size_t i = 0; i < fragmentsSz; i++)
		for (size_t j = 0; j < fragmentsSz; j++)
		{
			//fragments[i][j].SetWorkspaceCoords(Coord(bitmapCoords.leftTop.x + j * dx, bitmapCoords.leftTop.y - i * dy), Coord(bitmapCoords.leftTop.x + (j + 1) * dx, bitmapCoords.leftTop.y - (i + 1) * dy));
			//fragments[i][j].p_bitmapCoords = &bitmapCoords;
			//printf("[%d][%d]:\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", i, j, fragments[i][j].angleCoords.leftTop.x, fragments[i][j].angleCoords.leftTop.y, fragments[i][j].angleCoords.rightBot.x, fragments[i][j].angleCoords.rightBot.y);
		}

}

void LayoutBitmapGenerator::DistributeGeometries()
{
	/*
	//std::cout << "\n\n\nFragments geometries:\n";
	//std::cout << "\nPushing items into fragments\n";
	size_t i = 0;
	//std::cout << "Processing data(preloaded) geometries\n";
	for (auto preloaded = geometries.dataGeometries.begin(); preloaded != geometries.dataGeometries.end(); preloaded++,i++)
	{
		//std::cout << "\nGeometry[" << i << "]:\n";
		if ((*preloaded)->type == GeometryType::rectangle)
			if (pushRectangle(preloaded))
				preloaded = geometries.deleteDataGeometries(preloaded);
	}
	i = 0;
	//std::cout << "\nProcessing self generated geometries\n";
	for (auto selfGen = geometries.selfGeneratedGeometries.begin(); selfGen != geometries.selfGeneratedGeometries.end(); selfGen++,i++)
	{
		//std::cout << "\nGeometry[" << i << "]:\n";
		if (pushRectangle(selfGen))
			selfGen = geometries.deletesSelfGenGeometry(selfGen);
	}
	*/
}

void LayoutBitmapGenerator::initFragmentsIndicies()
{
	//std::cout << "\nInit fragment indicies\n";
	const size_t i_add = bitmap->GetIsize() / fragmentsSz;
	const size_t j_add = bitmap->GetJsize() / fragmentsSz;
	//std::cout << "i_add = " << i_add << "\nj_add = " << j_add << std::endl;
	for (size_t i = 0; i < fragmentsSz; i++)
		for (size_t j = 0; j < fragmentsSz; j++)
		{
			fragments[i][j].SetIndicies(i * i_add, j * j_add, (i + 1) * i_add - 1, (j + 1) * j_add - 1);
			//printf("fragment[%d][%d]:\nmin = [%d,%d]\tmax = [%d,%d]\n ", i, j, fragments[i][j].boundIndicies.iBegin, fragments[i][j].boundIndicies.jBegin, fragments[i][j].boundIndicies.iEnd, fragments[i][j].boundIndicies.jEnd);
		}

}

//pushing items into fragments

bool LayoutBitmapGenerator::pushRectangle(std::list<Geometry*>::const_iterator rect)
{
	const Coord& leftTop = (*rect)->coords[0];
	const Coord& rightBot = (*rect)->coords[2];
	//printf("type:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	size_t i_begin(0), i_end(fragmentsSz - 1), j_begin(0), j_end(fragmentsSz - 1);


	if (int32_t tmp = leftTop.x - bitmapCoords.leftTop.x; tmp > 0)
		j_begin = tmp / dx;
	//added - bitmapCoords.leftTop.x
	if (int32_t tmp = rightBot.x - bitmapCoords.leftTop.x; tmp >= 0 && tmp < bitmapCoords.rightBot.x - bitmapCoords.leftTop.x)
		j_end = tmp / dx;

	if (int32_t tmp = bitmapCoords.leftTop.y - leftTop.y; tmp > 0)
		i_begin = tmp / dy;

	if (int32_t tmp = bitmapCoords.leftTop.y - rightBot.y; tmp > 0 && tmp < bitmapCoords.leftTop.y - bitmapCoords.rightBot.y)
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
	size_t i_begin(0), i_end(fragmentsSz - 1), j_begin(0), j_end(fragmentsSz - 1);


	if (int32_t tmp = leftTop.x - bitmapCoords.leftTop.x; tmp > 0)
		j_begin = tmp / dx;
	//added - bitmapCoords.leftTop.x
	if (int32_t tmp = rightBot.x - bitmapCoords.leftTop.x; tmp >= 0 && tmp < bitmapCoords.rightBot.x - bitmapCoords.leftTop.x)
		j_end = tmp / dx;

	if (int32_t tmp = bitmapCoords.leftTop.y - leftTop.y; tmp > 0)
		i_begin = tmp / dy;

	if (int32_t tmp = bitmapCoords.leftTop.y - rightBot.y; tmp > 0 && tmp < bitmapCoords.leftTop.y - bitmapCoords.rightBot.y)
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
	if (item->min.x > bitmapCoords.rightBot.x || item->max.x < bitmapCoords.leftTop.x || item->max.y<bitmapCoords.rightBot.y || item->min.y>bitmapCoords.leftTop.y)
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
	for (auto& currLib : data->libraries)
	{
		for (size_t j = 0; j < layers.size(); j++)
		{
			for (auto& currLayer: currLib->layers)
			{
				if (layers[j] == currLayer.layer)
				{
					ProcessGeometries(currLayer.geometries);
					break;
				}
			}
		}
	}
}






//utility methods


void LayoutBitmapGenerator::allocFragments()
{
	if (fragmentsSz == 0)
		return;
	fragments = new Fragment * [fragmentsSz];
	for (size_t i = 0; i < fragmentsSz; i++)
	{
		fragments[i] = new Fragment[fragmentsSz];
	}
}

void LayoutBitmapGenerator::reset()
{
	isCorrect = false;
	data = nullptr;
	if (fragments != nullptr)
	{
		for (size_t i = 0; i < fragmentsSz; i++)
		{
			for(size_t j = 0; j < fragmentsSz; j++)
			{
				fragments[i][j].Reset();
			}
		}
	}
}


//Constructors and destructors

LayoutBitmapGenerator::~LayoutBitmapGenerator()
{
	reset();
}




void LayoutBitmapGenerator::zondRectangle(Rectangle* rect)
{
	const Coord& leftTop = rect->coords[0];
	const Coord& rightBot = rect->coords[2];
	//printf("\ntype:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	//Theoretical indicies
	double i_rect_begin;
	double i_rect_end;
	double j_rect_begin;
	double j_rect_end;
	
	j_rect_begin = (leftTop.x - bitmapCoords.leftTop.x) / dx;

	j_rect_end = (rightBot.x - bitmapCoords.leftTop.x) / dx;

	i_rect_begin = (bitmapCoords.leftTop.y - leftTop.y) / dy;

	i_rect_end = (bitmapCoords.leftTop.y - rightBot.y) / dy;

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