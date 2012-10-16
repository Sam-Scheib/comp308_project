/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "fluid.h"
#include "math.h"
#include <time.h>
#include "quaternion.h"

//static char* fragShader = "void main() {\
//		gl_FragColor = vec4(1,0.5,0,1); \
//		}";

Fluid::Fluid(int row, int col) {
	alpha = true;
	wavespeed = 0.1;
	wallheight = 6.0;
	groundheight = 0.0;
	waterheight = (row+col)/((row+col)/8.0);
	slope = -2.0;
	rows = row;
	cols = col;

	srand(time(NULL));

	heights = (float**) calloc(row, sizeof(float*));
	ground = (float**) calloc(row, sizeof(float*));
	velocities = (float**) calloc(row, sizeof(float*));
	normals = (G308_Point**) calloc(row, sizeof(G308_Point*));
	float totalV = 0.0;
	for (int i = 0; i < row; i++) {
		heights[i] = (float*) calloc(col, sizeof(float));
		ground[i] = (float*) calloc(col, sizeof(float));
		velocities[i] = (float*) calloc(col, sizeof(float));
		normals[i] = (G308_Point*) calloc(col, sizeof(G308_Point));
		for (int j = 0; j < col; j++) {
			heights[i][j] = waterheight; //(float)rand() / ((float)RAND_MAX/2) - 1.0; // Random height values
			ground[i][j] = (float)rand() / ((float)RAND_MAX);
			normals[i][j] = {0.0, 0.0, 0.0};
			//heights[i][j] = (float)i/row + (float)j/col;
			//printf("height of %d,%d is %f\n", i, j, heights[i][j]);
			//printf("velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			totalV += velocities[i][j];
		}
	}
	bowlTerrain();
	calcluateNormals(ground);
	generateTerrain();
	calcluateNormals(heights);
	printf("sum of velocity is %f\n", totalV);

}

Fluid::~Fluid() {
	if (heights != NULL) {
		for (int i = 0; i < rows; i++) {
			free(heights[i]);
		}
		free(heights);
	}
	if (velocities != NULL) {
		for (int i = 0; i < rows; i++) {
			free(velocities[i]);
		}
		free(velocities);
	}
}

void Fluid::bowlTerrain() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			if (i < rows/2 && j < cols/2) {
				if (j < i)
					ground[i][j] = j/slope+1;
				else
					ground[i][j] = i/slope+1;
			}
			else if (i < rows/2 && j >= cols/2) {
				if (cols-1-j < i)
					ground[i][j] = (cols-1-j)/slope+1;
				else
					ground[i][j] = i/slope+1;
			}
			else if (i >= rows/2 && j < cols/2) {
				if (j < rows-1-i)
					ground[i][j] = j/slope+1;
				else
					ground[i][j] = (rows-1-i)/slope+1;
			}
			else {
				if (cols-1-j < rows-1-i)
					ground[i][j] = (cols-1-j)/slope+1;
				else
					ground[i][j] = (rows-1-i)/slope+1;
			}
		}
	}
}

void Fluid::reset() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] = waterheight;
			velocities[i][j] = 0.0;
		}
	}
}

void Fluid::randomiseHeights() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] = (float)rand() / ((float)RAND_MAX) -0.5 + waterheight;
			velocities[i][j] = 0.0;
		}
	}
}

void Fluid::flattenTerrain() {
	for (int i = 0; i < rows-1; i++) {
		for (int j = 0; j < cols-1; j++) {
			ground[i][j] = groundheight;
		}
	}
	calcluateNormals(ground);
	generateTerrain();
}

void Fluid::randomiseTerrain() {
	for (int i = 0; i < rows-1; i++) {
		for (int j = 0; j < cols-1; j++) {
			ground[i][j] = (float)rand() / ((float)RAND_MAX);
		}
	}
	calcluateNormals(ground);
	generateTerrain();
}

void Fluid::lowerWater() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] -= 0.1;
			checkGroundHit(i,j);
		}
	}
}

void Fluid::poorWater() {
	int x = (int)(((float)rand() / ((float)RAND_MAX) -0.5)*(rows-4)) + rows/2;
	int y = (int)(((float)rand() / ((float)RAND_MAX) -0.5)*(cols-4)) + cols/2;
//	printf("x is %d, y is %d\n", x, y);
	poorWater(x, y);
}

