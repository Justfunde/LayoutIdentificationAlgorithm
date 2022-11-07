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
	void initFragmentsWorkspaces();
	void initFragmentsGeometries();
	void initFragmentsIndicies();

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

inline double calcDelta(const int32_t n1, const int32_t n2, const uint32_t split_count);


class Layout_comparator {
private:
	std::pair <LayoutBitmapGenerator*, LayoutBitmapGenerator*> generators;


};