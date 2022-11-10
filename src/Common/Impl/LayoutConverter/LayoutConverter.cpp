#include "Include/LayoutConverter.h"

static
GeometryType
GetType(const Geometry* Obj)
{
    return (nullptr == Obj) ? GeometryType::undefined : Obj->type;
}

enum class LineOrientation {
	undefined = 0,
    horizontal,
	vertical
};

enum class PathElemPos
{
	undefuned = 0,
	begin,
	mid,
	end
};

LineOrientation getOrientation(const Coord& first, const Coord& second)
{
	if (first.x == second.x && first.y == second.y)
		return LineOrientation::undefined;
	if (first.x == second.x)
		return LineOrientation::vertical;
	else return LineOrientation::horizontal;
}

void 
addCoordValue(std::shared_ptr<Geometry> geom, const Coord& value)
{
	for (size_t i = 0; i < geom->coords.size(); i++)
	{
		geom->coords[i].x += value.x;
		geom->coords[i].y += value.y;
	}
}

std::pair<Coord, Coord> 
getRectAngles(const Coord& first, const Coord& second, const int32_t addWidth, PathElemPos& pos)
{
	if (pos == PathElemPos::undefuned)
		return std::make_pair(Coord(), Coord());
	Coord leftTop;
	Coord rightBot;

	switch (getOrientation(first, second))
	{
	case LineOrientation::horizontal:
	{
		const int32_t& y = first.y;
		switch (pos)
		{
		case PathElemPos::begin:
		{
			if (second.x > first.x)
			{
				leftTop = { first.x,y + addWidth };
				rightBot = { second.x + addWidth,y - addWidth };
			}
			else
			{
				rightBot = { first.x,y - addWidth };
				leftTop = { second.x - addWidth,y + addWidth };
			}
			break;
		}
		case PathElemPos::mid:
		{
			if (second.x > first.x)
			{
				leftTop = { first.x - addWidth,y + addWidth };
				rightBot = { second.x + addWidth,y - addWidth };
			}
			else
			{
				rightBot = { first.x + addWidth,y - addWidth };
				leftTop = { second.x - addWidth,y + addWidth };
			}
			break;
		}
		case PathElemPos::end:
		{
			if (second.x > first.x)
			{
				leftTop = { first.x - addWidth,y + addWidth };
				rightBot = { second.x,y - addWidth };
			}
			else
			{
				rightBot = { first.x + addWidth,y - addWidth };
				leftTop = { second.x,y + addWidth };
			}
			break;
		}
		default: return std::make_pair(Coord(), Coord());
		}
	}

	case LineOrientation::vertical:
	{
		const int32_t& x = first.x;
		switch (pos)
		{
		case PathElemPos::begin:
		{
			if (first.y > second.y)
			{
				leftTop = { x - addWidth,first.y };
				rightBot = { x + addWidth,second.y - addWidth };
			}
			else
			{
				rightBot = { x + addWidth,first.y };
				leftTop = { x - addWidth,second.y + addWidth };
			}
			break;
		}
		case PathElemPos::mid:
		{
			if (first.y > second.y)
			{
				leftTop = { x - addWidth,first.y + addWidth };
				rightBot = { x + addWidth,second.y - addWidth };
			}
			else
			{
				rightBot = { x + addWidth,first.y - addWidth };
				leftTop = { x - addWidth,second.y + addWidth };
			}
			break;
		}
		case PathElemPos::end:
		{
			if (first.y > second.y)
			{
				leftTop = { x - addWidth,first.y + addWidth };
				rightBot = { x + addWidth,second.y };
			}
			else
			{
				rightBot = { x + addWidth, first.y - addWidth };
				leftTop = { x - addWidth,second.y };
			}
			break;
		}
		default:return std::make_pair(Coord(), Coord());
		}
	}
	default:return std::make_pair(Coord(), Coord());
	}
	return std::make_pair(leftTop, rightBot);
}


GeometryList
GeometryConverter::PathToRect(const Geometry* path)
{
    GeometryList boxes;
    if(GeometryType::path !=  GetType(path)) { return boxes;}

    const Path* pPath = static_cast<const Path*>(path);

    constexpr size_t coordCount = 5;

	const int32_t addWidth = pPath->width / 2;

	PathElemPos pos = PathElemPos::undefuned;
	for (size_t i = 0; i < pPath->coords.size() - 1; i++)
	{
		if (i == 0)
			pos = PathElemPos::begin;
		else if (i == pPath->coords.size() - 2)
			pos = PathElemPos::end;
		else pos = PathElemPos::mid;

		auto tempBox = std::make_shared<Rectangle>();
		tempBox->type = GeometryType::rectangle;
		tempBox->coords.resize(coordCount);


		auto angleCoords = getRectAngles(path->coords[i], path->coords[i + 1], addWidth,pos);//lefttop and rightBot
		
		boxes.push_back(std::move(tempBox));
	}
			
	return boxes;
}

GeometryList
GeometryConverter::SplitSref(
    const Geometry* Sref,
    Coord Min)
{
    GeometryList refArr;
	if (!Sref)
		return refArr;

    const Reference* reference = static_cast<const Reference*>(Sref);

	Min = { Min.x+ reference->pElement->min.x, Min.y+ reference->pElement->min.y } ;

	for (size_t i = 0; i < reference->pElement->geometries.size(); i++)
	{
		switch (reference->pElement->geometries[i]->type)
		{
		case GeometryType::polygon:
		{
			auto tempPoly = std::make_shared<Polygon>();
			*tempPoly = *static_cast<Polygon*>(reference->pElement->geometries[i]);
			addCoordValue(std::static_pointer_cast<Geometry>(tempPoly), Min);
			refArr.push_back(tempPoly);
			break;
		}
		case GeometryType::path:
		{
			std::list <std::shared_ptr<Geometry>> tempPathArr = PathToRect(static_cast<Path*>(reference->pElement->geometries[i]));
			for (auto it = tempPathArr.begin(); it != tempPathArr.end(); it++)
			{
				auto tempRect = std::make_shared<Rectangle>();
				*tempRect = *static_cast<Rectangle*>(reference->pElement->geometries[i]);
				addCoordValue(std::static_pointer_cast<Geometry>(tempRect), Min);
				refArr.push_back(tempRect);
			}

			break;
		}
	
		case GeometryType::rectangle:
		{
			auto tempRect = std::make_shared<Rectangle>();
			*tempRect = *static_cast<Rectangle*>(reference->pElement->geometries[i]);
			addCoordValue(std::static_pointer_cast<Geometry>(tempRect), Min);
			refArr.push_back(tempRect);

			break;
		}
		case GeometryType::reference:
		{
			auto tempRefArr = SplitSref(static_cast<Reference*>(reference->pElement->geometries[i]),Min);
			for (auto it = tempRefArr.begin(); it != tempRefArr.end(); it++)
				refArr.push_back(*it);
			break;
		}
		default:break;
		}
	}
	return refArr; 
}