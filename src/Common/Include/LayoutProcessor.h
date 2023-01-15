#ifndef __LAYOUT_PROCESSOR_H__
#define __LAYOUT_PROCESSOR_H__

#include "Include/LayoutMatrix.h"
#include "Include/LayoutFragment.h"


constexpr double eps = 0.0000000000001;

class LayoutBitmapGenerator {
private:

	//Must be preloaded
	LayoutData*				data;
	WorkspaceCoords	    	bitmapCoords;
	std::vector<int16_t>    layers;

	//Calculating inside generator
	LayoutMatrix            bitmap;
	Fragment**				fragments;
	size_t	    			fragmentsSize;
	double  				dx,dy;
	bool                    isCorrect;

	
public:
	//TODO:Do another constuctors and overloaded operators
	LayoutBitmapGenerator() :data(nullptr), fragments(nullptr), fragmentsSize(2), dx(0), dy(0), isCorrect(false) {}
	~LayoutBitmapGenerator();
	
	bool init(LayoutData* data,const Coord& leftTop, const Coord& rightBot, const std::vector <int16_t>& layers);
	bool process(size_t iSize,size_t jSize);
	LayoutMatrix getMatrix() const;

private:
	
	

	//Fragment init
	void InitFragmentsWorkspaces();
	void initFragmentsGeometries();
	void InitFragmentsIndicies();

	//Init vector of elements inside workspace
	std::vector<Geometry*> getLayerItems();
	void cpyGeometries(std::vector <Geometry*>& dest, const std::vector<Geometry*>& source);
	inline bool geometryWorkspaceIntersection(Geometry* item);

	//Pushing items into fragments
	void push_Rectangle(Geometry* rect);
	

	//utility methods
	void allocFragments();
	void reset();

};

namespace LayoutComparator
{
    //void
    //WriteDiffFile()
}

#endif //!__LAYOUT_PROCESSOR_H__