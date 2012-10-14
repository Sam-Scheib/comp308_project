/*
 * ball.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "Collidable.h"
class ball: collidable{
public:
	float radi;
	ball(float radius, G308_Point initialposition, G308_Point initialvelocity);
	void render();
	void updateTic();
	void applyForce(G308_Point*);
	bool willcollidenormal(G308_Point* normal);
	bool collideNormal(G308_Point* normal);
	void applyTickMovement();
	bool ballwillcollide(collidable* otherball);
	bool collision(collidable*);
	 G308_Point* getPosition();
	 G308_Point* getVector();
	 float* getMass();
	 ~ball();
};
