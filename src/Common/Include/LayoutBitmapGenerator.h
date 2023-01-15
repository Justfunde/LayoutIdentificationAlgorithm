#include "Include/LayoutData.hpp"
#include "Include/LayoutConverter.h"
#include "Include/LayoutMatrix.h"
#include "Include/LayoutFragment.h"


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

	//LayoutGeometries        geometries;
	double  				  dx, dy;
	bool                      isCorrect;
	double                    eps = 0.0000000000001;

	
public:
	LayoutBitmapGenerator();
	~LayoutBitmapGenerator();
	
	bool Init(LayoutData* data,const Coord& leftTop, const Coord& rightBot, const std::vector <int16_t>& layers);
	bool Process(size_t iSize,size_t jSize);
	LayoutMatrix getMatrix() const;

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
	void reset();
	friend class Layout_comparator;
};


inline double СalcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count);