void Fluid::poorWater(int x, int y) {
	if (x < 3 || x > rows-3 || y < 3 || y > cols-3)
		return;
	float amount = 0.2;
	velocities[x][y] -= amount;
	velocities[x+1][y] -= amount;
	velocities[x-1][y] -= amount;
	velocities[x][y+1] -= amount;
	velocities[x][y-1] -= amount;
	velocities[x+1][y+1] -= amount;
	velocities[x-1][y-1] -= amount;
	velocities[x-1][y+1] -= amount;
	velocities[x+1][y-1] -= amount;

	velocities[x+2][y] += amount/2;
	velocities[x-2][y] += amount/2;
	velocities[x][y+2] += amount/2;
	velocities[x][y-2] += amount/2;
	velocities[x+2][y+1] += amount/2;
	velocities[x-2][y+1] += amount/2;
	velocities[x+1][y+2] += amount/2;
	velocities[x+1][y-2] += amount/2;
	velocities[x+2][y-1] += amount/2;
	velocities[x-2][y-1] += amount/2;
	velocities[x+1][y+2] += amount/2;
	velocities[x+1][y-2] += amount/2;
	velocities[x+2][y+1] += amount/2;
	velocities[x-2][y+1] += amount/2;
	velocities[x-1][y+2] += amount/2;
	velocities[x-1][y-2] += amount/2;
	velocities[x+2][y-1] += amount/2;
	velocities[x-2][y-1] += amount/2;
	velocities[x-1][y+2] += amount/2;
	velocities[x-1][y-2] += amount/2;
}

void Fluid::wave() {
	if (rows < 5) {
		printf("Cannot perform wave with less than 5 rows");
		return;
	}
	for (int i = 0; i < cols-1; i++) {
		if (heights[rows/2][i] > ground[rows/2][i] + 0.2) {
			velocities[rows/2][i] += 0.2;
			velocities[rows/2+1][i] += 0.1;
			velocities[rows/2-1][i] += 0.1;
			velocities[rows/2+2][i] -= 0.1;
			velocities[rows/2-2][i] -= 0.1;
			velocities[rows/2+3][i] -= 0.1;
			velocities[rows/2-3][i] -= 0.1;
		}
	}
}

float Fluid::getTopWavePoint() {
	return heights[rows/2][cols-1];
}

float Fluid::getBottomWavePoint() {
	return heights[rows/2][1];
}

void Fluid::checkGroundHit(int x, int y) {
	if (heights[x][y] < ground[x][y]) {
		float heightDiff =  ground[x][y] - heights[x][y];
//		velocities[x+1][y] -= velocities[x][y]/4;
//		velocities[x-1][y] -= velocities[x][y]/4;
//		velocities[x][y+1] -= velocities[x][y]/4;
//		velocities[x][y-1] -= velocities[x][y]/4;
		heights[x][y] = ground[x][y];
		velocities[x][y] = 0.0;
	}
}

float Fluid::getHeightValue(int x, int y, int selfX, int selfY) {
	if (heights[selfX][selfY] < heights[x][y] && heights[x][y] > ground[x][y]) {
		return heights[x][y];
	}
	else if (heights[selfX][selfY] > heights[x][y]) {
		return heights[x][y];
	}
	return heights[selfX][selfY];
}
void Fluid::calculateSurface() {

	for (int k = 0; k < rows; k++) {
		heights[k][0] = heights[k][1];
		heights[k][cols-1] = heights[k][cols-2];
	}
	for (int l = 0; l < cols; l++) {
		heights[0][l] = heights[1][l];
		heights[rows-1][l] = heights[rows-2][l];
	}

	float totalV = 0.0;
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			count = 0.0;
			float nth = getHeightValue(i-1,j, i, j);
			float sth = getHeightValue(i+1,j, i, j);
			float wst = getHeightValue(i,j-1, i, j);
			float est = getHeightValue(i,j+1, i, j);
			velocities[i][j] += ((nth + sth + wst + est)/4.0 - heights[i][j]) * wavespeed;
			//velocities[i][j] += ((heights[i-1][j] + heights[i+1][j] + heights[i][j-1] + heights[i][j+1])/4.0 - heights[i][j]) * wavespeed;
			//printf("velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			totalV += velocities[i][j];
			velocities[i][j] *= 0.997;
			//printf("dampened velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			//printf("height of %d,%d is %f\n", i, j, heights[i][j]);
		}
	}
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] += velocities[i][j];
			checkGroundHit(i,j);
		}
	}
	calcluateNormals(heights);
