//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308 
// in ECS, Victoria University of Wellington, 
// without any express or implied warranty. 
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

// My definition 
#include "define.h"
#include "quaternion.h"
#include "imageLoader.h"
#include "fluid.h"

// Global variables
GLuint g_mainWnd;
GLuint g_nWinWidth  = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;
int rotation = 0.0f;

// Spotlight variables
G308_Point spotPosition = {6.0, 12.0, 0.0};
float spotCutoff = 20;
float spotXAngle = 266.0, spotYAngle = 59.0;

// Camera variables
float radius = 300.0;
G308_Point eye = {0.0, 0.0, 1.0};
G308_Point center = {0.0, 0.0, 0.0};
G308_Point top = {0.0, 1.0, 0.0};

bool lookActive = false, panningActive = false,zoomActive = false;
int mouseX, mouseY;

// Variables for our individual modules
Fluid* fluidSim;
bool displayFluid = true, waterFlowing = false;

void G308_keyboardListener(unsigned char, int, int);
void G308_mouseListener(int, int, int, int);
void G308_mouseMovement(int, int);
void panning(float, float);
void zooming(float);
void G308_Display() ;
void G308_Reshape(int w, int h) ;
void G308_SetCamera();
void resetCamera();
void G308_SetLight();
void SpotLight();

int main(int argc, char** argv)
{
	if(argc != 1){
		printf("run without arguments:\n./Assign5\n");
		exit(EXIT_FAILURE);
	}
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(g_nWinWidth, g_nWinHeight);
    g_mainWnd = glutCreateWindow("COMP308 Assignment1");

    glutDisplayFunc(G308_Display);
    glutReshapeFunc(G308_Reshape);
    glutKeyboardFunc(G308_keyboardListener);
	glutMouseFunc(G308_mouseListener);
	glutMotionFunc(G308_mouseMovement);


	// Add individual modules here
	if (displayFluid) {
		fluidSim = new Fluid(30, 30);
	}


	G308_SetLight();
	G308_SetCamera();

	glutIdleFunc(G308_Display);
	glutMainLoop();

    return 0;
}

// Display function
void G308_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glColor3f(0.0f,0.3f,1.0f); /* set object color as blue*/

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("first error is: %s\n", gluErrorString(err));
	}

	// Call indivudal display methods here
	if (displayFluid) {
		if (waterFlowing)
			fluidSim->calculateSurface();
		fluidSim->displayFluid();
	}

//	SpotLight();
	resetCamera();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

G308_Point arcBallPoint(int x, int y) {
	G308_Point pt = {0, 0, 0};
	pt.x = (x-center.x)/radius;
	pt.y = (y-center.y)/radius;
	float r = pt.x*pt.x + pt.y*pt.y;
	if (r > 1.0) {
		float s = 1.0/sqrt(r);
		pt.x = s*pt.x;
		pt.y = s*pt.y;
		pt.z = 0.0;
	}
	else
		pt.z = sqrt(1.0 - r);

//	float xValue = (x-center.x)/radius;
//	float yValue = (y-center.y)/radius;
//	float zValue = sqrt(1-(pow(xValue,2)+pow(yValue,2)));
//	printf("point values are %f %f %f\n", pt.x, pt.y, pt.z);
	return pt;

}

G308_Point normalise(G308_Point temp) {
	float l = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
	temp = {temp.x/l, temp.y/l, temp.z/l};
	return temp;
}

G308_Point getNewPoint(G308_Point p, float* matrix) {
	G308_Point newPoint = {
			p.x * matrix[0] + p.y * matrix[4] + p.z * matrix[8] + matrix[12],
			p.x * matrix[1] + p.y * matrix[5] + p.z * matrix[9] + matrix[13],
			p.x * matrix[2] + p.y * matrix[6] + p.z * matrix[10] + matrix[14]
	};
	//printf("new point values are %f %f %f\n", p.x, p.y, p.z);
	return newPoint;
}

