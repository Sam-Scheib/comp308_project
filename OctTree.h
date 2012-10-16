/*
 * OctTree.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */


#include <set>
#include "ball.h"

using namespace std;


class OctTree {
public:
	OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize);
	OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize, OctTree* rootnode);
	G308_Point LowerLeftCorner;
	G308_Point UpperRightcorner;
	G308_Point center;
	G308_Point faceNormal;
	bool childrenpresent;
	bool surfaceNode;
	set<ball*> balls;
	OctTree *children[2][2][2];
	OctTree *rootnode;

	void whichchildren(ball* ball,bool addflag);
	void remove(ball*);
	void splitSelf();
	void add(ball* ball);
	void renderTree();
	void removeChildren();
	void pullballsfromChildren();
	void moveBalls();
	void performCollisions();
	virtual ~OctTree();



};


