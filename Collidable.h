/*
 * Collidable.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */


#include "define.h"


class collidable {
public:
	virtual bool collision(collidable*)=0;
	virtual G308_Point* getPosition()=0;
	virtual G308_Point* getVector()=0;
	virtual float* getMass()=0;
	virtual void applyForce(G308_Point*)=0;

	virtual ~collidable(){};
};


