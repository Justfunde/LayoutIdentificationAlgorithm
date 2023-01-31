#ifndef __LAYOUT_BITMAP_ZOND_H__
#define __LAYOUT_BITMAP_ZOND_H__
#include <cstddef>

#include "Include/LayoutData.hpp"
#include "Include/LayoutConverter.h"
#include "Include/LayoutMatrix.h"

struct Indicies 
{
	size_t iBegin;
	size_t iEnd;
	size_t jBegin;
	size_t jEnd;

	Indicies() 
	: iBegin(0)
	, iEnd(0)
	, jBegin(0)
	, jEnd(0) {}
};

class LayoutZond
{
    private:
	Indicies boundIndicies;
	WorkspaceCoords angleCoords;
	double dx, dy;
	double eps;
	

    public:
	LayoutZond(double Eps = 0) 
	: dx(0)
	, dy(0)
	, eps(Eps) {}

	void SetEps(double Eps);

	void SetDeltas(double dx, double dy);

	void SetBoundIndicies(const Indicies& BoundIndicies) { boundIndicies = BoundIndicies; }

	void SetWorkspaceCoord(const WorkspaceCoords& AngleCoords) { angleCoords = AngleCoords; }

	void ZondRectangle(GeometryPtr Rect, LayoutMatrixPtr Matrix);

	void ZondRectangle(LegacyGeometryPtr Rect, LayoutMatrixPtr Matrix);

	void ZondPolygon(GeometryPtr Poly, LayoutMatrixPtr Matrix);

	void ZondPolygon(LegacyGeometryPtr Poly, LayoutMatrixPtr Matrix);

	void ZondGeometry(GeometryPtr Geom, LayoutMatrixPtr Matrix);

	void ZondGeometry(LegacyGeometryPtr Geom, LayoutMatrixPtr Matrix);


	private:
	Indicies NormIndicies(double iBegin, double iEnd, double jBegin, double jEnd);

	using IndexPair = std::pair<size_t, size_t>; //first is minimal

	static IndexPair NormIndicies(double BeginInd, double EndInd, double Delta, double Eps, const IndexPair& BoundIndicies);
};

#endif //!__LAYOUT_BITMAP_ZOND_H__