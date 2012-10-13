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
	float wavespeed, groundHeight, wallheight;
	float** heights;
	float** ground;
	float** velocities;
	G308_Point** normals;
	void calcluateNormals(float**);
	void generateTerrain();
	float getHeightValue(int, int, int, int);

public:
	Fluid(int, int);
	~Fluid();
	void randomiseHeights();
	void calculateSurface();
	void lowerWater();
	void poorWater();
	void displayFluid();
};

