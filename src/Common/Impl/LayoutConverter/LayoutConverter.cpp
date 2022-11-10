#include "Include/LayoutConverter.h"

using LineCoordinated = std::pair<Coord, Coord>;

enum class LineOrientation {
	undefined = 0,
    horizontal,
	vertical
};


enum class PathElemPos
{
	undefined = 0,
	begin,
	mid,
	end
};

static
int32_t
calcDelta(
   int32_t first,
   int32_t second)
{
   return second - first;
}


static
void 
InFillBox(
   GeometryPtr Box2Fill,
   const Coord& LeftBot,
   const Coord& RightTop,
   int16_t LayerNum)
{
    Coord currCoord;
    int32_t dx = calcDelta(LeftBot.x, RightTop.x);
    int32_t dy = calcDelta(LeftBot.y, RightTop.y);

    //Left top
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y;
    Box2Fill->coords.push_back(currCoord);
    
    //Right top
    Box2Fill->coords.push_back(RightTop);

    //Right bot
    currCoord.x = RightTop.x;
    currCoord.y = RightTop.y - dy;
    Box2Fill->coords.push_back(currCoord);

    //Left bot
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y - dy;
    Box2Fill->coords.push_back(currCoord);

    //Left top
    currCoord.x = RightTop.x - dx;
    currCoord.y = RightTop.y;
    Box2Fill->coords.push_back(currCoord);

    Box2Fill->layer = LayerNum;

    Box2Fill->min = Box2Fill->max = Box2Fill->coords[0];
    for (size_t i = 1; i < Box2Fill->coords.size(); ++i) {
      if (Box2Fill->min.x > Box2Fill->coords[i].x)
        Box2Fill->min.x = Box2Fill->coords[i].x;
      if (Box2Fill->min.y > Box2Fill->coords[i].y)
        Box2Fill->min.y = Box2Fill->coords[i].y;
      if (Box2Fill->max.x < Box2Fill->coords[i].x)
        Box2Fill->max.x = Box2Fill->coords[i].x;
      if (Box2Fill->max.y < Box2Fill->coords[i].y)
        Box2Fill->max.y = Box2Fill->coords[i].y;
    }
}


static
GeometryType
InGetType(const Geometry* Obj)
{
    return (nullptr == Obj) ? GeometryType::undefined : Obj->type;
}

LineOrientation
InGetOrientation(
    const LineCoordinated& LineCoord)
{
	if (LineCoord.first.x == LineCoord.second.x && LineCoord.first.y == LineCoord.second.y) { return LineOrientation::undefined; }
	if (LineCoord.first.x == LineCoord.second.x) { return LineOrientation::vertical; }
	return LineOrientation::horizontal;
}


void 
InAddCoordValue(
    GeometryPtr pGeometry,
    const Coord& AdditValue)
{
    pGeometry->min.x += AdditValue.x;
    pGeometry->min.y += AdditValue.y;
    pGeometry->max.x += AdditValue.x;
    pGeometry->max.y += AdditValue.y;
	for (auto& it : pGeometry->coords)
	{
		it.x += AdditValue.x;
		it.y += AdditValue.y;
	}
}