//	printf("sum of velocity is %f\n", totalV);
}

void Fluid::calcluateNormals(float** values) {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			G308_Point nth = {0.0, values[i][j-1]-values[i][j], -1.0};
			G308_Point est = {1.0, values[i+1][j]-values[i][j], 0.0};
			G308_Point sth = {0.0, values[i][j+1]-values[i][j], 1.0};
			G308_Point wst = {-1.0, values[i-1][j]-values[i][j], 0.0};
			G308_Point nthNorm = crossProduct(nth, wst);
			G308_Point estNorm = crossProduct(est, nth);
			G308_Point sthNorm = crossProduct(sth, est);
			G308_Point wstNorm = crossProduct(wst, sth);
			normals[i][j].x = (nthNorm.x + estNorm.x + sthNorm.x + wstNorm.x);
			normals[i][j].y = (nthNorm.y + estNorm.y + sthNorm.y + wstNorm.y);
			normals[i][j].z = (nthNorm.z + estNorm.z + sthNorm.z + wstNorm.z);
		}
	}
}

void Fluid::glInit() {
	if (!glInited) {
		from = new FBO();
		from->size = 256;
		from->glInit();
		to = new FBO();
		to->size = 256;
		to->glInit();
		//shader = new ShaderProgram(Shader(fragShader, GL_FRAGMENT_SHADER));
		glInited = true;
	}
}

void Fluid::displayFluid() {

	//shader->enable();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//glTranslatef(-rows/2.0, -wallheight/0.2, -cols/2.0);

	glCallList(terrainList);

	if (alpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //GL_ONE
	}
	glColor4f(0.2f, 0.5f, 0.7f, 0.5);
	GLfloat ambient[] = { 0.05, 0.05, 0.03, 1.0 };
	GLfloat diffuse[] = { 0.114, 0.0284, 0.0284, 1.0 };
	GLfloat specular[] = {0.002, 0.002, 0.002, 0.5 };
	GLfloat shininess[] = { 1.0 };
	GLfloat emission[] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	glBegin(GL_QUADS);
	for (int i = 1; i < rows-2; i++) {
		for (int j = 1; j < cols-2; j++) {
			glNormal3f(normals[i][j].x, normals[i][j].y, normals[i][j].z);
			glVertex3f(i, heights[i][j], j);

			glNormal3f(normals[i+1][j].x, normals[i+1][j].y, normals[i+1][j].z);
			glVertex3f(i+1, heights[i+1][j], j);

			glNormal3f(normals[i+1][j+1].x, normals[i+1][j+1].y, normals[i+1][j+1].z);
			glVertex3f(i+1, heights[i+1][j+1], j+1);

			glNormal3f(normals[i][j+1].x, normals[i][j+1].y, normals[i][j+1].z);
			glVertex3f(i, heights[i][j+1], j+1);
		}
	}
	for (int k = 1; k < rows-2; k++) {

		//draw front water
		glNormal3f(0.0, 1.0, 0.0);

//		glNormal3f(normals[k][1].x, normals[k][1].y, normals[k][1].z);
		glVertex3f(k, heights[k][1], 1);

//		glNormal3f(normals[k][1].x, normals[k][1].y, normals[k][1].z);
		glVertex3f(k, ground[k][1], 1);

//		glNormal3f(normals[k+1][1].x, normals[k+1][1].y, normals[k+1][1].z);
		glVertex3f(k+1, ground[k+1][1], 1);

//		glNormal3f(normals[k+1][1].x, normals[k+1][1].y, normals[k+1][1].z);
		glVertex3f(k+1, heights[k+1][1], 1);

		// Draw back water

//		glNormal3f(normals[k][cols-2].x, normals[k][cols-2].y, normals[k][cols-2].z);
		glVertex3f(k, heights[k][cols-2], cols-2);

//		glNormal3f(normals[k][cols-2].x, normals[k][cols-2].y, normals[k][cols-2].z);
		glVertex3f(k, ground[k][cols-2], cols-2);

//		glNormal3f(normals[k+1][cols-2].x, normals[k+1][cols-2].y, normals[k+1][cols-2].z);
		glVertex3f(k+1, ground[k+1][cols-2], cols-2);

//		glNormal3f(normals[k+1][cols-2].x, normals[k+1][cols-2].y, normals[k+1][cols-2].z);
		glVertex3f(k+1, heights[k+1][cols-2], cols-2);
	}
	for (int l = 1; l < cols-2; l++) {

		//draw front water

//		glNormal3f(normals[1][l].x, normals[1][l].y, normals[1][l].z);
		glVertex3f(1, heights[1][l], l);

//		glNormal3f(normals[1][l].x, normals[1][l].y, normals[1][l].z);
		glVertex3f(1, ground[1][l], l);

//		glNormal3f(normals[1][l+1].x, normals[1][l+1].y, normals[1][l+1].z);
		glVertex3f(1, ground[1][l+1], l+1);

//		glNormal3f(normals[1][l+1].x, normals[1][l+1].y, normals[1][l+1].z);
		glVertex3f(1, heights[1][l+1], l+1);

		// Draw back water

//		glNormal3f(normals[rows-2][l].x, normals[rows-2][l].y, normals[rows-2][l].z);
		glVertex3f(rows-2, heights[rows-2][l], l);

//		glNormal3f(normals[rows-2][l].x, normals[rows-2][l].y, normals[rows-2][l].z);
		glVertex3f(rows-2, ground[rows-2][l], l);

//		glNormal3f(normals[rows-2][l+1].x, normals[rows-2][l+1].y, normals[rows-2][l+1].z);
		glVertex3f(rows-2, ground[rows-2][l+1], l+1);

//		glNormal3f(normals[rows-2][l+1].x, normals[rows-2][l+1].y, normals[rows-2][l+1].z);
		glVertex3f(rows-2, heights[rows-2][l+1], l+1);
	}
	glEnd();
	if (alpha)
		glDisable(GL_BLEND);
	glPopMatrix();
}

