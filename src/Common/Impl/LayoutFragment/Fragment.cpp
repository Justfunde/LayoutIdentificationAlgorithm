#include "Include/Layout_comparator.h"
#pragma warning(disable:4477)
#pragma warning(disable:4244)
Fragment::~Fragment()
{
	p_matrix = nullptr;
}


//public methods

void Fragment::initIndicies(size_t i_begin, size_t j_begin, size_t i_end, size_t  j_end )
{
	this->boundIndicies.i_begin = i_begin;
	this->boundIndicies.i_end = i_end;
	this->boundIndicies.j_begin = j_begin;
	this->boundIndicies.j_end = j_end;
}

void Fragment::fillMatrix()
{
	if (!p_matrix)
		return;
	dx = calcDelta(angleCoords.leftTop.x, angleCoords.rightBot.x, boundIndicies.j_end - boundIndicies.j_begin + 1);
	dy = calcDelta(angleCoords.leftTop.y, angleCoords.rightBot.y, boundIndicies.i_end - boundIndicies.i_begin + 1);
	std::cout << "dx = " << dx << "\ndy = " << dy << std::endl;
	for (size_t i = 0; i < includedItems.size(); i++)
	{
		std::cout << "\nGeometry[" << i << "]:\n";
		switch (includedItems[i]->type)
		{

		case GeometryType::polygon:
			break;
		case GeometryType::path:
			break;
		case GeometryType::rectangle:
			zondRectangle(includedItems[i]);
			break;
		case GeometryType::reference:
			break;
		default:
			break;
		}
	}
}
std::pair <int32_t, int32_t> Fragment::norm_j_indicies(double begin,double end,double delta)
{
	//checking possible situations
	constexpr double mid = 0.5;
	const double error = eps * delta;
	if (begin < 0 && end > boundIndicies.j_end- boundIndicies.j_begin +1)
		return std::make_pair(boundIndicies.j_begin, boundIndicies.j_end);
	else if (begin < 0 && end < boundIndicies.j_end- boundIndicies.j_begin + 1)
	{
		const double jEndMantissa = end - trunc(end);
		int32_t norm_j_end(0);
		if (mid - error <= jEndMantissa)
			norm_j_end = end;
		else norm_j_end = end - 1;
		return std::make_pair(0, norm_j_end);
	}
	else if (begin > 0 && end > boundIndicies.j_end- boundIndicies.j_begin + 1)
	{
		const double jBeginMantissa = begin - trunc(begin);
		int32_t norm_j_begin(0);
		if (mid + error >= jBeginMantissa)
			norm_j_begin = begin;
		else norm_j_begin = begin + 1;
		return std::make_pair(norm_j_begin, boundIndicies.j_end);
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
	else {
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
std::pair <int32_t, int32_t> Fragment::norm_i_indicies(double begin, double end, double delta)
{
	constexpr double mid = 0.5;
	const double error = eps * delta;
	if (begin < 0 && end > boundIndicies.i_end- boundIndicies.i_begin + 1)
		return std::make_pair(boundIndicies.i_begin, boundIndicies.i_end);
	else if (begin < 0 && end < boundIndicies.i_end- boundIndicies.i_begin + 1)
	{
		const double iEndMantissa = end - trunc(end);
		int32_t norm_i_end(0);
		if (mid - error <= iEndMantissa)
			norm_i_end = end;
		else norm_i_end = end - 1;
		return std::make_pair(0, norm_i_end);
	}
	else if (begin > 0 && end > boundIndicies.i_end- boundIndicies.i_begin + 1)
	{
		const double iBeginMantissa = begin - trunc(begin);
		int32_t norm_i_begin(0);
		if (mid + error >= iBeginMantissa)
			norm_i_begin = begin;
		else norm_i_begin = begin + 1;
		return std::make_pair(norm_i_begin, boundIndicies.i_end);
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
	else {
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
//Fragment::Indicies Fragment::normIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx)
//{
//	constexpr double mid = 0.5;
//	const double iError = eps * dy;
//	const double jError = eps * dx;
//
//
//	BitMatrix flag(1,4);
//	enum FlagState :int8_t
//	{
//
//	};
//	Indicies normalIndicies;
//	//I part
//	if (iBegin < boundIndicies.i_begin && iEnd > boundIndicies.i_end - boundIndicies.i_begin + 1)
//	{
//		normalIndicies.i_begin = boundIndicies.i_begin;
//		normalIndicies.i_end   = boundIndicies.i_end;
//	}
//	else if (iBegin < boundIndicies.i_begin && iEnd < boundIndicies.i_end - boundIndicies.i_begin + 1)
//	{
//
//		const double iEndMantissa = end - trunc(end);
//		int32_t norm_i_end(0);
//		if (mid - error <= iEndMantissa)
//			norm_i_end = end;
//		else norm_i_end = end - 1;
//		return std::make_pair(0, norm_i_end);
//	}
//	
//}



void Fragment::zondRectangle(Geometry* rect)
{
	const Coord& leftTop = rect->coords[0];
	const Coord& rightBot = rect->coords[2];
	//printf("\ntype:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	//Theoretical indicies
	double i_rect_begin = static_cast<double> (boundIndicies.i_begin);
	double i_rect_end = static_cast<double> (boundIndicies.i_end);
	double j_rect_begin = static_cast<double> (boundIndicies.j_begin);
	double j_rect_end = static_cast<double> (boundIndicies.j_end);
	//printf("Bound indicies:\nmin = [%d,%d]\tmax = [%d,%d]\n ",boundIndicies.i_begin, boundIndicies.j_begin, boundIndicies.i_end, boundIndicies.j_end);
	//checking if Rectangle lies on the fragment

	if (leftTop.x <= angleCoords.leftTop.x + dx / 2 - dx * eps && rightBot.x >= angleCoords.rightBot.x - dx / 2 + dx * eps)
		if (leftTop.y >= angleCoords.leftTop.y - dy / 2 + dy * eps && rightBot.y <= angleCoords.rightBot.y + dy / 2 - dy * eps)
		{
			std::cout << "\nRectangle lies on the fragment\n";
			for (int32_t i = boundIndicies.i_begin; i <= boundIndicies.i_end; i++)
				for (int32_t j = boundIndicies.j_begin; j <= boundIndicies.j_end; j++)
				{
					try {
						p_matrix->Set(i, j, 1);
					}
					catch (...)
					{
						std::cout << "err";
					}
				}
			//p_matrix->print();
			return;
		}


		j_rect_begin = (leftTop.x - angleCoords.leftTop.x) / dx;

		j_rect_end = (rightBot.x - angleCoords.leftTop.x) / dx;

		i_rect_begin = (angleCoords.leftTop.y - leftTop.y) / dy;

		i_rect_end = (angleCoords.leftTop.y - rightBot.y) / dy;

	std::cout << "Indicies before normalization:\n";
	printf("begin = [%.2f,%.2f]\t end = [%.2f,%.2f]\n", i_rect_begin, j_rect_begin, i_rect_end, j_rect_end);

	std::cout << "Indicies after normalization\n";
	std::pair<int32_t,int32_t> realI = norm_i_indicies(i_rect_begin,i_rect_end, dy);
	std::pair<int32_t, int32_t> realJ = norm_j_indicies(j_rect_begin,j_rect_end, dx);
	
	printf("begin = [%d,%d]\t end = [%d,%d]\n", realI.first, realJ.first, realI.second, realJ.second);

	realI.first += boundIndicies.i_begin;
	realI.second += boundIndicies.i_begin;
	realJ.first += boundIndicies.j_begin;
	realJ.second += boundIndicies.j_begin;

 	for (int32_t i = realI.first; i <= realI.second&&i>=boundIndicies.i_begin&&i<= boundIndicies.i_end; i++)
		for (int32_t j = realJ.first; j <= realJ.second&& j >= boundIndicies.j_begin && j <= boundIndicies.j_end; j++)
		{
			try {
				p_matrix->Set(i, j, 1);
			}
			catch (...)
			{
				std::cout << "err";
			}
		}


	std::cout << std::endl << std::endl;
	//p_matrix->print();
	std::cout << std::endl << std::endl;



}