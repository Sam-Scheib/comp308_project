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
	wavespeed = 0.02;
	wallheight = 5.0;
	groundheight = 0.0;
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
			heights[i][j] = 1.8; //(float)rand() / ((float)RAND_MAX/2) - 1.0; // Random height values
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

void Fluid::randomiseHeights() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] = (float)rand() / ((float)RAND_MAX) + 1.0;
			velocities[i][j] = 0.0;
		}
	}
}

void Fluid::flattenTerrain() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			ground[i][j] = groundheight;
		}
	}
	calcluateNormals(ground);
	generateTerrain();
}

void Fluid::randomiseTerrain() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
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
			if (heights[i][j] < ground[i][j]) {
				heights[i][j] = ground[i][j];
				velocities[i][j] = 0.0;
			}
		}
	}
}

void Fluid::poorWater() {
	heights[rows/2][cols/2] += 0.3;
	heights[rows/2+1][cols/2] += 0.2;
	heights[rows/2-1][cols/2] += 0.2;
	heights[rows/2][cols/2+1] += 0.2;
	heights[rows/2][cols/2-1] += 0.2;
}

void Fluid::wave() {
	for (int i = 1; i < cols-1; i++) {
		velocities[1][i] += 0.2;
		velocities[2][i] -= 0.1;
		velocities[3][i] -= 0.1;
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
			if (heights[i][j] < ground[i][j]) {
				heights[i][j] = ground[i][j];
				velocities[i][j] = 0.0;
			}
		}
	}
	calcluateNormals(heights);
	//printf("sum of velocity is %f\n", totalV);
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
	glTranslatef(-4.0, -2.0, -30.0);

	glCallList(terrainList);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //GL_ONE
	glColor4f(0.2f, 0.5f, 0.7f, 0.5f);
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

//			glNormal3f(0.0, 1.0, 0.0);
//			glTexCoord2f(texture1.u/0.2, texture1.v/0.2);
			glNormal3f(normals[i][j].x, normals[i][j].y, normals[i][j].z);
			glVertex3f(i, heights[i][j], j);

//			glNormal3f(0.0, 1.0, 0.0);
//			glTexCoord2f(texture2.u/0.2, texture2.v/0.2);
			glNormal3f(normals[i+1][j].x, normals[i+1][j].y, normals[i+1][j].z);
			glVertex3f(i+1, heights[i+1][j], j);

//			glNormal3f(0.0, 1.0, 0.0);
//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
			glNormal3f(normals[i+1][j+1].x, normals[i+1][j+1].y, normals[i+1][j+1].z);
			glVertex3f(i+1, heights[i+1][j+1], j+1);

//			glNormal3f(0.0, 1.0, 0.0);
//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
			glNormal3f(normals[i][j+1].x, normals[i][j+1].y, normals[i][j+1].z);
			glVertex3f(i, heights[i][j+1], j+1);
		}
	}
	glEnd();
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

		// FRONT WALL
		glColor3f(0.38, 0.42, 0.32);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, 0.0, (float)cols-2);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 0.0, (float)cols-2);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, wallheight, (float)cols-2);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, wallheight, (float)cols-2);

		// LEFT WALL
		//glColor3f(0.9, 0.0, 0.9);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 0.0, (float)cols-2);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, wallheight, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, wallheight, (float)cols-2);

		// BACK WALL
		//glColor3f(0.9, 0.9, 0.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, 0.0, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, wallheight, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, wallheight, 1.0);

		// RIGHT WALL
		//glColor3f(0.9, 0.0, 0.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, 0.0, (float)cols-2);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, 0.0, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, wallheight, 1.0);

		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((float)rows-2, wallheight, (float)cols-2);

	glEnd();
	glEndList();
}
