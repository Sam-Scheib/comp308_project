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
	velocities = (float**) calloc(row, sizeof(float*));
	float totalV = 0.0;
	for (int i = 0; i < row; i++) {
		heights[i] = (float*) calloc(col, sizeof(float));
		velocities[i] = (float*) calloc(col, sizeof(float));
		for (int j = 0; j < col; j++) {
			heights[i][j] = 0.0; //(float)rand() / ((float)RAND_MAX/2) - 1.0; // Random height values
			//heights[i][j] = (float)i/row + (float)j/col;
			printf("height of %d,%d is %f\n", i, j, heights[i][j]);
			//printf("velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			totalV += velocities[i][j];
		}
	}
	heights[4][4] = 2.0;
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

//	all i,j do v[i,j] += (u[i-1,j] + u[i+1,j] + u[i,j-1] + u[i,j+1])/4
//	- u[i,j];
//	forall i,j do v[i,j] *= 0.99;
//	forall i,j do u[i,j] += v[i,j];
void Fluid::calculateSurface() {
	for (int k = 0; k < rows; k++) {
		heights[k][0] = heights[k][1];
		heights[k][rows-1] = heights[k][rows-2];
	}
	for (int l = 0; l < cols; l++) {
		heights[0][l] = heights[1][l];
		heights[cols-1][l] = heights[cols-2][l];
	}

	float totalV = 0.0;
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			velocities[i][j] += (heights[i-1][j] + heights[i+1][j] + heights[i][j-1] + heights[i][j+1])/4.0 - heights[i][j];
			//printf("velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			totalV += velocities[i][j];
			//velocities[i][j] *= 0.99;
			//printf("dampened velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			heights[i][j] += velocities[i][j];
			//printf("height of %d,%d is %f\n", i, j, heights[i][j]);
		}
	}
	printf("sum of velocity is %f\n", totalV);
}

void Fluid::displayFluid() {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-4.0, -2.0, -30.0);
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_QUADS);
		//bl
		glVertex3f((float)rows-2, 0.0, (float)cols-2);
		//br
		glVertex3f(0.0, 0.0, (float)cols-2);
		//tr
		glVertex3f(0.0, 0.0, 0.0);
		//tl
		glVertex3f((float)rows-2, 0.0, 0.0);

	glEnd();
	glColor4f(0.0, 0.0, 0.8, 0.5);
	for (int i = 1; i < rows-2; i++) {
		for (int j = 1; j < cols-2; j++) {
			glPushMatrix();
			if (velocities[i][j] > 0.0)
				glColor3f(1.0, 0.0, 0.0);
			else if (velocities[i][j] < 0.0)
							glColor3f(0.0, 1.0, 0.0);
				glTranslatef(i, 2.0 + heights[i][j], j);
				glutSolidSphere(0.1, 10, 10);

			glPopMatrix();
		}
	}
	glPopMatrix();
}
