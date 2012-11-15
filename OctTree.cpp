/*
 * OctTree.cpp
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#include "OctTree.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include "quaternion.h"
#include "math.h"

//change these

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize, Fluid* fluidx) {
	// TODO Auto-generated constructor stub
	depth = currentLevel;
	botleft = bottomleftCorner;
	size = isize;
	MAX_OBJ_PERTREE = 3;
	depth = 0;
	level = 0;
	ballNumber = 0;
	childrenpresent = false;
	balls = new std::set<ball*>();
	fluid= fluidx;
	rootnode = this;

	LowerLeftCorner = *bottomleftCorner;

	top = new G308_Point;
	bot = new G308_Point;
	left = new G308_Point;
	right = new G308_Point;
	front = new G308_Point;
	back = new G308_Point;
	top->x = 0;
	top->y = -1;
	top->z = 0;
	bot->x = 0;
	bot->y = 1;
	bot->z = 0;
	left->x = -1;
	left->y = 0;
	left->z = 0;
	right->x = 1;
	right->y = 0;
	right->z = 0;
	front->z = -1;
	front->x = 0;
	front->y = 0;
	back->z = 1;
	back->y = 0;
	back->x = 0;
	surfaceNode = true;

}

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize,
		OctTree* rootnodea, Fluid* xfluid) {
	depth = currentLevel;
	botleft = bottomleftCorner;
	size = isize;
	rootnode = rootnodea;
	fluid = xfluid;
	MAX_OBJ_PERTREE = 3;

	level = 0;
	ballNumber = 0;
	childrenpresent = false;
	balls = new std::set<ball*>();

	LowerLeftCorner = *bottomleftCorner;
	top = new G308_Point;
	bot = new G308_Point;
	left = new G308_Point;
	right = new G308_Point;
	front = new G308_Point;
	back = new G308_Point;

	top->x = 0;
	top->y = -1;
	top->z = 0;

	bot->x = 0;
	bot->y = 1;
	bot->z = 0;

	left->x = -1;
	left->y = 0;
	left->z = 0;

	right->x = 1;
	right->y = 0;
	right->z = 0;

	front->z = -1;
	front->y = 0;
	front->x = 0;

	back->z = 1;
	back->y = 0;
	back->x = 0;
	surfaceNode = true;

}

void OctTree::add() {
	G308_Point* pi = new G308_Point;
	pi->x = LowerLeftCorner.x + rand() % (int)size;

	pi->y = (LowerLeftCorner.y + rand() % (int)size/2)+size/3;

	pi->z = LowerLeftCorner.z + rand() % (int)size;

	G308_Point* vec = new G308_Point;

	vec->x = rand() % 100;
	vec->y = rand() % 100;
	vec->z = rand() % 100;

	float length = sqrt(
			(vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));
	*vec = scalarMultiply(*vec, (1 / (length)));
	*vec = scalarMultiply(*vec, 0.04);

	ball* b = new ball(0.2, *pi, *vec);
	add(b);
}
/*
 * Add To the oct tree, seeing if we have children, If we Have no children we just add the object.
 *  Then we check if we need to/ can remodel the tree.
 */
