#include "Include/LayoutMatrix.h"
#include "Include/LayoutBitmapZond.h"
#include "LayoutConverter.h"

#include <memory>
#include <list>


class Fragment
{
    private:
	WorkspaceCoords				    angleCoords;
    Indicies                        boundIndicies;
	GeometryList        		    includedItems;
	LegacyGeometryList              legacyGeometryList;

	LayoutMatrixPtr                 layoutMatrix;
	double                          dx, dy;

	LayoutZond                      layoutZond;
	
public:
	Fragment() : dx(0), dy(0) {} 

    void SetMatrix(LayoutMatrixPtr Matrix);

    void SetWorkspaceCoords(WorkspaceCoords Workspace);

    void SetIndicies(size_t iBegin, size_t jBegin, size_t iEnd, size_t jEnd);

   	void PushGeometry(Geometry* Geom) { legacyGeometryList.push_back(Geom);}

	void PushGeometry(GeometryPtr Geom) { includedItems.push_back(Geom); }

	void Process();

	void Reset();
	
	bool GeometryWorkspaceIntersection(Geometry* item);
	
};