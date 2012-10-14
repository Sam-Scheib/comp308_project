/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */
#include "define.h"
#include "shader.h"
#include "FBO.h"

class Fluid {

private:
	int rows, cols, count;
	int terrainList;
	float wavespeed, slope, wallheight, groundheight;
	float** heights;
	float** ground;
	float** velocities;
	G308_Point** normals;
	ShaderProgram* shader;
	FBO *from, *to;
	void calcluateNormals(float**);
	void generateTerrain();
	void bowlTerrain();
	float getHeightValue(int, int, int, int);

public:
	bool glInited;
	void glInit();
	Fluid(int, int);
	~Fluid();
	void randomiseHeights();
	void randomiseTerrain();
	void flattenTerrain();
	void calculateSurface();
	void lowerWater();
	void poorWater();
	void wave();
	void displayFluid();
};

