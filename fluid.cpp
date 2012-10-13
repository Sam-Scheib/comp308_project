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

void Fluid::randomiseHeights() {
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] = (float)rand() / ((float)RAND_MAX/2) - 1.0;
			velocities[i][j] = 0.0;
		}
	}
}

//	all i,j do v[i,j] += (u[i-1,j] + u[i+1,j] + u[i,j-1] + u[i,j+1])/4
//	- u[i,j];
//	forall i,j do v[i,j] *= 0.99;
//	forall i,j do u[i,j] += v[i,j];
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
			velocities[i][j] += ((heights[i-1][j] + heights[i+1][j] + heights[i][j-1] + heights[i][j+1])/4.0 - heights[i][j]) * 0.2;
			//printf("velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			totalV += velocities[i][j];
			velocities[i][j] *= 0.99;
			//printf("dampened velocity of %d,%d is %1.8f\n", i, j, velocities[i][j]);
			//printf("height of %d,%d is %f\n", i, j, heights[i][j]);
		}
	}
	for (int i = 1; i < rows-1; i++) {
		for (int j = 1; j < cols-1; j++) {
			heights[i][j] += velocities[i][j];
		}
	}
	//printf("sum of velocity is %f\n", totalV);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //GL_ONE
	glColor4f(0.2f, 0.5f, 0.7f, 0.5f);
	GLfloat ambient[] = { 0.05, 0.05, 0.03, 1.0 };
	GLfloat diffuse[] = { 0.114, 0.0284, 0.0284, 1.0 };
	GLfloat specular[] = {0.2, 0.2, 0.2, 1.0 };
	GLfloat shininess[] = { 70.0 };
	GLfloat emission[] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);

	glBegin(GL_QUADS);
	for (int i = 1; i < rows-3; i++) {
		for (int j = 1; j < cols-3; j++) {
//			glPushMatrix();
//			if (velocities[i][j] > 0.0)
//				glColor3f(1.0, 0.0, 0.0);
//			else if (velocities[i][j] < 0.0)
//						glColor3f(0.0, 1.0, 0.0);
//			glTranslatef(i, 2.0 + heights[i][j], j);
//			glutSolidSphere(0.1, 10, 10);
//			G308_Point point1 =  m_pVertexArray[m_pTriangles[i].v1];
//			G308_Point point2 =  m_pVertexArray[m_pTriangles[i].v2];
//			G308_Point point3 =  m_pVertexArray[m_pTriangles[i].v3];
//
//			G308_Normal normal1 = m_pNormalArray[m_pTriangles[i].n1];
//			G308_Normal normal2 = m_pNormalArray[m_pTriangles[i].n2];
//			G308_Normal normal3 = m_pNormalArray[m_pTriangles[i].n3];
//
//			G308_UVcoord texture1 = m_pUVArray[m_pTriangles[i].t1];
//			G308_UVcoord texture2 = m_pUVArray[m_pTriangles[i].t2];
//			G308_UVcoord texture3 = m_pUVArray[m_pTriangles[i].t3];

//			glNormal3f(normal1.x, normal1.y,normal1.z);
//			glTexCoord2f(texture1.u/0.2, texture1.v/0.2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(i, 2.0 + heights[i][j], j);

//			glNormal3f(normal2.x, normal2.y,normal2.z);
//			glTexCoord2f(texture2.u/0.2, texture2.v/0.2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(i+1, 2.0 + heights[i+1][j], j);

//			glNormal3f(normal3.x, normal3.y,normal3.z);
//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(i+1, 2.0 + heights[i+1][j+1], j+1);

//			glNormal3f(normal3.x, normal3.y,normal3.z);
//			glTexCoord2f(texture3.u/0.2, texture3.v/0.2);
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(i, 2.0 + heights[i][j+1], j+1);

//			glPopMatrix();
		}
	}
	glEnd();
	glDisable(GL_BLEND);
	glPopMatrix();
}
