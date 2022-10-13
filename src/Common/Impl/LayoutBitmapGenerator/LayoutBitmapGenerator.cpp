#pragma warning(disable:6273)
#pragma warning(disable:4473)
#pragma warning(disable:4477)
#include <iomanip>
#include "Include/Layout_comparator.h"
#include <stdio.h>


//Public methods

bool LayoutBitmapGenerator::init(LayoutData* data, const Coord& leftTop, const Coord& rightBot, const std::vector <int16_t>& layers)
{
	using namespace std;
	wcout << "\nInitialization BitmapGenerator with " << data->fileName << endl;
	reset();


	//Correct parameters checking
	if (!data || layers.empty() || fragmentsSize == 0)
		return false;
	if (!bitmapCoords.setAngleCoords(leftTop, rightBot))
		return false;


	printf("leftTop = (%d,%d)\nrightBot = (%d,%d)\n", bitmapCoords.leftTop.x, bitmapCoords.leftTop.y, bitmapCoords.rightBot.x, bitmapCoords.rightBot.y);
	this->data = data;
	this->layers = layers;


	cout << "Layers:\n";
	for (size_t i = 0; i < layers.size(); i++)
	{
		cout << layers[i] << " ";
	}
	allocFragments();
	isCorrect = true;
	cout << "\nInit result:" << isCorrect << endl;
	return isCorrect;
}

bool LayoutBitmapGenerator::process(size_t iSize, size_t jSize)
{
	using namespace std;
	wcout << "\n\n\n\nProcessing\t" << data->fileName << endl;
	if (!isCorrect)
		return false;
	if (iSize % fragmentsSize != 0 || jSize % fragmentsSize != 0)
		return false;


	bitmap.resize(iSize, jSize);
	cout << "Bitmap size:(" << bitmap.GetIsize() << " " << bitmap.GetJsize() << ")" << endl;
	for (size_t i = 0; i < fragmentsSize; i++)
		for (size_t j = 0; j < fragmentsSize; j++)
			fragments[i][j].p_matrix = &bitmap;

	dx = calcDelta(bitmapCoords.leftTop.x, bitmapCoords.rightBot.x, fragmentsSize);
	dy = calcDelta(bitmapCoords.leftTop.y, bitmapCoords.rightBot.y, fragmentsSize);


	cout << "Fragments Information:\ndx = " << dx << "\ndy = " << dy << endl;


	initFragmentsWorkspaces();
	initFragmentsGeometries();
	initFragmentsIndicies();

	cout << "\n\n\nFilling matrix:\n";
	for (size_t i = 0; i < fragmentsSize; i++)
	{
		for (size_t j = 0; j < fragmentsSize; j++)
		{
			cout << "\nFragment[" << i << "][" << j << "]\n";
			fragments[i][j].fillMatrix();
		}
	}
	cout << endl << endl << endl << endl;
	//bitmap.print();

}

LayoutMatrix LayoutBitmapGenerator::getMatrix() const
{
	return bitmap;
}


//Fragment initialization

void LayoutBitmapGenerator::initFragmentsWorkspaces()
{
	std::cout << "\n\n\nFragments angle coordinates : \n";

	for (size_t i = 0; i < fragmentsSize; i++)
		for (size_t j = 0; j < fragmentsSize; j++)
		{
			fragments[i][j].angleCoords.setAngleCoords(Coord(bitmapCoords.leftTop.x + j * dx, bitmapCoords.leftTop.y - i * dy), Coord(bitmapCoords.leftTop.x + (j + 1) * dx, bitmapCoords.leftTop.y - (i + 1) * dy));
		
			printf("[%d][%d]:\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", i, j, fragments[i][j].angleCoords.leftTop.x, fragments[i][j].angleCoords.leftTop.y, fragments[i][j].angleCoords.rightBot.x, fragments[i][j].angleCoords.rightBot.y);
		}

}

void LayoutBitmapGenerator::initFragmentsGeometries()
{
	std::cout << "\n\n\nFragments geometries:\n";
	const std::vector <Geometry*> bitmapIncluded = getLayerItems();
	std::cout << "\nPushing items into fragments\n";
	for (size_t i = 0; i < bitmapIncluded.size(); i++)
	{
		std::cout << "\nGeometry[" << i << "]:\n";
		switch (bitmapIncluded[i]->type)
		{
		case GeometryType::polygon:
			break;
		case GeometryType::path:
			break;
		case GeometryType::rectangle:
			push_Rectangle(bitmapIncluded[i]);
			break;
		case GeometryType::reference:
			break;
		default:
			break;
		}
	}

}

void LayoutBitmapGenerator::initFragmentsIndicies()
{
	std::cout << "\nInit fragment indicies\n";
	const size_t i_add = bitmap.GetIsize() / fragmentsSize;
	const size_t j_add = bitmap.GetJsize() / fragmentsSize;
	std::cout << "i_add = " << i_add << "\nj_add = " << j_add << std::endl;
	for (size_t i = 0; i < fragmentsSize; i++)
		for (size_t j = 0; j < fragmentsSize; j++)
		{
			fragments[i][j].initIndicies(i * i_add, j * j_add, (i + 1) * i_add - 1, (j + 1) * j_add - 1);
			printf("fragment[%d][%d]:\nmin = [%d,%d]\tmax = [%d,%d]\n ", i, j, fragments[i][j].boundIndicies.i_begin, fragments[i][j].boundIndicies.j_begin, fragments[i][j].boundIndicies.i_end, fragments[i][j].boundIndicies.j_end);
		}

}

