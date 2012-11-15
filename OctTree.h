/*
 * OctTree.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */


#include <set>
#include "ball.h"
#include "fluid.h"
using namespace std;


class OctTree {
public:
	OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize, Fluid*);
	OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize, OctTree* rootnode, Fluid*);
	G308_Point LowerLeftCorner;
	G308_Point UpperRightcorner;
	G308_Point center;
	G308_Point faceNormal;
	bool childrenpresent;
	bool surfaceNode;
	std::set<ball*> *balls;
	OctTree *children[2][2][2];
	OctTree *rootnode;
	G308_Point* top ;
	G308_Point* bot ;
	G308_Point* left ;
	G308_Point* right ;
	G308_Point* front ;
	G308_Point* back ;
	Fluid* fluid;
	int MAX_TREE_DEPTH ;
	int MIN_OBJ_PERTREE ;
	int MAX_OBJ_PERTREE ;
	int depth;
	int level;
	int ballNumber;
	float xplane;
	float yplane;
	float zplane;
	G308_Point* botleft;
	float size;
	#define X_TOP 1
	#define X_BOT 2
	#define Y_TOP 4
	#define Y_BOT 8
	#define Z_TOP 16
	#define Z_BOT 32

	void whichchildren(ball* ball,bool addflag);
	void remove(ball*);
	void splitSelf();
	void add(ball* ball);
	void add();
	void renderTree();
	void removeChildren();
	void pullballsfromChildren();
	void moveBalls();
	void performCollisions();
	virtual ~OctTree();




};


