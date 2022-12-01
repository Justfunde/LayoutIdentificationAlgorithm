#pragma once
#include "Include/LayoutData.hpp"
#include "Include/LayoutMatrix.h"
#include "Include/LayoutFragment.h"
#include <stdio.h>

constexpr double eps = 0.0000000000001;



class LayoutBitmapGenerator {
private:

	//Must be preloaded
	LayoutData*				data;
	WorkspaceCoords	    	bitmapCoords;
	std::vector<int16_t>    layers;

	//Calculating inside generator
	LayoutMatrixPtr           bitmap;
	Fragment**				  fragments;
	size_t	    			  fragmentsSize;
	//LayoutGeometries        geometries;
	double  				  dx,dy;
	bool                      isCorrect;

	
public:
	LayoutBitmapGenerator() : data(nullptr), fragments(nullptr), fragmentsSize(2), dx(0), dy(0), isCorrect(false) {}
	~LayoutBitmapGenerator();
	
	bool init(LayoutData* data,const Coord& leftTop, const Coord& rightBot, const std::vector <int16_t>& layers);
	bool process(size_t iSize,size_t jSize);
	LayoutMatrix getMatrix() const;

private:
	void firstMatrixInit();

	//Zonding
	void zondRectangle(Rectangle* rect);
	
	//Fragment initialization
	void initFragmentsWorkspaces();
	void distributeGeometries();
	void initFragmentsIndicies();

	//Init vector of elements inside workspace
	void getLayerItems();

	void processGeometries (const std::vector<Geometry*>& source);
	inline bool GeometryWorkspaceIntersection(Geometry* item);

	//Pushing items into fragments
	bool pushRectangle(std::list<Geometry*>::const_iterator rect);
	bool pushRectangle(std::list<std::shared_ptr<Geometry>>::const_iterator rect);
	//bool push

	//utility methods
	void allocFragments();
	void reset();
	friend class Layout_comparator;
};


inline double calcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count);


class Layout_comparator {
private:
	std::pair <LayoutBitmapGenerator*, LayoutBitmapGenerator*> generators;


};