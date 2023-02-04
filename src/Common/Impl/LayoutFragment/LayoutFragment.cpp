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

void 
Fragment::Reset()
{
	angleCoords.SetAngleCoords({0,0}, {0,0});
	//boundIndicies = {0, 0, 0, 0};
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
	layoutZond.SetWorkspaceCoord(Workspace);
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
	layoutZond.SetBoundIndicies(boundIndicies);
}


void
Fragment::Process()
{
	if (!layoutMatrix) { throw std::runtime_error("Matrix was not initialized"); }

	dx = СalcDelta(angleCoords.leftTop.x, angleCoords.rightBot.x, boundIndicies.jEnd - boundIndicies.jBegin + 1);
	dy = СalcDelta(angleCoords.leftTop.y, angleCoords.rightBot.y, boundIndicies.iEnd - boundIndicies.iBegin + 1);
	layoutZond.SetDeltas(dx, dy);
	//std::cout << "dx = " << dx << "\ndy = " << dy << std::endl;

	for(auto it : legacyGeometryList)
	{
		layoutZond.ZondGeometry(it, layoutMatrix);
	}

	for(auto it : includedItems)
	{
		layoutZond.ZondGeometry(it, layoutMatrix);
	}
}