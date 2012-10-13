/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */
#include "define.h"

class Fluid {

private:
	int rows, cols, count;
	int terrainList;
	float wavespeed, slope, wallheight;
	float** heights;
	float** ground;
	float** velocities;
	G308_Point** normals;
	void calcluateNormals(float**);
	void generateTerrain();
	void bowlTerrain();
	float getHeightValue(int, int, int, int);

public:
	Fluid(int, int);
	~Fluid();
	void randomiseHeights();
	void randomiseTerrain();
	void calculateSurface();
	void lowerWater();
	void poorWater();
	void wave();
	void displayFluid();
};

