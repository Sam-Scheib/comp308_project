/*
 * ball.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "ball.h"
#include <GL/glut.h>

float radi;
G308_Point position;
G308_Point velocity;

ball::ball(float rad, G308_Point position_in, G308_Point velocity_in) {
	// TODO Auto-generated constructor stub
	position = position_in;
	velocity = velocity_in;
	radi = rad;
}
void ball::render(){
	glPushMatrix();
	glTranslatef(position.x,position.y,position.z);
	glutSolidSphere(radi,4,4);
	glPopMatrix();
}

G308_Point* ball::getVector(){

	return &velocity;
}

G308_Point* ball::getPosition(){

	return &position;
}

float* ball::getMass(){
	float ft = 0;
	return &ft;
}
void ball::applyForce(G308_Point* inputForce){
	velocity.x = velocity.x+inputForce->x;
	velocity.y = velocity.y+inputForce->y;
	velocity.z = velocity.z+inputForce->z;

}



bool ball::collision(collidable* object){
	//G308_Point dist = subtract(position,*object->getPosition());
	//Normalize this value




return true;
}

ball::~ball() {

	// TODO Auto-generated destructor stub
}

 /* namespace std */
