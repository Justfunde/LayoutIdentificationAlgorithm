/**
 * @file LayoutBitmapGenerator.h
 * @author Mikhail Kotlyarov  (m.kotlyarov@elvis.ru)
 * @brief Declaration of class and methods for layout BitMatrix generation
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "Include/LayoutData.hpp"
#include "Include/LayoutConverter.h"
#include "Include/LayoutMatrix.h"
#include "Include/LayoutFragment.h"
#include "Include/LayoutBitmapZond.h"

#include <set>
#include <stdio.h>

/**
 * @class LayoutBitmapGenerator
 * @brief Class for bitmap generation (from layout)
 */
class LayoutBitmapGenerator {
private:
	
	/**
	 * @struct PreloadedData
	 * @brief Structure for handling data from Init()
	 */
	struct PreloadedData
	{
		LayoutData*				data = nullptr; ///< Readen layout file dota
		WorkspaceCoords	    	bitmapCoords; ///< Coordinates to zond
		std::set<int16_t>       layers;///< Layers to zond
	};
	PreloadedData   preloadedData; ///<

	//Calculating inside generator
	LayoutMatrixPtr           bitmap; ///< Matrix pointer
	Vector2D<Fragment>        fragmentMatrix; ///< Fragment Matrix

	GeometryList              geometryList;///< geometry list, filtered from preloadedData.data and converted by layoutConverter 
	LegacyGeometryList        legacyGeometryList;///< list of geometries from preloadedData.data, which is inside preloadedData.bitmapCoords

	LayoutZond                zond; ///< Object for zonding bitmap

	double  				  dx, dy; ///< deltas for fragments and matrix

	
public:
	/**
	 * @brief Construct a new Layout Bitmap Generator object
	 * 
	 */
	LayoutBitmapGenerator(): bitmap(new LayoutMatrix ),fragmentMatrix(1, 1) , dx(0) , dy(0), zond(0.0000000000001) {}
	
	/**
	 * @brief Destroy the Layout Bitmap Generator object
	 * 
	 */
	~LayoutBitmapGenerator();
	
	/**
	 * @brief Generator parameters initialization
	 * 
	 * @param Data Inited data
	 * @param Layers Layers num to zond
	 * @param Min LeftBot coordinates of workspace
	 * @param Max RightTop coorditates of workspace
	 */
	void 
	Init(
		LayoutData* Data,
		const std::set <int16_t>& Layers,
		const Coord& Min,
		const Coord& Max);

	/**
	 * @brief Setter of layer set
	 * 
	 * @param Layers Layers to zond
	 */
	void
	SetLayerArray(
		const std::set <int16_t>& Layers);


	/**
	 * @brief Processing data and generating matrix
	 * 
	 * @param iSize Row count
	 * @param jSize ColumnCoujt
	 * @return true 
	 * @return false 
	 */
	bool
	Process(
		size_t RowCnt,
		size_t ColCnt);


	/**
	 * @brief Getter of processed matrix
	 * 
	 * @return LayoutMatrixPtr 
	 */
	LayoutMatrixPtr
	GetMatrix() const;

private:

	/**
	 * @brief Calculating Geom cross-intersection fragment count
	 * 
	 * @param Geom Geometry item
	 * @return uint32_t 
	 */
	uint32_t
	GetFragmentIntersectionCnt(
		Geometry* Geom);



	/**
	 * @brief Matrix zonding in main thread
	 * 
	 */
	void
	ZondMatrix();

	
	//Fragment initialization

	/**
	 * @brief Matrix workspace initialization
	 * 
	 */
	void
	InitFragmentsWorkspaces();


	/**
	 * @brief Distributing geometries into workspaces
	 * 
	 */
	void
	DistributeGeometries();


	/**
	 * @brief Fragment indicies initialization
	 * 
	 */
	void
	InitFragmentsIndicies();

	/**
	 * @brief Initing geometry lists(distributing geometries from preloadedData.data)
	 * 
	 */
	void
	InitGeometryItems();

	/**
	 * @brief Converting geometries, processing data
	 * 
	 * @param Source Array of Geometries to process
	 */
	void
	ProcessGeometries (
		const std::vector<Geometry*>& Source);


	/**
	 * @brief Checking if Item is inside workspace
	 * 
	 * @param Item Item to check
	 * @return true 
	 * @return false 
	 */
	inline
	bool
	GeometryWorkspaceIntersection(
		Geometry* Item);

	//Pushing items into fragments

	/**
	 * @brief Pushing Rect into fragments
	 * 
	 * @param Rect Rect to push
	 * @return true 
	 * @return false 
	 */
	bool
	pushRectangle(
		std::list<Geometry*>::const_iterator Rect);


	/**
	 * @brief Pushing Rect into fragments
	 * 
	 * @param Rect Rect to push
	 * @return true 
	 * @return false 
	 */
	bool
	pushRectangle(
		std::list<std::shared_ptr<Geometry>>::const_iterator Rect);

	//utility methods
	/**
	 * @brief Reset of generator object
	 * 
	 */
	void Reset();
};


inline double СalcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count);