std::pair<Coord, Coord>
InGetRectAngles(
    const Coord& BeginLine,
    const Coord& EndLine,
    int32_t HalfWidth,
    PathElemPos Pos)
{
	if (Pos == PathElemPos::undefined)
		return std::make_pair(Coord(), Coord());
	Coord leftTop;
	Coord rightBot;

	switch (InGetOrientation({BeginLine, EndLine}))
	{
	case LineOrientation::horizontal:
	{
		const int32_t y = BeginLine.y;
		switch (Pos)
		{
		case PathElemPos::begin:
		{
			if (EndLine.x > BeginLine.x)
			{
				leftTop = { BeginLine.x,y + HalfWidth };
				rightBot = { EndLine.x + HalfWidth,y - HalfWidth };
			}
			else
			{
				rightBot = { BeginLine.x,y - HalfWidth };
				leftTop = { EndLine.x - HalfWidth,y + HalfWidth };
			}
			break;
		}
		case PathElemPos::mid:
		{
			if (EndLine.x > BeginLine.x)
			{
				leftTop = { BeginLine.x - HalfWidth,y + HalfWidth };
				rightBot = { EndLine.x + HalfWidth,y - HalfWidth };
			}
			else
			{
				rightBot = { BeginLine.x + HalfWidth,y - HalfWidth };
				leftTop = { EndLine.x - HalfWidth,y + HalfWidth };
			}
			break;
		}
		case PathElemPos::end:
		{
			if (EndLine.x > BeginLine.x)
			{
				leftTop = { BeginLine.x - HalfWidth,y + HalfWidth };
				rightBot = { EndLine.x,y - HalfWidth };
			}
			else
			{
				rightBot = { BeginLine.x + HalfWidth,y - HalfWidth };
				leftTop = { EndLine.x,y + HalfWidth };
			}
			break;
		}
		default: return std::make_pair(Coord(), Coord());
		}
	}

	case LineOrientation::vertical:
	{
		const int32_t x = BeginLine.x;
		switch (Pos)
		{
		case PathElemPos::begin:
		{
			if (BeginLine.y > EndLine.y)
			{
				leftTop = { x - HalfWidth,BeginLine.y };
				rightBot = { x + HalfWidth,EndLine.y - HalfWidth };
			}
			else
			{
				rightBot = { x + HalfWidth,BeginLine.y };
				leftTop = { x - HalfWidth,EndLine.y + HalfWidth };
			}
			break;
		}
		case PathElemPos::mid:
		{
			if (BeginLine.y > EndLine.y)
			{
				leftTop = { x - HalfWidth,BeginLine.y + HalfWidth };
				rightBot = { x + HalfWidth,EndLine.y - HalfWidth };
			}
			else
			{
				rightBot = { x + HalfWidth,BeginLine.y - HalfWidth };
				leftTop = { x - HalfWidth,EndLine.y + HalfWidth };
			}
			break;
		}
		case PathElemPos::end:
		{
			if (BeginLine.y > EndLine.y)
			{
				leftTop = { x - HalfWidth,BeginLine.y + HalfWidth };
				rightBot = { x + HalfWidth,EndLine.y };
			}
			else
			{
				rightBot = { x + HalfWidth, BeginLine.y - HalfWidth };
				leftTop = { x - HalfWidth,EndLine.y };
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
GeometryConverter::PathToRectList(const Geometry* path)
{
    GeometryList boxes;
    if(GeometryType::path !=  InGetType(path)) { return boxes;}

    const Path* pPath = static_cast<const Path*>(path);
    constexpr size_t coordCount = 5;
	const int32_t halfWidth = pPath->width / 2;

	PathElemPos pos = PathElemPos::undefined;
	for (size_t i = 0; i < pPath->coords.size() - 1; i++)
	{

		if (0 == i) { pos = PathElemPos::begin; }
		else if (pPath->coords.size() - 2 == i  ) { pos = PathElemPos::end; }
		else { pos = PathElemPos::mid; }

        auto tempBox = std::make_shared<Rectangle>(new Rectangle);
		tempBox->coords.resize(coordCount);

		auto angleCoords = InGetRectAngles(path->coords[i], path->coords[i + 1], halfWidth, pos);//lefttop and rightBot

        InFillBox(tempBox, { angleCoords.first.x, angleCoords.second.y}, { angleCoords.second.x, angleCoords.first.y}, pPath->layer);
		boxes.push_back(tempBox);
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

	Min = { Min.x + reference->pElement->min.x, Min.y + reference->pElement->min.y } ;

	for ( const Geometry* it : reference->pElement->geometries)
	{
		switch (InGetType(it))
		{
		case GeometryType::polygon:
		{
			auto tempPoly = std::make_shared<Polygon>( new Polygon);
			*tempPoly = *static_cast<const Polygon*>(it);
			InAddCoordValue(tempPoly, Min);

			refArr.push_back(tempPoly);
			break;
		}

		case GeometryType::path:
		{
			GeometryList tempPathArr = PathToRectList(it);
			for (GeometryPtr  pathPtr : tempPathArr)
			{
				InAddCoordValue(pathPtr, Min);
				refArr.push_back(pathPtr);
			}
			break;
		}
	
		case GeometryType::rectangle:
		{
			auto tempRect = std::make_shared<Rectangle>( new Rectangle);
			*tempRect = *static_cast<const Rectangle*>(it);
			InAddCoordValue(tempRect, Min);

			refArr.push_back(tempRect);
			break;
		}

		case GeometryType::reference:
		{
			auto tempRefArr = SplitSref(it, Min);
			for (GeometryPtr RefIter :  tempRefArr) { refArr.push_back(RefIter); }
			break;
		}
		default:break;
		}
	}
	return refArr; 
}