void G308_keyboardListener(unsigned char key, int x, int y) {
	switch(key){
	// step through a display frame
	case 'c':
		fluidSim->calculateSurface();
		glutPostRedisplay();
		break;
	// toggle water flowing naturally
	case 'x':
		waterFlowing = !waterFlowing;
		break;
	case 't':
		rotation += 2;
		//printf("rotation is %d\n", rotation);
		break;
	// randomise the heights of the water
	case 'r':
		fluidSim->randomiseHeights();
		break;
	case 'f':
		fluidSim->lowerWater();
		break;
	// add water to the centre of the pool
	case 'p':
		fluidSim->poorWater();
		break;
	//
	// wasd controls for camera movement
	//
	case 'w':
		zooming(-0.2);
		break;
	case 's':
		zooming(0.2);
		break;
	case 'a':
		panning(0.2, 0);
		break;
	case 'd':
		panning(-0.2, 0);
		break;

	// Old lighting controls
	case 'e':
		spotCutoff += 1.0;
		if (spotCutoff > 90)
			spotCutoff = 90;
		break;
	case 'q':
		spotCutoff -= 1.0;
		if (spotCutoff < 1)
			spotCutoff = 1;
		break;
	case 'i':
		spotXAngle -= 1.0;
		break;
	case 'k':
		spotXAngle += 1.0;
		break;
	case 'j':
		spotYAngle -= 1.0;
		break;
	case 'l':
		spotYAngle += 1.0;
		break;
	}
	glutPostRedisplay();
}

void panning(float xChange, float yChange) {
	G308_Point temp = {center.x-eye.x, center.y-eye.y, center.z-eye.z};
	temp = crossProduct(temp, top);
	temp = normalise(temp);
	eye.x -= temp.x * xChange - top.x * yChange;
	eye.y -= temp.y * xChange - top.y * yChange;
	eye.z -= temp.z * xChange - top.z * yChange;
	center.x -= temp.x * xChange - top.x * yChange;
	center.y -= temp.y * xChange - top.y * yChange;
	center.z -= temp.z * xChange - top.z * yChange;
}

void zooming(float zChange) {
	G308_Point temp = {center.x-eye.x, center.y-eye.y, center.z-eye.z};
	temp = normalise(temp);
	eye.x -= temp.x * zChange;
	eye.y -= temp.y * zChange;
	eye.z -= temp.z * zChange;
	center.x -= temp.x * zChange;
	center.y -= temp.y * zChange;
	center.z -= temp.z * zChange;
}

void G308_mouseListener(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouseX = x;
		mouseY = y;
		lookActive = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		lookActive = false;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		mouseX = x;
		mouseY = y;
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
			zoomActive = true;
		}
		else
			panningActive = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		panningActive = false;
		zoomActive = false;
	}
}

void G308_mouseMovement(int x, int y) {
//	printf("center before %f %f %f\n", center.x, center.y, center.z);
//	printf("eye before %f %f %f\n", eye.x, eye.y, eye.z);
//	printf("top before %f %f %f\n", top.x, top.y, top.z);
	if(lookActive) {
		float matrix[16];
		G308_Point directionVector = {center.x-eye.x, center.y-eye.y, center.z-eye.z};
		G308_Point temp = directionVector;
		temp = crossProduct(temp, top);
		temp = normalise(temp);
		quaternion up = quaternion((mouseY-y)/20.0, temp);
		quaternion side = quaternion((mouseX-x)/20.0, top);
		quaternion q = up * side;
		q.toMatrix(matrix);
		G308_Point newVector = getNewPoint(directionVector, matrix);
		center.x = eye.x + newVector.x;
		center.y = eye.y + newVector.y;
		center.z = eye.z + newVector.z;

		up.toMatrix(matrix);
		newVector = getNewPoint(top, matrix);
		top.x = newVector.x;
		top.y = newVector.y;
		top.z = newVector.z;
//		printf("center after %f %f %f\n", center.x, center.y, center.z);
//		printf("eye after %f %f %f\n", eye.x, eye.y, eye.z);
//		printf("top after %f %f %f\n", top.x, top.y, top.z);
		mouseX = x;
		mouseY = y;
	}
	else if (panningActive) {
//		printf("in panning\n");temp
		float xChange = (mouseX-x)/20.0;
		float yChange = (mouseY-y)/20.0;
		G308_Point temp = {center.x-eye.x, center.y-eye.y, center.z-eye.z};
		temp = crossProduct(temp, top);
		temp = normalise(temp);
		eye.x -= temp.x * xChange - top.x * yChange;
		eye.y -= temp.y * xChange - top.y * yChange;
		eye.z -= temp.z * xChange - top.z * yChange;
		center.x -= temp.x * xChange - top.x * yChange;
		center.y -= temp.y * xChange - top.y * yChange;
		center.z -= temp.z * xChange - top.z * yChange;

		mouseX = x;
		mouseY = y;
//		printf("about to move camera\n");
//		printf("camera moved\n");
	}
	else if (zoomActive) {
		float yChange = (y-mouseY)/10.0;
		G308_Point temp = {center.x-eye.x, center.y-eye.y, center.z-eye.z};
		temp = normalise(temp);
		eye.x -= temp.x * yChange;
		eye.y -= temp.y * yChange;
		eye.z -= temp.z * yChange;
		center.x -= temp.x * yChange;
		center.y -= temp.y * yChange;
		center.z -= temp.z * yChange;
		mouseX = x;
		mouseY = y;
	}
}