//pushing items into fragments

void LayoutBitmapGenerator::push_Rectangle(Geometry* rect)
{
	const Coord& leftTop = rect->coords[0];
	const Coord& rightBot = rect->coords[2];
	printf("type:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
	size_t i_begin(0), i_end(fragmentsSize - 1), j_begin(0), j_end(fragmentsSize - 1);


	if (int32_t tmp = leftTop.x - bitmapCoords.leftTop.x; tmp > 0)
		j_begin = tmp / dx;
	//added - bitmapCoords.leftTop.x
	if (int32_t tmp = rightBot.x - bitmapCoords.leftTop.x; tmp >= 0 && tmp < bitmapCoords.rightBot.x - bitmapCoords.leftTop.x)
		j_end = tmp / dx;

	if (int32_t tmp = bitmapCoords.leftTop.y - leftTop.y; tmp > 0)
		i_begin = tmp / dy;

	if (int32_t tmp = bitmapCoords.leftTop.y - rightBot.y; tmp > 0 && tmp < bitmapCoords.leftTop.y - bitmapCoords.rightBot.y)
		i_end = tmp / dy;
	printf("begin = [%d,%d]\t end = [%d,%d]\n", i_begin, j_begin, i_end, j_end);
	for (size_t i = i_begin; i <= i_end; i++)
		for (size_t j = j_begin; j <= j_end; j++)
			fragments[i][j].includedItems.push_back(rect);

}


//Initialization vector of elements

inline bool LayoutBitmapGenerator::geometryWorkspaceIntersection(Geometry* item)
{
	switch (item->type)
	{
	case GeometryType::polygon:
		break;
	case GeometryType::path:

		break;
	case GeometryType::rectangle:
	{

		const Coord& leftTop = item->coords[0];
		const Coord& rightBot = item->coords[2];
		printf("type:rectangle\nleftTop = (%d,%d)\nrightBot = (%d,%d)\n", leftTop.x, leftTop.y, rightBot.x, rightBot.y);
		//Checking if rectangle faces are located outside workspace
		if (leftTop.y<bitmapCoords.rightBot.y || rightBot.y>bitmapCoords.leftTop.y || leftTop.x > bitmapCoords.rightBot.x || rightBot.x < bitmapCoords.leftTop.x)
		{
			std::cout << "outside\n";
			return false;
		}
		std::cout << "inside\n";
		return true;
	}
	case GeometryType::reference:
		break;
	default:
		return false;
	}
}

void LayoutBitmapGenerator::cpyGeometries(std::vector <Geometry*>& dest, const std::vector<Geometry*>& source)
{
	std::vector<size_t> intersectedIndicies;//arr of Geometry indicies that are inside our coordinateWorkspace
	intersectedIndicies.reserve(source.size() / 3);

	for (size_t i = 0; i < source.size(); i++)
	{
		std::cout << "\nGeometry[" << i << "]:\n";
		if (geometryWorkspaceIntersection(source[i]))//getting indecies of elements that are in workspace
			intersectedIndicies.push_back(i);
	}
	dest.reserve(dest.size() + intersectedIndicies.size());
	for (auto iter = intersectedIndicies.begin(); iter != intersectedIndicies.end(); iter++)
		dest.push_back(source[*iter]);
}

std::vector<Geometry*> LayoutBitmapGenerator::getLayerItems()
{
	std::cout << "\nGetting items from necessary layers\n";
	std::vector <Geometry*> layerGeometries;
	//TODO: ���������� ��� ������� ������������
	for (size_t i = 0; i < data->libraries.size(); i++)
	{
		for (size_t j = 0; j < layers.size(); j++)
		{
			for (size_t k = 0; k < data->libraries[i]->layers.size(); k++)
			{
				if (layers[j] == data->libraries[i]->layers[k].layer)
				{
					std::cout << "\nProcessing layer: " << data->libraries[i]->layers[k].layer;
					cpyGeometries(layerGeometries, data->libraries[i]->layers[k].geometries);
					break;
				}
			}
		}
		
	}
	layerGeometries.shrink_to_fit();
	return layerGeometries;
}






//utility methods

inline double calcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count)
{
	return fabs(n2 - n1) / split_count;
}

void LayoutBitmapGenerator::allocFragments()
{
	if (fragmentsSize == 0)
		return;
	fragments = new Fragment * [fragmentsSize];
	for (size_t i = 0; i < fragmentsSize; i++)
	{
		fragments[i] = new Fragment[fragmentsSize];
	}
}

void LayoutBitmapGenerator::reset()
{
	isCorrect = false;
	data = nullptr;
	if (fragments != nullptr)
	{
		for (size_t i = 0; i < fragmentsSize; i++)
		{
			delete[] fragments[i];
			fragments[i] = nullptr;
		}
		delete fragments;
		fragments = nullptr;
	}
}


//Constructors and destructors

LayoutBitmapGenerator::~LayoutBitmapGenerator()
{
	reset();
}