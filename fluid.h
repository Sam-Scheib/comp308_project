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
	int rows, cols;
	int terrainList;
	float wavespeed;
	float** heights;
	float** velocities;
	G308_Point** normals;
	void calculateSurface();
	void calcluateNormals();
	void generateTerrain();

public:
	Fluid(int, int);
	~Fluid();
	void randomiseHeights();
	void displayFluid();
};