void Fluid::generateTerrain() {
	if (terrainList != 0)
		glDeleteLists(terrainList, 1);

	terrainList = glGenLists(1);

	glShadeModel(GL_SMOOTH);
	glNewList(terrainList, GL_COMPILE);

	glBegin(GL_QUADS);

		// GROUND
		glColor3f(0.858824, 0.858824, 0.439216);

//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, 1.0);

		for (int i = 1; i < rows-2; i++) {
			for (int j = 1; j < cols-2; j++) {

	//			glNormal3f(0.0, 1.0, 0.0);
	//			glTexCoord2f(texture1.u/0.2, texture1.v/0.2);
				glNormal3f(normals[i][j].x, normals[i][j].y, normals[i][j].z);
				glVertex3f(i, ground[i][j]+0.01, j);

	//			glNormal3f(0.0, 1.0, 0.0);
	//			glTexCoord2f(texture2.u/0.2, texture2.v/0.2);
				glNormal3f(normals[i+1][j].x, normals[i+1][j].y, normals[i+1][j].z);
				glVertex3f(i+1, ground[i+1][j]+0.01, j);

	//			glNormal3f(0.0, 1.0, 0.0);
	//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
				glNormal3f(normals[i+1][j+1].x, normals[i+1][j+1].y, normals[i+1][j+1].z);
				glVertex3f(i+1, ground[i+1][j+1]+0.01, j+1);

	//			glNormal3f(0.0, 1.0, 0.0);
	//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
				glNormal3f(normals[i][j+1].x, normals[i][j+1].y, normals[i][j+1].z);
				glVertex3f(i, ground[i][j+1]+0.01, j+1);
			}
		}

//		// FRONT WALL
//		glColor3f(0.48, 0.52, 0.42);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, wallheight, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, wallheight, (float)cols-2);
//
//		// LEFT WALL
//		//glColor3f(0.9, 0.0, 0.9);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, wallheight, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, wallheight, (float)cols-2);
//
//		// BACK WALL
//		//glColor3f(0.9, 0.9, 0.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, 0.0, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, wallheight, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f(1.0, wallheight, 1.0);
//
//		// RIGHT WALL
//		//glColor3f(0.9, 0.0, 0.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, (float)cols-2);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, 0.0, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, wallheight, 1.0);
//
//		glNormal3f(0.0, 1.0, 0.0);
//		glVertex3f((float)rows-2, wallheight, (float)cols-2);

	glEnd();
	glEndList();
}
