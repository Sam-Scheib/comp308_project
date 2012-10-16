/*
 * ball.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#ifndef _BALL_H
#define _BALL_H

//#include "Collidable.h"
#include "define.h"

class ball{
public:
	float radi;
	G308_Point position;
	G308_Point velocity;
	ball(float radius, G308_Point initialposition, G308_Point initialvelocity);
	void render();

	float radius();
	void updateTic();
	void applyForce(G308_Point*);
	bool willcollidenormal(G308_Point* normal);
	bool collideNormal(G308_Point* normal);
	void applyTickMovement();
	bool ballwillcollide(ball* otherball);
	bool collision(ball*);
	 G308_Point* getPosition();
	 G308_Point* getVector();
	 float* getMass();
	~ball();
};

#endif
