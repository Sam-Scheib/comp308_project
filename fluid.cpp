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

Fluid::Fluid(int row, int col) {
	rows = row;
	cols = col;

	srand(time(NULL));

	heights = (float**) calloc(row, sizeof(float*));
	for (int i = 0; i < row; i++) {
		heights[i] = (float*) calloc(col, sizeof(float));
		heights[i][0] = (float)rand() / ((float)RAND_MAX/10);
		printf("first value is %f\n", heights[i][0]);
	}
	velocities = (float**) calloc(row, sizeof(float*));
	for (int i = 0; i < row; i++) {
		velocities[i] = (float*) calloc(col, sizeof(float));
		printf("first value is %f\n", velocities[i][0]);
	}

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


//	all i,j do v[i,j] += (u[i-1,j] + u[i+1,j] + u[i,j-1] + u[i,j+1])/4
//	- u[i,j];
//	forall i,j do v[i,j] *= 0.99;
//	forall i,j do u[i,j] += v[i,j];
void Fluid::calculateSurface() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			velocities[i][j] += (heights[i-1][j] + heights[i+1][j] + heights[i][j-1] + heights[i][j+1])/4 - heights[i][j];
			printf("velocity of %d,%d is %f\n", i, j, velocities[i][0]);
			velocities[i][j] *= 0.99;
			heights[i][j] += velocities[i][j];
			printf("height of %d,%d is %f\n", i, j, heights[i][0]);
		}
	}
}

void Fluid::displayFluid() {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.8);
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			glPushMatrix();
				glTranslatef(i-1, 0, j-1);
				glutSolidSphere(0.2, 10, 10);

			glPopMatrix();
		}
	}
	glPopMatrix();
}
