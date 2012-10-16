/*
 * OctTree.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "OctTree.h"
//change these
int MAX_TREE_DEPTH = 8;
int MIN_OBJ_PERTREE = 3;
int MAX_OBJ_PERTREE = 6;
int depth;
int level;
int ballNumber;
G308_Point* botleft;
float size;
#define X_TOP 1
#define X_BOT 2
#define Y_TOP 4
#define Y_BOT 8
#define Z_TOP 16
#define Z_BOT 32

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize) {
	// TODO Auto-generated constructor stub
	depth = currentLevel;
	botleft = bottomleftCorner;
	size = isize;
	if (depth == 0) {
		rootnode == this;
	}

}

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize,
		OctTree* rootnodea) {
	depth = currentLevel;
	botleft = bottomleftCorner;
	size = isize;
	rootnode = rootnodea;
}
/*
 * Add To the oct tree, seeing if we have children, If we Have no children we just add the object.
 *  Then we check if we need to/ can remodel the tree.
 */
void OctTree::add(ball* ball) {

	if (childrenpresent) {
		whichchildren(ball, true);

	} else {
		ballNumber++;

		balls.insert(ball);
		if (ballNumber > MAX_OBJ_PERTREE) {
			splitSelf();
		}
	}

}
void OctTree::renderTree() {

}

void OctTree::whichchildren(ball* ball, bool addflag) {
	float xmax = ball->getPosition()->x + ball->radi;
	float xmin = ball->getPosition()->x - ball->radi;
	float ymax = ball->getPosition()->y + ball->radi;
	float ymin = ball->getPosition()->y - ball->radi;
	float zmax = ball->getPosition()->z + ball->radi;
	float zmin = ball->getPosition()->z - ball->radi;
	float childsize = size / 2;

	unsigned char switchlist[8] = { X_TOP | Y_TOP | Z_TOP,
			X_BOT | Y_TOP | Z_TOP, X_TOP | Y_BOT | Z_TOP, X_BOT | Y_BOT | Z_TOP,
			X_TOP | Y_TOP | Z_BOT, X_BOT | Y_TOP | Z_BOT, X_TOP | Y_BOT | Z_BOT,
			X_BOT | Y_BOT | Z_BOT

	};

	unsigned char possiblechildren = 0;

	if (xmax > LowerLeftCorner.x + childsize) {
		possiblechildren = possiblechildren | X_TOP;
		if (xmin < LowerLeftCorner.x + childsize) {
			possiblechildren = possiblechildren | X_BOT;
		}

	} else {
		possiblechildren = possiblechildren | X_BOT;
	}

	if (ymax > LowerLeftCorner.y + childsize) {
		possiblechildren = possiblechildren | Y_TOP;
		if (ymin < LowerLeftCorner.y + childsize) {
			possiblechildren = possiblechildren | Y_BOT;
		}

	} else {
		possiblechildren = possiblechildren | Y_BOT;
	}

	if (zmax > LowerLeftCorner.z + childsize) {
		possiblechildren = possiblechildren | Z_TOP;
		if (zmin < LowerLeftCorner.z + childsize) {
			possiblechildren = possiblechildren | Z_BOT;
		}

	} else {
		possiblechildren = possiblechildren | Z_BOT;
	}

	for (int i = 0; i < 8; i++) {
		if ((possiblechildren & switchlist[i]) == switchlist[i]) {

			int x = 0, y = 0, z = 0;
			if (switchlist[i] & X_TOP)
				x = 1;
			else
				x = 0;
			if (switchlist[i] & Z_TOP)
				z = 1;
			else
				z = 0;
			if (switchlist[i] & Y_TOP)
				y = 1;
			else
				y = 0;

			if (addflag) {
				children[x][y][z]->add(ball);
			} else {
				children[x][y][z]->remove(ball);
			}

		}

	}

}

void OctTree::remove(ball* ball) {

	if (childrenpresent) {

		whichchildren(ball, false);

	} else {

		ballNumber--;

		balls.erase(ball);

	}

}

void OctTree::pullballsfromChildren() {
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				if (children[x][y][z]->childrenpresent) {

					balls.insert(children[x][y][z]->balls.begin(),
							children[x][y][z]->balls.end());

				} else {
					children[x][y][z]->removeChildren();
					if (children[x][y][z]->childrenpresent == false) {
						balls.insert(children[x][y][z]->balls.begin(),
								children[x][y][z]->balls.end());
					} else {
						//		printf("Something broke in pullballs \n");
					}
				}
			}
		}
	}

}

void OctTree::removeChildren() {
	pullballsfromChildren();
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				delete children[x][y][z];
			}
		}
	}

	childrenpresent = false;

}
void OctTree::performCollisions() {

	if (childrenpresent) {
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					children[x][y][z]->performCollisions();
				}
			}
		}
	} else {

		for (set<ball*>::iterator iter = balls.begin(); iter != balls.end();) {
			for (set<ball*>::iterator rator = balls.begin();
					rator != balls.end();) {
				if (iter == rator) {
					//skip
				} else {
					//Test Collision / enact collision with your patner
					ball* b1 = *iter;
					ball* b2 = *rator;
					if(b1->ballwillcollide(b2)){
					b1->collision(b2);}

				}
			}
		}

	}

}

void OctTree::moveBalls() {
	if (childrenpresent) {
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					children[x][y][z]->moveBalls();
				}
			}
		}

	} else {
		for (set<ball*>::iterator iter = balls.begin(); iter != balls.end();) {
			ball* cball = *iter;
			remove((*iter));
			cball->applyTickMovement();
			cball->render();
			rootnode->add(cball);

		}
	}

}

void OctTree::splitSelf() {
	//If this operation has performed, we now have children
	childrenpresent = true;
	float childsize = size / 2;
	G308_Point* childnodepos = new G308_Point;

	for (int x = 0; x < 2; x++) {
		childnodepos->x = botleft->x + x * childsize;
		for (int y = 0; y < 2; y++) {
			childnodepos->y = botleft->y + y * childsize;
			for (int z = 0; z < 2; z++) {
				childnodepos->z = botleft->z + z * childsize;
				children[x][y][z] = new OctTree(depth + 1, childnodepos,
						childsize, rootnode);

			}
		}

	}

	//Put things in the right hole

	for (set<ball*>::iterator iter = balls.begin(); iter != balls.end();
			++iter) {
		add(*iter);

	}
	balls.clear();

}

OctTree::~OctTree() {
	// TODO Auto-generated destructor stub

}

