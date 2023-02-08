/**
 * @file LayoutBitmapZond.h
 * @author Mikhail Kotlyarov  (m.kotlyarov@elvis.ru)
 * @brief Zond class and methods declaration for matrix processing
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LAYOUT_BITMAP_ZOND_H__
#define __LAYOUT_BITMAP_ZOND_H__
#include <cstddef>

#include "Include/LayoutData.hpp"
#include "Include/LayoutConverter.h"
#include "Include/LayoutMatrix.h"


/**
 * @struct Indicies
 * @brief 2D indicies desctiption
 * 
 */
struct Indicies 
{
	size_t iBegin = 0;
	size_t iEnd   = 0;
	size_t jBegin = 0;
	size_t jEnd   = 0;
};


/**
 * @class LayoutZond
 * @brief BitMatrix zonder
 * 
 */
class LayoutZond
{
    private:
	Indicies boundIndicies; ///< Bound indicies of matrix
	WorkspaceCoords angleCoords; ///< Angle coords of matrix
	double dx, dy; ///< Delta of matrix
	double eps; ///< Accuracy of calculations
	

    public:
	/**
	 * @brief Construct a new Layout Zond object
	 * 
	 * @param Eps Accuracy of calculations
	 */
	LayoutZond(double Eps = 0) 
	: dx(0)
	, dy(0)
	, eps(Eps) {}

	/**
	 * @brief Set accuracy of calculations
	 * 
	 * @param Eps accuracy ( Eps < 0.5, but should be < 0.1)
	 */
	void
	SetEps(
		double Eps);


	/**
	 * @brief Set the Deltas of matrix
	 * 
	 * @param dx Delta for x axis
	 * @param dy Delta for y axis
	 */
	void
	SetDeltas(
		double dx,
		double dy);


	/**
	 * @brief Set the Bound Indicies of matrix
	 * 
	 * @param BoundIndicies 
	 */
	void
	SetBoundIndicies(
		const Indicies& BoundIndicies) { boundIndicies = BoundIndicies; }


	/**
	 * @brief Set the Workspace coordinates
	 * 
	 * @param AngleCoords Coordinates of workspaces
	 */
	void
	SetWorkspaceCoord(
		const WorkspaceCoords& AngleCoords) { angleCoords = AngleCoords; }

	/**
	 * @brief Zonding rectangle
	 * 
	 * @param Rect Rectangle pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondRectangle(
		GeometryPtr Rect,
		LayoutMatrixPtr Matrix);


	/**
	 * @brief Zonding rectangle
	 * 
	 * @param Rect Rectangle pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondRectangle(
		LegacyGeometryPtr Rect,
		LayoutMatrixPtr Matrix);


	/**
	 * @brief Zonding polygon
	 * 
	 * @param Rect Polygon pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondPolygon(
		GeometryPtr Poly,
		LayoutMatrixPtr Matrix);


	/**
	 * @brief Zonding rectangle
	 * 
	 * @param Rect Polygon pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondPolygon(
		LegacyGeometryPtr Poly,
		LayoutMatrixPtr Matrix);


	/**
	 * @brief Zonding any geometry object
	 * 
	 * @param Rect Geometry pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondGeometry(
		GeometryPtr Geom,
		LayoutMatrixPtr Matrix);


	/**
	 * @brief Zonding any geometry object
	 * 
	 * @param Rect Geometry pointer
	 * @param Matrix Matrix pointer
	 */
	void
	ZondGeometry(
		LegacyGeometryPtr Geom,
		LayoutMatrixPtr Matrix);


private:

	/**
	 * @brief Indicies notmalization(i and j)
	 * 
	 * @param iBegin Begin row rect index
	 * @param iEnd End row rect index
	 * @param jBegin Begin column rect index
	 * @param jEnd End column rect index
	 * @return Indicies 
	 */
	Indicies
	NormIndicies(
		double iBegin,
		double iEnd,
		double jBegin,
		double jEnd);

	using IndexPair = std::pair<size_t, size_t>; //first is minimal

	/**
	 * @brief Indicies normalication (only one pair)
	 * 
	 * @param BeginInd Begin index
	 * @param EndInd   End index
	 * @param Delta    Delta for current axis
	 * @param Eps      Accuracy
	 * @param BoundIndicies Bound indicies of matrix
	 * @return IndexPair 
	 */
	static
	IndexPair
	NormIndicies(
		double BeginInd,
		double EndInd,
		double Delta,
		double Eps,
		const IndexPair& BoundIndicies);
};

#endif //!__LAYOUT_BITMAP_ZOND_H__