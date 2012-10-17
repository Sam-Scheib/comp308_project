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
	float wavespeed, slope, wallheight, groundheight, waterheight;
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
	void checkGroundHit(int, int);

public:
	bool glInited, alpha;
	void glInit();
	Fluid(int, int);
	~Fluid();
	void reset();
	void randomiseHeights();
	void randomiseTerrain();
	void flattenTerrain();
	void calculateSurface();
	void lowerWater();
	void poorWater();
	void poorWater(int, int);
	void wave();
	float getTopWavePoint();
	float getBottomWavePoint();
	void displayFluid();
};

