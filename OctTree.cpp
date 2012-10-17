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
//change these

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize) {
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
	if (depth == 0) {
		rootnode = this;
	}
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
	back->z = 0;

}

OctTree::OctTree(int currentLevel, G308_Point* bottomleftCorner, float isize,
		OctTree* rootnodea) {
	depth = currentLevel;
	botleft = bottomleftCorner;
	size = isize;
	rootnode = rootnodea;

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
	front->x = 0;
	front->y = 0;
	back->z = 1;
	back->y = 0;
	back->z = 0;
}

void OctTree::add(){
	G308_Point* pi = new G308_Point;
	pi->x = LowerLeftCorner.x+2;

	pi->y = LowerLeftCorner.y+1;

	pi->z = LowerLeftCorner.z+3;

	G308_Point* vec = new G308_Point;
	vec->x= -0.01;
	vec->y = -0.01;
	vec->z = -0.01;

	ball* b = new ball(0.2,*pi,*vec);
	add(b);
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

		balls->insert(ball);
		//if (ballNumber > MAX_OBJ_PERTREE) {
		//	splitSelf();
		//}
	}

}
void OctTree::renderTree() {
	if(childrenpresent==true){
		for(int x = 0; x<2;x++){
			for(int y = 0; y<2;y++){
				for(int z=0;z<2;z++){
					children[x][y][z]->renderTree();
				}
			}
		}


	}else{
	glPushMatrix();
	glTranslatef(LowerLeftCorner.x+((1.0/2.0)*size),LowerLeftCorner.y+((1.0/2.0)*size),LowerLeftCorner.z+((1.0/2.0)*size));
	printf("%f %f %f: lowerleftx y z\n",LowerLeftCorner.x,LowerLeftCorner.y,LowerLeftCorner.z);
	printf("size %f \n", size);
	glutWireCube(size);
	glPopMatrix();
	}
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
		puts("x1");
		possiblechildren = possiblechildren | X_TOP;
		if (xmin < LowerLeftCorner.x + childsize) {
			puts("x2");
			possiblechildren = possiblechildren | X_BOT;
		}

	} else {
		printf("X size %f, llc %f, child %f\n", xmax, LowerLeftCorner.x, childsize);
		puts("x3");
		possiblechildren = possiblechildren | X_BOT;
	}

	if (ymax > LowerLeftCorner.y + childsize) {
		puts("y1");
		possiblechildren = possiblechildren | Y_TOP;
		if (ymin < LowerLeftCorner.y + childsize) {
			puts("y2");
			possiblechildren = possiblechildren | Y_BOT;
		}

	} else {
		printf("Y size %f, llc %f, child %f\n", ymax, LowerLeftCorner.y, childsize);
		puts("y3");
		possiblechildren = possiblechildren | Y_BOT;
	}

	if (zmax > LowerLeftCorner.z + childsize) {
		puts("z1");
		possiblechildren = possiblechildren | Z_TOP;
		if (zmin < LowerLeftCorner.z + childsize) {\
			puts("z2");
			possiblechildren = possiblechildren | Z_BOT;
		}

	} else {
		printf("Z size %f, llc %f, child %f\n", zmax, LowerLeftCorner.z, childsize);
		puts("z3");
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
				printf("inserting %d %d %d\n",x,y,z);
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
						printf("collisen tered~~~~~~~~~~~~~~~\n");
						b1->collision(b2);
					}

				}

			}



			if ((*iter)->position.x <  (rootnode->LowerLeftCorner.x)) {
												//if((*iter)->willcollidenormal(right)){
												printf("xcollide bot\n");
												((*iter)->collideNormal(left));

												//}
											}


			if ((*iter)->position.x >  (rootnode->LowerLeftCorner.x+size)) {
										//if((*iter)->willcollidenormal(right)){

										((*iter)->collideNormal(left));
										printf("xcollide top\n");
										//}
									}


			 if ((*iter)->position.y >  (rootnode->LowerLeftCorner.y+size)) {
							//if((*iter)->willcollidenormal(right)){
							((*iter)->collideNormal(top));
							printf("ycollide top\n");
							//}
						}
			 if ((*iter)->position.y <  rootnode->LowerLeftCorner.y) {
									//if((*iter)->willcollidenormal(right)){
									printf("something\n");
									((*iter)->collideNormal(bot));

									//}
			}

			 if((*iter)->position.z<(rootnode->LowerLeftCorner.z)){
				 ((*iter)->collideNormal(front));
					printf("important\n");

			 }
			 if((*iter)->position.z>(rootnode->LowerLeftCorner.z+size)){
				 ((*iter)->collideNormal(front));
					printf("collide front\n");
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
	ballNumber=0;
	for (set<ball*>::iterator iter = balls->begin(); iter != balls->end();
			++iter) {
		add(*iter);

	}
	balls->clear();


}

OctTree::~OctTree() {
	// TODO Auto-generated destructor stub

}

