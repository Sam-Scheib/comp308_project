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
	srand(time(NULL));

	heights = (float**) calloc(row, sizeof(float*));
	for (int i = 0; i < row; i++) {
		heights[i] = (float*) calloc(col, sizeof(float));
		heights[i][0] = (float)rand() / (float)RAND_MAX;
		printf("first value is %f\n", heights[i][0]);
	}

}

Fluid::~Fluid() {
	if (heights != NULL) {
		for (int i = 0; i < rows; i++) {
			free(heights[i]);
		}
		free(heights);
	}
}

void Fluid::displaySurface() {

}
