#include "Include/LayoutMatrix.h"
#include "LayoutConverter.h"

#include <memory>
#include <list>


//constexpr double eps = 0.0000000000001;
	struct Indicies {
		size_t iBegin = 0;
		size_t iEnd   = 0;
		size_t jBegin = 0;
		size_t jEnd   = 0;
		static Indicies normIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx, const Indicies& boundIndicies);
	};

class Fragment
{
    private:
	WorkspaceCoords				    angleCoords;
    Indicies                        boundIndicies;
	GeometryList        		    includedItems;
	LegacyGeometryList              legacyGeometryList;

	LayoutMatrixPtr                 layoutMatrix;
	double                          dx, dy;
	
public:
	//TODO: delete another constructors and operators
	Fragment() : dx(0), dy(0) {} 

    void SetMatrix(LayoutMatrixPtr Matrix);

    void SetWorkspaceCoords(WorkspaceCoords Workspace);

    void SetIndicies(size_t iBegin, size_t jBegin, size_t iEnd, size_t jEnd);

   	void PushGeometry(Geometry* Geom) { legacyGeometryList.push_back(Geom);}
	void PushGeometry(GeometryPtr Geom) { includedItems.push_back(Geom); }

	void Process();

	void Reset();
	
	bool GeometryWorkspaceIntersection(Geometry* item);
	
    private:
	// Zonding geometry elements
	void ZondRectangle(Geometry* Rect);
	

	//Working with Geometry indicies
	std::pair <int32_t, int32_t> NormJPartIndicies(double Begin, double End, double Delta);
	std::pair <int32_t, int32_t> NormIPartIndicies(double Begin, double End, double Delta);
	Indicies NormIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx);
};