// Reshape function
void G308_Reshape(int w, int h)
{
    if (h == 0) h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;
    
    glViewport(0, 0, g_nWinWidth, g_nWinHeight);  
}

// Set Light
void G308_SetLight()
{
	float pointPosition[] = {0.0f, 2.0f, 5.0f, 1.0f};
	float directionalPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
	float ambientPosition[] = {0.0f, 10.0f, 10.0f, 1.0f};
	float direction[] = {0.4f, -1.0f, 0.4f};
	float diffintensity[] = {0.3f, 0.3f, 0.3f, 1.0f};
	float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
	float spec[] = {0.8, 0.8, 0.8, 1.0};

	// point light
	glLightfv(GL_LIGHT0, GL_POSITION, pointPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_SPECULAR,  spec);

	// direction light
	glLightfv(GL_LIGHT1, GL_POSITION, directionalPosition);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT1, GL_SPECULAR,  spec);

	// Ambient light
	glLightfv(GL_LIGHT2, GL_POSITION, ambientPosition);
	glLightfv(GL_LIGHT2, GL_AMBIENT,  ambient);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	SpotLight();
}

void SpotLight() {

	//printf("x angle is %f, y angle is %f\n", spotXAngle, spotYAngle);
	float position[] = {spotPosition.x, spotPosition.y, spotPosition.z, 1.0};
	float direction[]	  = {sin(spotXAngle/180*M_PI) * cos(spotYAngle/180*M_PI),
							 sin(spotXAngle/180*M_PI) * sin(spotYAngle/180*M_PI),
							 cos(spotXAngle/180*M_PI)};
	float diffintensity[] = {0.6f, 0.6f, 0.6f, 1.0f};
	float ambient[]       = {0.0f, 0.0f, 0.0f, 1.0f};

	glPushMatrix();
		GLUquadric* quad = gluNewQuadric();
		glColor3f(0.5, 0.5, 0.5);
		glTranslatef(spotPosition.x, spotPosition.y, spotPosition.z);
		glutSolidSphere(0.3, 10, 10);
//		glPushMatrix();
//			glRotatef(90, 1, 0, 0);
//			glTranslatef(spotPosition.x, spotPosition.y, spotPosition.z);
//			glTranslatef(0.0, 0.4, 0.0);
//			gluCylinder(quad,0.05, 0.05 ,0.8, 10, 10);
//		glPopMatrix();
		glRotatef(-spotXAngle, 1, 0, 0);
		glRotatef(spotYAngle - 90, 0, 1, 0);
		gluCylinder(quad,0.0,spotCutoff/50,0.8,10,10);
		gluDeleteQuadric(quad);
	glPopMatrix();

	glLightfv(GL_LIGHT3, GL_POSITION, position);
	glLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, &spotCutoff);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction);
	glLightfv(GL_LIGHT3, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT3, GL_AMBIENT,  ambient);

	glEnable(GL_LIGHT3);
}

// Set Camera Position
void G308_SetCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


//-------------------------------------------------------------
// [Assignment1]
//
// You may define new position to see the some of the obj files.
// If so, you need to clearly comment it in your ReadMe file 
// as well as the source code.
//-------------------------------------------------------------
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, top.x, top.y, top.z);
	//gluLookAt(0.0, 2.5, 50.0, 0.0, 2.5, 0.0, 0.0, 1.0, 0.0); //bunny and teapot and sphere
	//gluLookAt(0.0, 2.5, 50.0, 0.0, 2.5, 0.0, 0.0, 1.0, 0.0); //bunny and teapot and sphere
	//gluLookAt(0.0, -5.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 1.0); //dragon
	//gluLookAt(10.0, -25.0, 20.0, 0.0, 2.5, 0.0, 0.0, 1.0, 0.0); //looking at bunny feet for errors

}

void resetCamera() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, top.x, top.y, top.z);
}