void OctTree::add(ball* ball) {

	if (surfaceNode == false) {
		whichchildren(ball, true);

	} else {
		ballNumber++;

		balls->insert(ball);
		if (balls->size() > MAX_OBJ_PERTREE && depth < 3) {
			surfaceNode = false;
			splitSelf();
		}
	}

}
void OctTree::renderTree() {
	if (childrenpresent) {
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					children[x][y][z]->renderTree();
				}
			}
		}
	}

	glPushMatrix();
	glTranslatef(LowerLeftCorner.x + (0.5 * size),
			LowerLeftCorner.y + (0.5 * size),
			LowerLeftCorner.z + (0.5 * size));
	glutWireCube(size);
	glPopMatrix();

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
		//	puts("x1");
		possiblechildren = possiblechildren | X_TOP;
		if (xmin < LowerLeftCorner.x + childsize) {
			//	puts("x2");
			possiblechildren = possiblechildren | X_BOT;
		}

	} else {
		//printf("X size %f, llc %f, child %f\n", xmax, LowerLeftCorner.x, childsize);
		//puts("x3");
		possiblechildren = possiblechildren | X_BOT;
	}

	if (ymax > LowerLeftCorner.y + childsize) {
		//puts("y1");
		possiblechildren = possiblechildren | Y_TOP;
		if (ymin < LowerLeftCorner.y + childsize) {
			//puts("y2");
			possiblechildren = possiblechildren | Y_BOT;
		}

	} else {
		//printf("Y size %f, llc %f, child %f\n", ymax, LowerLeftCorner.y, childsize);
		//puts("y3");
		possiblechildren = possiblechildren | Y_BOT;
	}

	if (zmax > LowerLeftCorner.z + childsize) {
		//puts("z1");
		possiblechildren = possiblechildren | Z_TOP;
		if (zmin < LowerLeftCorner.z + childsize) {
			//puts("z2");
			possiblechildren = possiblechildren | Z_BOT;
		}

	} else {
		//printf("Z size %f, llc %f, childsize %f\n", zmax, LowerLeftCorner.z, childsize);
		//puts("z3");
		possiblechildren = possiblechildren | Z_BOT;
	}

	for (int i = 0; i < 8; i++) {
		if ((possiblechildren & switchlist[i]) == switchlist[i]) {

			int x = 0, y = 0, z = 0;
			if (switchlist[i] & X_TOP)
				x = 1;
			if (switchlist[i] & Z_TOP)
				z = 1;
			if (switchlist[i] & Y_TOP)
				y = 1;

			if (addflag) {
				//	printf("inserting %d %d %d\n",x,y,z);
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
		//	printf("No child\n");
		ballNumber--;

		balls->erase(ball);

	}

}

void OctTree::pullballsfromChildren() {
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			for (int z = 0; z < 2; z++) {
				if (children[x][y][z]->childrenpresent) {

					balls->insert(children[x][y][z]->balls->begin(),
							children[x][y][z]->balls->end());

				} else {
					children[x][y][z]->removeChildren();
					if (children[x][y][z]->childrenpresent == false) {
						balls->insert(children[x][y][z]->balls->begin(),
								children[x][y][z]->balls->end());
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
					//printf("children enterd\n");
				}
			}
		}
	} else {

		for (set<ball*>::iterator iter = balls->begin(); iter != balls->end();
				iter++) {
			for (set<ball*>::iterator rator = balls->begin();
					rator != balls->end(); rator++) {
				if (iter == rator) {
					//skip
				} else {
					//Test Collision / enact collision with your patner
					ball* b1 = *iter;
					ball* b2 = *rator;
					if (b1->ballwillcollide(b2)) {
						//printf("collisen tered~~~~~~~~~~~~~~~\n");
						b1->collision(b2);
					}

				}

			}

			if ((*iter)->position.x - (*iter)->radi
					< (rootnode->LowerLeftCorner.x)) {

				if ((*iter)->willcollidenormal(left)) {

					((*iter)->collideNormal(left));

				}
			}

			if ((*iter)->position.x + (*iter)->radi
					> (rootnode->LowerLeftCorner.x + rootnode->size)) {

				if ((*iter)->willcollidenormal(right)) {
					((*iter)->collideNormal(left));
				}

			}

			if ((*iter)->position.y + (*iter)->radi
					> (rootnode->LowerLeftCorner.y + rootnode->size)) {

				if ((*iter)->willcollidenormal(bot)) {
					((*iter)->collideNormal(top));
				}

			}
			if ((*iter)->position.y - (*iter)->radi
					< rootnode->LowerLeftCorner.y) {
				;
				if ((*iter)->willcollidenormal(top)) {
					((*iter)->collideNormal(bot));
					//This is where we insert the collsion stuff for his stuff.
					// If sam can make his thing square, we just have to match the size, get the x,y position of the ball, which should map relatively easily to the forrect thing
				}

			}

			if ((*iter)->position.z - (*iter)->radi
					< (rootnode->LowerLeftCorner.z)) {
				if ((*iter)->willcollidenormal(front)) {
					((*iter)->collideNormal(front));
				}

			}
			if ((*iter)->position.z + (*iter)->radi
					> (rootnode->LowerLeftCorner.z + rootnode->size)) {
			//	printf("?\n");
				if ((*iter)->willcollidenormal(back)) {
					((*iter)->collideNormal(back));

				}

				//Works

			}

			(*iter)->render();

			//DIRTY HACK REMOVE BEFORE SHOWING ANYONE
			/*
			 if ((*iter)->position.x > 10) {
			 if ((*iter)->willcollidenormal(right)) {
			 ((*iter)->collideNormal(right));
			 };
			 }
			 else if ((*iter)->position.x < -10) {

			 if((*iter)->willcollidenormal(bot)){
			 ((*iter)->collideNormal(bot));
			 }
			 } else if ((*iter)->position.y < - 10) {
			 if((*iter)->willcollidenormal(left)){
			 (*iter)->collideNormal(left);
			 }

			 }else if ((*iter)->position.y >  10) {
			 //if((*iter)->willcollidenormal(right)){
			 printf("something\n");
			 ((*iter)->collideNormal(top));

			 //}
			 }else if ((*iter)->position.z > +10){
			 if((*iter)->willcollidenormal(back)){
			 (*iter)->collideNormal(back);
			 }
			 }else if ((*iter)->position.z< -10){
			 if((*iter)->willcollidenormal(front)){
			 (*iter)->collideNormal(front);
			 }

			 }
			 */
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
		std::set<ball*> *temp = new std::set<ball*>();
		for (set<ball*>::iterator iter = balls->begin(); iter != balls->end();
				iter++) {
			ball* cball = *iter;

			temp->insert(cball);

		}
		balls->clear();
		ballNumber = 0;

		for (set<ball*>::iterator rat = temp->begin(); rat != temp->end();
				rat++) {
			(*rat)->applyTickMovement();

			rootnode->add(*rat);
		}

	}

}

void OctTree::splitSelf() {
	//If this operation has performed, we now have children

	float childsize = size / 2;
	G308_Point* childnodepos = new G308_Point;

	for (int x = 0; x < 2; x++) {
		childnodepos->x = LowerLeftCorner.x + x * childsize;
		for (int y = 0; y < 2; y++) {
			childnodepos->y = LowerLeftCorner.y + y * childsize;
			for (int z = 0; z < 2; z++) {
				childnodepos->z = LowerLeftCorner.z + z * childsize;
				children[x][y][z] = new OctTree(depth + 1, childnodepos,
						childsize, rootnode, fluid);

			}
		}

	}

	//Put things in the right hole
	ballNumber = 0;
	set<ball*> *temp = new set<ball*>;
	temp->insert(balls->begin(), balls->end());
	balls->clear();
	for (set<ball*>::iterator iter = temp->begin(); iter != temp->end();
			++iter) {
		whichchildren(*iter, true);

	}

	childrenpresent = true;

}

OctTree::~OctTree() {
	// TODO Auto-generated destructor stub

}

