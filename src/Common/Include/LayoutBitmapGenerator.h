#include "Include/LayoutData.hpp"
#include "Include/LayoutConverter.h"
#include "Include/LayoutMatrix.h"
#include "Include/LayoutFragment.h"
#include "Include/LayoutBitmapZond.h"

#include <stdio.h>

class LayoutBitmapGenerator {
private:
	struct PreloadedData
	{
		LayoutData*				data = nullptr;
		WorkspaceCoords	    	bitmapCoords;
		std::vector<int16_t>    layers;
	};
	PreloadedData   preloadedData;

	//Calculating inside generator
	LayoutMatrixPtr           bitmap;
	Vector2D<Fragment>        fragmentMatrix;  

	GeometryList              geometryList;
	LegacyGeometryList        legacyGeometryList;

	LayoutZond                zond;

	double  				  dx, dy;

	
public:
	LayoutBitmapGenerator(): bitmap(new LayoutMatrix ),fragmentMatrix(1, 1) , dx(0) , dy(0), zond(0.0000000000001) {}
	~LayoutBitmapGenerator();
	
	void 
	Init(
		LayoutData* Data,
		const std::vector <int16_t>& Layers,
		const Coord& Min,
		const Coord& Max);


	bool
	Process(
		size_t iSize,
		size_t jSize);

	LayoutMatrixPtr
	GetMatrix() const;

private:

	uint32_t GetFragmentIntersectionCnt(Geometry* Geom);

	void FirstMatrixInit();

	//Zonding
	void ZondRectangle(Geometry* Rect);
	void ZondPolygon(Geometry* Poly);
	
	//Fragment initialization
	void InitFragmentsWorkspaces();
	void DistributeGeometries();
	void InitFragmentsIndicies();

	//Init vector of elements inside workspace
	void InitGeometryItems();

	void ProcessGeometries (const std::vector<Geometry*>& source);
	inline bool GeometryWorkspaceIntersection(Geometry* item);

	//Pushing items into fragments
	bool pushRectangle(std::list<Geometry*>::const_iterator rect);
	bool pushRectangle(std::list<std::shared_ptr<Geometry>>::const_iterator rect);
	//bool push

	//utility methods
	void Reset();
	friend class Layout_comparator;
};


inline double СalcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count);