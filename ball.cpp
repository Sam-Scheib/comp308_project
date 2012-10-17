/*
 * ball.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */
#include "quaternion.h"
#include "ball.h"
#include <GL/glut.h>
#include "math.h"

float GRAVITY = 0.01;

ball::ball(float rad, G308_Point position_in, G308_Point velocity_in) {
	// TODO Auto-generated constructor stub
	position = position_in;
	velocity = velocity_in;
	radi = rad;
}


void ball::applyTickMovement() {
	//Gravity
	//velocity.y = velocity.y + GRAVITY;

	position = subtract(position, velocity);

}

float ball::radius(){
	return this->radi;
}

void ball::render() {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(radi, 12, 12);
	glTranslatef(-position.x, -position.y, -position.z);
	glPopMatrix();
	//printf("whatthefuck %f %f %f \n",position.x,position.y,position.z);
}

G308_Point* ball::getVector() {

	return &velocity;
}

G308_Point* ball::getPosition() {

	return &position;
}

float* ball::getMass() {
	float ft = 0;
	return &ft;
}
void ball::applyForce(G308_Point* inputForce) {
	velocity.x = velocity.x - inputForce->x;
	velocity.y = velocity.y - inputForce->y;
	velocity.z = velocity.z - inputForce->z;

}

bool ball::willcollidenormal(G308_Point* normal) {

	return (dotProduct(velocity, *normal) < 0);
}

bool ball::ballwillcollide(ball* otherball) {
	G308_Point distance = subtract(position, *otherball->getPosition());
	float length = sqrt(
			(distance.x * distance.x) + (distance.y * distance.y)
					+ (distance.z * distance.z));
	if (length < radi + otherball->radius()) {
		G308_Point additivevel = subtract(velocity, *otherball->getVector());
		float d = dotProduct(additivevel, distance);
		return d > 0;
	}
	return false;
}

bool ball::collideNormal(G308_Point* normal) {

	float length = sqrt(
			(normal->x * normal->x) + (normal->y * normal->y)
					+ (normal->z * normal->z));
	G308_Point normaldist = scalarMultiply(*normal, (1 / length));
	float dotprod = dotProduct(velocity, normaldist);
	G308_Point normVelocity = scalarMultiply(normaldist, 2*.8);
	normVelocity = scalarMultiply(normVelocity, 0.8*dotprod);//hack

	velocity = subtract(velocity, normVelocity);

	return true;
}

bool ball::collision(ball* object) {
	G308_Point dist = subtract(position, *object->getPosition());
	float length = sqrt(
			(dist.x * dist.x) + (dist.y * dist.y) + (dist.z * dist.z));
	G308_Point normaldist = scalarMultiply(dist, (1 / length));

	float temp2;
	G308_Point temp;
	G308_Point area;
	temp = scalarMultiply(normaldist, 2);
	temp2 = dotProduct(temp, velocity);
	area = scalarMultiply(temp, temp2);
	velocity = subtract(velocity, area);
	temp2 = dotProduct(temp, *object->getVector());
	area = scalarMultiply(temp, temp2);
	//object->applyForce(&area);
//	velocity.x = -velocity.x;
//	velocity.y = -velocity.y;
//	velocity.z = -velocity.z;

	return true;
}

ball::~ball() {
	;
	// TODO Auto-generated destructor stub
}

/* namespace std */
