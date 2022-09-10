#pragma once
#include "LayoutData.hpp"
#include "LayoutMatrix.h"

constexpr double eps = 0.0000000000001;

//TODO: convert structure to class
struct Fragment
{
	
	struct Indicies {
		size_t i_begin = 0;
		size_t i_end = 0;
		size_t j_begin = 0;
		size_t j_end = 0;
	};
	WorkspaceCoords				    angleCoords;
	std::vector <Geometry*>		    includedItems;

	Indicies                        boundIndicies;
	LayoutMatrix*                   p_matrix;
	double                          dx, dy;
	
public:
	//TODO: delete another constructors and operators
	//Fragment(LayoutMatrix& frag):p_matrix(frag) {}
	Fragment() :p_matrix(nullptr), dx(0), dy(0) {}
	~Fragment();

	void fillMatrix();
	void initIndicies(size_t i_begin, size_t i_end, size_t j_begin, size_t j_end);

	// Zonding geometry elements
	void zondRectangle(Geometry* rect);
	

	//Working with Geometry indicies
	std::pair <int32_t, int32_t> norm_j_indicies(double begin, double end, double delta);
	std::pair <int32_t, int32_t> norm_i_indicies(double begin, double end, double delta);
	Indicies normIndicies(double iBegin, double iEnd, double dy, double jBegin, double jEnd, double dx);
};


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