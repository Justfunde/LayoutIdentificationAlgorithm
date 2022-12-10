#include <stdexcept>
#include "Include/LayoutFragment.h"

constexpr double eps = 0.0000000000001;

#pragma warning(disable:4477)
#pragma warning(disable:4244)

inline
double
СalcDelta(
	int32_t N1,
	int32_t N2,
	uint32_t PartCnt)
{
	return fabs(N2 - N1) / PartCnt;
}

bool 
Fragment::GeometryWorkspaceIntersection(
	Geometry* item)
{
	if (!item) { return false;}

	//Checking if rectangle faces are located outside workspace
	//printf("type:rectangle\nmin = (%d,%d)\nmax = (%d,%d)\n", item->min.x, item->min.y, item->max.x, item->max.y);
	if (item->min.x > angleCoords.rightBot.x || item->max.x < angleCoords.leftTop.x || item->max.y<angleCoords.rightBot.y || item->min.y>angleCoords.leftTop.y)
	{
	//	std::cout << "outside\n";
		return false;
	}
	//std::cout << "inside\n";
	return true;
}

Indicies Indicies::normIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx, const Indicies& boundIndicies)
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
}



void 
Fragment::Reset()
{
	angleCoords.setAngleCoords({0,0}, {0,0});
	boundIndicies = {0, 0, 0, 0};
	includedItems.clear();
	layoutMatrix.reset();
	dx = 0;
	dy = 0;
}

void
Fragment::SetMatrix(
	LayoutMatrixPtr Matrix)
{
	if(!Matrix) { throw std::invalid_argument("Matrix is nullptr");}
	layoutMatrix = Matrix;
}


void
Fragment::SetWorkspaceCoords(
	WorkspaceCoords Workspace)
{
	angleCoords = Workspace;
}


void
Fragment::SetIndicies(
	size_t iBegin,
	size_t jBegin,
	size_t iEnd,
	size_t jEnd)
{
	boundIndicies.iBegin = iBegin;
	boundIndicies.iEnd = iEnd;
	boundIndicies.jBegin = jBegin;
	boundIndicies.jEnd = jEnd;
}





void
Fragment::Process()
{
	if (!layoutMatrix) { throw std::runtime_error("Matrix was not initialized"); }

	dx = СalcDelta(angleCoords.leftTop.x, angleCoords.rightBot.x, boundIndicies.jEnd - boundIndicies.jBegin + 1);
	dy = СalcDelta(angleCoords.leftTop.y, angleCoords.rightBot.y, boundIndicies.iEnd - boundIndicies.iBegin + 1);

	//std::cout << "dx = " << dx << "\ndy = " << dy << std::endl;

	for (auto& it : includedItems)
	{
		//std::cout << "\nGeometry[" << i << "]:\n";
		switch (it->type)
		{
		case GeometryType::polygon:
			break;
		case GeometryType::rectangle:
			ZondRectangle(it.get());
			break;
		default:
			break;
		}
	}
}


std::pair <int32_t, int32_t> 
Fragment::NormJPartIndicies(
	double begin, 
	double end,
	double delta)
{
	//checking possible situations
	constexpr double mid = 0.5;
	const double error = eps * delta;

	if (begin < 0 && end > boundIndicies.jEnd- boundIndicies.jBegin +1)
	{
		return std::make_pair(boundIndicies.jBegin, boundIndicies.jEnd);
	}
	else if (begin < 0 && end < boundIndicies.jEnd- boundIndicies.jBegin + 1)
	{
		const double jEndMantissa = end - trunc(end);
		int32_t norm_j_end(0);
		if (mid - error <= jEndMantissa)
			norm_j_end = end;
		else norm_j_end = end - 1;
		return std::make_pair(0, norm_j_end);
	}
	else if (begin > 0 && end > boundIndicies.jEnd- boundIndicies.jBegin + 1)
	{
		const double jBeginMantissa = begin - trunc(begin);
		int32_t norm_j_begin(0);
		if (mid + error >= jBeginMantissa)
			norm_j_begin = begin;
		else norm_j_begin = begin + 1;
		return std::make_pair(norm_j_begin, boundIndicies.jEnd);
	}
	else if (static_cast<int32_t>(end) - static_cast<int32_t>(begin) == 0)
	{ 
		const double jBeginMantissa = begin - trunc(begin);
		const double jEndMantissa = end - trunc(end);
		if ((jBeginMantissa >= mid - eps && jBeginMantissa <= mid + error) || (jEndMantissa >= mid - eps && jEndMantissa <= mid + error))
			return std::make_pair(static_cast<int32_t>(end), static_cast<int32_t>(end));
		if (jBeginMantissa <= mid + error && jEndMantissa >= mid - error)
			return std::make_pair(static_cast<int32_t>(end), static_cast<int32_t>(end));
		else return std::make_pair(1, -1);
	
	}
	else
	{
		int32_t norm_j_begin(0), norm_j_end(0);
		const double jBeginMantissa = begin - trunc(begin);
		const double jEndMantissa = end - trunc(end);

		if (jBeginMantissa >= mid - eps && jBeginMantissa <= mid + error)
			norm_j_begin = static_cast<int32_t>(begin);
		else if (jBeginMantissa > mid + error)
			norm_j_begin = static_cast<int32_t>(begin) + 1;
		else norm_j_begin = static_cast<int32_t>(begin);
		
		if (jEndMantissa >= mid - eps && jEndMantissa <= mid + error)
			norm_j_end = static_cast<int32_t>(end);
		else if (jEndMantissa > mid + error)
			norm_j_end = static_cast<int32_t>(round(end));
		else norm_j_end = static_cast<int32_t>(trunc(end));
		return std::make_pair(norm_j_begin, norm_j_end);
	}
}


