/*
 * ball.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "Collidable.h"
class ball: collidable{
public:

	ball(float radius, G308_Point initialposition, G308_Point initialvelocity);
	void render();
	void updateTic();
	void applyForce(G308_Point*);
	bool collision(collidable*);
	 G308_Point* getPosition();
	 G308_Point* getVector();
	 float* getMass();
	 ~ball();
};
