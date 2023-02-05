#include "Include/LayoutBitmapZond.h"

#include <cstdint>
#include <exception>

void 
LayoutZond::SetEps(
	double Eps)
{
	if(eps < 0 || eps > 0.5) throw std::invalid_argument("Invalid eps");
	eps = Eps;
}


void 
LayoutZond::SetDeltas(
	double dx,
	double dy)
{
	if(0 > dy || 0 > dx) throw std::invalid_argument("Invalid delta");
	this->dx = dx;
	this->dy = dy;
}


Indicies LayoutZond::NormIndicies(double iBegin, double iEnd, double jBegin, double jEnd)
{
	Indicies ret;
	IndexPair iIndexPair = LayoutZond::NormIndicies(iBegin, iEnd, dy, eps, {boundIndicies.iBegin, boundIndicies.iEnd});
	IndexPair jIndexPair = LayoutZond::NormIndicies(jBegin, jEnd, dx, eps, {boundIndicies.jBegin, boundIndicies.jEnd});

	ret.iBegin = iIndexPair.first;
	ret.iEnd = iIndexPair.second;

	ret.jBegin = jIndexPair.first;
	ret.jEnd = jIndexPair.second;

	return ret;
}

LayoutZond::IndexPair 
LayoutZond::NormIndicies(
	double BeginInd,
	double EndInd,
	double Delta,
	double Eps,
	const IndexPair& BoundIndicies)
{
	if(0 > BeginInd || 0 > EndInd ) { throw std::invalid_argument("Bad indicies");}
	if(EndInd < BeginInd) { throw std::invalid_argument("Left side index is bigger than right side");}
	if(0 >= Delta) { throw std::invalid_argument("Delta must be greater than 0");}

	IndexPair ret = {0, 0};
	constexpr double mid = 0.5;
	const double error = Eps * Delta;
	
	if (static_cast<int32_t>(EndInd) - static_cast<int32_t>(BeginInd) == 0)
	{
		const double jBeginMantissa = BeginInd - std::trunc(BeginInd);
		const double jEndMantissa = EndInd - std::trunc(EndInd);

		if ((jBeginMantissa >= mid - error && jBeginMantissa <= mid + error) || (jEndMantissa >= mid - error && jEndMantissa <= mid + error) ||
			(jBeginMantissa <= mid + error && jEndMantissa >= mid - error))
		{
			ret.first = static_cast<size_t>(BeginInd);
			ret.second = static_cast<size_t>(BeginInd);
		}
		else {
			ret.first = 1;
			ret.second = 0;
		}

	}
	else {
		if (BeginInd <= BoundIndicies.first)
			ret.first = BoundIndicies.first;
		else {
			const double jBeginMantissa = BeginInd - std::trunc(BeginInd);
			if (mid + error >= jBeginMantissa)
				ret.first = static_cast<size_t>(BeginInd);
			else ret.first = static_cast<size_t>(BeginInd) + 1;
		}
		if (EndInd >= BoundIndicies.second)
			ret.second = BoundIndicies.second;
		else
		{
			const double jEndMantissa = EndInd - std::trunc(EndInd);
			if (mid - error <= jEndMantissa)
				ret.second = static_cast<size_t>(round(EndInd));
			else ret.second = static_cast<size_t>(EndInd) - 1;
		}
	}
	return ret;
}


void 
LayoutZond::ZondGeometry(
	GeometryPtr Geom,
	LayoutMatrixPtr Matrix)
{
	ZondGeometry(Geom.get(), Matrix);
}

void
LayoutZond::ZondGeometry(
	LegacyGeometryPtr Geom,
	LayoutMatrixPtr Matrix)
{
	switch (Geom->type)
	{
		case GeometryType::polygon : ZondPolygon(Geom, Matrix); break;
    	case GeometryType::rectangle : ZondRectangle(Geom, Matrix); break;
    	case GeometryType::path : 
    	case GeometryType::reference :
		default : throw std::invalid_argument("This geometry type cannot be zonded!");
	}
}


void
LayoutZond::ZondRectangle(
	LegacyGeometryPtr Rect,
	LayoutMatrixPtr Matrix)
{
	const Coord& leftTop = Rect->coords[0];
	const Coord& rightBot = Rect->coords[2];

	//checking if Rectangle lies on the fragment
	//printf("Bound indicies:\nmin = [%d,%d]\tmax = [%d,%d]\n ",boundIndicies.i_begin, boundIndicies.j_begin, boundIndicies.i_end, boundIndicies.j_end);

	if (leftTop.x <= angleCoords.leftTop.x + dx / 2 - dx * eps && rightBot.x >= angleCoords.rightBot.x - dx / 2 + dx * eps)
		if (leftTop.y >= angleCoords.leftTop.y - dy / 2 + dy * eps && rightBot.y <= angleCoords.rightBot.y + dy / 2 - dy * eps)
		{
			//std::cout << "\nRectangle lies on the fragment\n";
			for (int32_t i = boundIndicies.iBegin; i <= boundIndicies.iEnd; i++)
				for (int32_t j = boundIndicies.jBegin; j <= boundIndicies.jEnd; j++)
				{
					try {
						Matrix->Set(i, j, 1);
					}
					catch (...)
					{
						//std::cout << "err";
					}
				}
			//layoutMatrix->print();
			return;
		}


	double jRectBegindIndex = (leftTop.x - angleCoords.leftTop.x) / dx;

	double jRectEndIndex = (rightBot.x - angleCoords.leftTop.x) / dx;

	double iRectBeginIndex = (angleCoords.leftTop.y - leftTop.y) / dy;

	double iRectEndIndex = (angleCoords.leftTop.y - rightBot.y) / dy;

	//std::cout << "Indicies before normalization:\n";
	//printf("begin = [%.2f,%.2f]\t end = [%.2f,%.2f]\n", iRectBeginIndex, jRectBegindIndex, iRectEndIndex, jRectEndIndex);

	//std::cout << "Indicies after normalization\n";
	Indicies normIndicies = NormIndicies(iRectBeginIndex, iRectEndIndex, jRectBegindIndex, jRectEndIndex);
	
	//printf("begin = [%d,%d]\t end = [%d,%d]\n", normIndicies.iBegin, normIndicies.jBegin, normIndicies.iEnd, normIndicies.iEnd);

	normIndicies.iBegin += boundIndicies.iBegin;
	normIndicies.iEnd += boundIndicies.iBegin;
	normIndicies.jBegin += boundIndicies.jBegin;
	normIndicies.jEnd += boundIndicies.jBegin;

 	for (int32_t i = normIndicies.iBegin; i <= normIndicies.iEnd && i >= boundIndicies.iBegin && i <= boundIndicies.iEnd; i++)
		for (int32_t j = normIndicies.jBegin; j <= normIndicies.jEnd && j >= boundIndicies.jBegin && j <= boundIndicies.jEnd; j++)
		{
			try {
				Matrix->Set(i, j, 1);
			}
			catch (...)
			{
				//std::cout << "err";
			}
		}
}


void
LayoutZond::ZondPolygon(
	LegacyGeometryPtr Poly,
	LayoutMatrixPtr Matrix)
{

}


void
LayoutZond::ZondRectangle(
	GeometryPtr Rect,
	LayoutMatrixPtr Matrix)
{
	ZondRectangle(Rect.get(), Matrix);
}


void
LayoutZond::ZondPolygon(
	GeometryPtr Poly,
	LayoutMatrixPtr Matrix)
{
	ZondPolygon(Poly.get(), Matrix);
}