std::pair <int32_t, int32_t> 
Fragment::NormIPartIndicies(
	double begin,
	double end,
	double delta)
{
	constexpr double mid = 0.5;
	const double error = eps * delta;

	if (begin < 0 && end > boundIndicies.iEnd- boundIndicies.iBegin + 1)
	{
		return std::make_pair(boundIndicies.iBegin, boundIndicies.iEnd);
	}
	else if (begin < 0 && end < boundIndicies.iEnd- boundIndicies.iBegin + 1)
	{
		const double iEndMantissa = end - trunc(end);
		int32_t norm_i_end(0);
		if (mid - error <= iEndMantissa)
			norm_i_end = end;
		else norm_i_end = end - 1;
		return std::make_pair(0, norm_i_end);
	}
	else if (begin > 0 && end > boundIndicies.iEnd- boundIndicies.iBegin + 1)
	{
		const double iBeginMantissa = begin - trunc(begin);
		int32_t norm_i_begin(0);
		if (mid + error >= iBeginMantissa)
			norm_i_begin = begin;
		else norm_i_begin = begin + 1;
		return std::make_pair(norm_i_begin, boundIndicies.iEnd);
	}
	else if (static_cast<int32_t>(end) - static_cast<int32_t>(begin) == 0)
	{
		const double iBeginMantissa = begin - trunc(begin);
		const double iEndMantissa = end - trunc(end);
		if ((iBeginMantissa >= mid - error && iBeginMantissa <= mid + error) || (iEndMantissa >= mid - error && iEndMantissa <= mid + error))
			return std::make_pair(static_cast<int32_t>(end), static_cast<int32_t>(end));
		if (iBeginMantissa <= mid + error && iEndMantissa >= mid - error)
			return std::make_pair(static_cast<int32_t>(end), static_cast<int32_t>(end));
		else return std::make_pair(1, -1);

	}
	else 
	{
		int32_t norm_i_begin(0), norm_i_end(0);
		const double iBeginMantissa = begin - trunc(begin);
		const double iEndMantissa = end - trunc(end);

		if (iBeginMantissa >= mid - eps && iBeginMantissa <= mid + error)
			norm_i_begin = static_cast<int32_t>(begin);
		else if (iBeginMantissa > mid + error)
			norm_i_begin = static_cast<int32_t>(begin) + 1;
		else norm_i_begin = static_cast<int32_t>(begin);

		if (iEndMantissa >= mid - eps && iEndMantissa <= mid + error)
			norm_i_end = static_cast<int32_t>(end);
		else if (iEndMantissa > mid + error)
			norm_i_end = static_cast<int32_t>(round(end));
		else norm_i_end = static_cast<int32_t>(trunc(end));
		return std::make_pair(norm_i_begin, norm_i_end);

	}
}


void
Fragment::ZondRectangle(
	Geometry* rect)
{
	const Coord& leftTop = rect->coords[0];
	const Coord& rightBot = rect->coords[2];
	//printf("\ntype:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);

	//Theoretical indicies
	double iRectBeginIndex  = static_cast<double> (boundIndicies.iBegin);
	double iRectEndIndex    = static_cast<double> (boundIndicies.iEnd);
	double jRectBegindIndex = static_cast<double> (boundIndicies.jBegin);
	double jRectEndIndex    = static_cast<double> (boundIndicies.jEnd);
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
						layoutMatrix->Set(i, j, 1);
					}
					catch (...)
					{
						//std::cout << "err";
					}
				}
			//layoutMatrix->print();
			return;
		}


		jRectBegindIndex = (leftTop.x - angleCoords.leftTop.x) / dx;

		jRectEndIndex = (rightBot.x - angleCoords.leftTop.x) / dx;

		iRectBeginIndex = (angleCoords.leftTop.y - leftTop.y) / dy;

		iRectEndIndex = (angleCoords.leftTop.y - rightBot.y) / dy;

	//std::cout << "Indicies before normalization:\n";
	printf("begin = [%.2f,%.2f]\t end = [%.2f,%.2f]\n", iRectBeginIndex, jRectBegindIndex, iRectEndIndex, jRectEndIndex);

	//std::cout << "Indicies after normalization\n";
	std::pair<int32_t,int32_t> realI = NormIPartIndicies(iRectBeginIndex,iRectEndIndex, dy);
	std::pair<int32_t, int32_t> realJ = NormJPartIndicies(jRectBegindIndex,jRectEndIndex, dx);
	
	printf("begin = [%d,%d]\t end = [%d,%d]\n", realI.first, realJ.first, realI.second, realJ.second);

	realI.first += boundIndicies.iBegin;
	realI.second += boundIndicies.iBegin;
	realJ.first += boundIndicies.jBegin;
	realJ.second += boundIndicies.jBegin;

 	for (int32_t i = realI.first; i <= realI.second&&i>=boundIndicies.iBegin&&i<= boundIndicies.iEnd; i++)
		for (int32_t j = realJ.first; j <= realJ.second&& j >= boundIndicies.jBegin && j <= boundIndicies.jEnd; j++)
		{
			try {
				layoutMatrix->Set(i, j, 1);
			}
			catch (...)
			{
				//std::cout << "err";
			}
		}
}