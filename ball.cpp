/*
 * ball.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "ball.h"
#include <GL/glut.h>
#include "math.h"
#include "quaternion.h"
float GRAVITY = 1;

G308_Point position;
G308_Point velocity;

ball::ball(float rad, G308_Point position_in, G308_Point velocity_in) {
	// TODO Auto-generated constructor stub
	position = position_in;
	velocity = velocity_in;
	radi = rad;
}

void ball::applyTickMovement() {
	//Gravity
	velocity.y = velocity.y - GRAVITY;

	position = subtract(position, velocity);

}

void ball::render() {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(radi, 4, 4);
	glPopMatrix();
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

	return (dotProduct(velocity, *normal) > 0);
}

bool ball::ballwillcollide(collidable* otherball) {
	float r = radi * 2;
	G308_Point distance = subtract(position, *otherball->getPosition());
	float length = sqrt(
			(distance.x * distance.x) + (distance.y * distance.y)
					+ (distance.z * distance.z));
	if (length < r * r) {
		G308_Point additivevel = subtract(velocity, *otherball->getVector());
		float d = dotProduct(additivevel, distance);
		return d < 0;
	}
	return false;
}

bool ball::collideNormal(G308_Point* normal) {

	float length = sqrt(
			(normal->x * normal->x) + (normal->x * normal->x)
					+ (normal->x * normal->x));
	G308_Point normaldist = scalarMultiply(*normal, (1 / length));
	float dotprod = dotProduct(velocity, normaldist);
	G308_Point normVelocity = scalarMultiply(normaldist, 2);
	normVelocity = scalarMultiply(normVelocity, dotprod);

	velocity = subtract(velocity, normVelocity);

	return true;
}

bool ball::collision(collidable* object) {
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
	object->applyForce(&area);

	return true;
}

ball::~ball() {

	// TODO Auto-generated destructor stub
}

/* namespace std */
