//---------------------------------------------------------------------------
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
// Copyright (c) 2012 by Taehyun Rhee
//
// Edited by Roma Klapaukh, Daniel Atkins, and Taehyun Rhee
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include "G308_Skeleton.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include "define.h"

using namespace std;

//takes a filename for an asf file
Skeleton::Skeleton(char* filename) {
	//set up defaults
	numBones = 1;
	maxBones = 60;
	angle = 0;
	selectedAxis = -1;
	selectedBone = -1;
	root = (bone*) malloc(sizeof(bone) * maxBones);
	for (int i = 0; i < maxBones; i++) {
		root[i].numChildren = 0;
		root[i].dir.x = 0;
		root[i].dir.y = 0;
		root[i].dir.z = 0;
		root[i].dof = DOF_NONE;
		root[i].length = 0;
		root[i].name = NULL;
		root[i].id = i;
		root[i].children = (bone**) malloc(sizeof(bone*) * 5);

		//other useful items
		root[i].currentCamRotation = 0;

	}
	char*name = (char*) malloc(sizeof(char) * 5);
	name[0] = 'r';
	name[1] = name[2] = 'o';
	name[3] = 't';
	name[4] = '\0';
	root[0].name = name;
	root[0].dof = DOF_ROOT;
	readASF(filename);
	//generate intial positions
	calculatePositions(root);
	NUM_BONES = numBones;

	//Delta of distance to the end point to finish on
	DIST_DELTA = 0.1f;

	//Initialise our Bone_Data array
	B_DATA = (IK_Rotation*) malloc(sizeof(IK_Rotation) * NUM_BONES);

	for (int i = 0; i<NUM_BONES; i++) {
		//zero Bone position for each Bone Data struct
		B_DATA[i].B_POS = root[i].pos;
		//set up the bone_id's for each Bone Data struct
		B_DATA[i].bone_id = root[i].id;
		//null parent link
		B_DATA[i].parent = NULL;
	}
	//generate child structure by going through bones again
	//and adding all as per the bone array, as well as that
	//make sure each child has a link to it's parent as IK works
	//backwards
	for (int i = 0; i<NUM_BONES; i++) {
		bone current_bone = root[i];
		for(int j = 0; j<current_bone.numChildren; j++) {
			B_DATA[i].num_children = current_bone.numChildren;
			B_DATA[i].children = (IK_Rotation**) malloc(sizeof(IK_Rotation*)*B_DATA[i].num_children);
			IK_Rotation* child;
			child = find_IK_Rotation(current_bone.children[j]->id);
			B_DATA[i].children[j] = child;
			child->parent = &B_DATA[i];
			printf("%d %d", child->B_POS.x, child->B_POS.y);
		}
	}
}

Skeleton::~Skeleton() {
	deleteBones(root);
}

void Skeleton::deleteBones(bone* root) {
	free(B_DATA);
	for (int i = 0; i < maxBones; i++) {
		if (root[i].name != NULL) {
			free(root[i].name);
		}
		if (root[i].children != NULL) {
			free(root[i].children);
		}
		//free the root[i].animationList here at some point TODO
	}
	free(root);
}

/**
 * Like a draw but doesn't actually display anything, just calculates the position
 * of each bone at this point in time
 */
void Skeleton::calculatePositions(bone* currentBone) {
	if (currentBone == NULL) {
		return;
	}
	//YOUR CODE GOES HERE
	glPushMatrix();
	GLfloat f[16];
	//Depreciated
	//root->rotation.toMatrix(f);
	//glMultMatrixf(f);

	//if any animations are loaded we should apply the
	//rotations and translations to each bone here
//	updateAnimation(currentBone);

	//undo axis transforms as they are not
	//applied to bone drawing or translations
	//Depreciated
	//root->rotation.multiplicativeInverse().toMatrix(f);
	//glMultMatrixf(f);

	//save our position at this point
	 glGetFloatv(GL_MODELVIEW_MATRIX, f);
	 //f now has the state of the model view
	 currentBone->pos = {f[12], f[13], f[14]};
	//translate to new location from current
	glTranslatef(currentBone->dir.x*currentBone->length, currentBone->dir.y*currentBone->length, currentBone->dir.z*currentBone->length);

	//draw children at updated position
	//it's important to note that any rotations made earlier that are not explicitly
	//undone will be passed on to children
	for (int i=0;i<currentBone->numChildren; i++) {
		calculatePositions(currentBone->children[i]);
	}
	glPopMatrix();
}

// [Assignment2] you may need to revise this function
void Skeleton::display() {
	if (root == NULL) {
		return;
	}
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(0.05, 0.05, 0.05);

	GLUquadric* quad = gluNewQuadric(); //Create a new quadric to allow you to draw cylinders
	if (quad == 0) {
		printf("Not enough memory to allocate space to draw\n");
		exit(EXIT_FAILURE);
	}

	//if r has been pressed we update root
	if (rootRotation) {
		float y = root->currentCamRotation;
		y = y+5;
		if (y > 360) {
			y = 0+(y-360);//clamp max
		}
		else if (y < 0 ) {
			y = 360-(0-y);//clamp min
		}
		root->currentCamRotation = y;
		printf("Rotated:%f\n", y);
		rootRotation = 0;
	}
	//do the display drawing of the item
	//if the state is one do a colour draw
	//for picking and selection
	//otherwise draw normally
	displayNormal(root, quad);

	//clean up memory
	gluDeleteQuadric(quad);
	glPopMatrix();
}

// [Assignment2] you need to fill this function
void Skeleton::displayNormal(bone* currentBone, GLUquadric* q) {
	if (currentBone == NULL) {
		return;
	}
	//YOUR CODE GOES HERE
	//first align local axis
	glPushMatrix();

	//apply result
	GLfloat f[16];
	root->rotation.toMatrix(f);
	glMultMatrixf(f);


	//draw the axes
	//they are tied to the local axis system
	//and hence don't change
	//we don't draw the root bones axes
	//things happen in
	if (strcmp(currentBone->name,root[0].name)) {
		drawAxes(q, currentBone);
	}



	//if any animations are loaded we should apply the
	//rotations and translations to each bone here
	updateAnimation(currentBone);

	//draw the joint
	if (selectedBone==currentBone->id) {
		//highlight
		glColor3f(1, 0, 1);
	}
	else {
		glColor3f(0,1,1); // Joint
	}
	glutSolidSphere(0.4, 5, 5);

	//undo axis transforms as they are not
	//applied to bone drawing or translations
	root->rotation.multiplicativeInverse().toMatrix(f);
	glMultMatrixf(f);

	glPushMatrix();//save before magical bone draw
	//set color for bone, currently off grey
	if (selectedBone==currentBone->id) {
		//highlight
		glColor3f(0.5, 0.5, 0);
	}
	else{
		glColor3f(0.6, 0.6, 0.6);
	}
	drawMagicalBone(currentBone, q);
	glPopMatrix();//undo magical bone draw

	//translate to new location from current
	glTranslatef(currentBone->dir.x*currentBone->length, currentBone->dir.y*currentBone->length, currentBone->dir.z*currentBone->length);

	//draw children at updated position
	//it's important to note that any rotations made earlier that are not explicitly
	//undone will be passed on to children
	for (int i=0;i<currentBone->numChildren; i++) {
		displayNormal(currentBone->children[i], q);
	}
	glPopMatrix();
}

void Skeleton::drawAxes(GLUquadric* q, bone* root) {
	//begin drawing local axis into scene

	glPushMatrix(); // Z-axis
	if (root->id == selectedBone && selectedAxis == 2) {
		glColor3f(0.5, 0.5, 0);
	}
	else {
		glColor3f(0,0,1);
	}
	gluCylinder(q,0.1,0.1,0.8, 5, 5);
	glTranslatef(0,0,0.8);
	glutSolidCone(0.2, 0.2, 5, 5);
	glPopMatrix();// end Z-axis

	glPushMatrix(); // Y-axis
	if (root->id == selectedBone && selectedAxis == 1) {
		glColor3f(0.5, 0.5, 0);
	}
	else {
		glColor3f(0,1,0);
	}
	glRotatef(90,1,0,0);
	gluCylinder(q,0.1,0.1,0.8, 5, 5);
	glTranslatef(0,0,0.8);
	glutSolidCone(0.2, 0.2, 5, 5);
	glPopMatrix(); // end Y-axis

	glPushMatrix(); // X-axis
	if (root->id == selectedBone && selectedAxis == 0) {
		glColor3f(0.5, 0.5, 0);
	}
	else {
		glColor3f(1,0,0);
	}
	glRotatef(90,0,1,0);
	gluCylinder(q,0.1,0.1,0.8, 5, 5);
	glTranslatef(0,0,0.8);
	glutSolidCone(0.2, 0.2, 5, 5);
	glPopMatrix();// end X-axis
}

void Skeleton::drawMagicalBone(bone* root, GLUquadric* q) {
	//determine normal of root and world -Z
	float ang = acos(root->dir.z);
	//rotate world -z to face along the direction of the bone
	glRotatef(ang*180/M_PI, -root->dir.y, root->dir.x, 0);
	//draw a bone
	gluCylinder(q, 0.1, 0.1, root->length, 10, 10);

}

//apply any animations found in the postures 2d vector!
void Skeleton::updateAnimation(bone* current) {
	GLfloat f[16];
	//int id = current->id;
	quaternion* q = NULL;
	getRotation(current->id, q);
	q->toMatrix(f);
	glMultMatrixf(f);
	return;
}


bone* Skeleton::findBone(char * name) {
	//search the bone list for the bone and return it
	for(int i=0; i<numBones; i++) {
		if(strcmp(root[i].name, name) == 0 ) {
			return &root[i];
		}
	}
	printf("Couldn't find a bone matching %s\n", name);
	exit(EXIT_FAILURE);
}
/*
 * IK METHODS BELOW THIS POINT
 */

//Public Methods:

/**
 * Get the rotation for the bone given by the bone_id
 */
int Skeleton::getRotation(int bone_id, quaternion* q) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			q =  &B_DATA[i].B_ROT;
			return 1;
		}
	}
	return 0;
}

/**
 * Attempt to generate a solution for the given end point
 */
void Skeleton::solveIK(G308_Point goal, bone* end_effector) {
	IK_Rotation* bone_data = NULL;
	bone_data = find_IK_Rotation(end_effector->id);
	IK_Rotation* cur_rot_point = bone_data;
	quaternion angle;
	bool complete = false;
	int i = 0;
	//we run over some set of max iterations
	while( i<MAX_IK_RUNS && complete) {
		//if our cur_rot_point has a parent
		while(cur_rot_point->parent!=NULL && complete) {
			//set the cur_rot_point to the parent
			cur_rot_point = cur_rot_point->parent;
			//find an angle of rotation around cur_rot_points position that brings
			//end effector closest to our goal
			angle = calculateRotation(goal, cur_rot_point->B_POS, bone_data->B_POS);
			GLfloat m[16];
			angle.toMatrix(m);
			//apply this rotation around cur_rot_point to cur_rot_point and all of the
			//children of cur_rot_point
			applyRotation(m, cur_rot_point, cur_rot_point->B_POS);
			//check how far end_effector is from our goal
			float dist = vector_length(subtract(goal, bone_data->B_POS));
			if(dist < DIST_DELTA) {
				//if we are within distance we break the loop
				complete = true;
			}
		}
		i++;
	}
	//iteration completed
}

//Private Methods:
IK_Rotation* Skeleton::find_IK_Rotation(int bone_id) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			return &(B_DATA[i]);

		}
	}
	return 0;
}
/**
 * take a rotation matrix and the current bone id and update
 * it and all it's children's positions by rotating them around the rot point
 * by the rotation matrix
 */
void Skeleton::applyRotation(GLfloat* matrix, IK_Rotation* bone_data, G308_Point rot_point) {
	G308_Point pos = bone_data->B_POS;
	//find the vector line between pos and rot_point
	G308_Point line = {pos.x-rot_point.x, pos.y - rot_point.y, pos.z - rot_point.z};
	//apply rotation to the line
	line = getRotatedVector(line, matrix);
	//find new end position
	pos = {rot_point.x+line.x, rot_point.y+line.y, rot_point.z+line.z};
	//update stored position
	bone_data->B_POS = pos;
	//repeat for all children
	for (int i = 0; i<bone_data->num_children; i++) {
		applyRotation(matrix, bone_data->children[i], rot_point);
	}
}

/**
 * Find a rotation around rot point that brings the end point as close as possible
 * to the goal point
 */
quaternion Skeleton::calculateRotation(G308_Point goal, G308_Point rot_point, G308_Point end) {
	//we are currently doing this without thinking about DOF constraints
	//vector from rotation point to goal point
	G308_Point goal_rot = subtract(goal, rot_point);
	//vector from rotation point to end effector
	G308_Point rot_end = subtract(end, rot_point);

	//angle and axis thats being rotated around
	float angle;
	G308_Point axis;

	goal_rot = normalise(goal_rot);
	rot_end = normalise(rot_end);
	//find dot of normalised vectors
	angle = dotProduct(goal_rot, rot_end);
	//angle is the inverse cos
	angle = acos(angle);
	//axis is the vector at right angles to both other vectors
	axis = crossProduct(goal_rot, rot_end);
	axis = normalise(axis);

	//return the quaternion representing this rotation
	return quaternion(angle, axis);

}

/**
 * Return a vector that represents the vector p rotated by
 * the rotation matrix matrix. this is a dup of a function
 * in main but I couldnt' be bothered putting them in a good place
 */
G308_Point Skeleton::getRotatedVector(G308_Point p, float* matrix) {

	G308_Point newPoint = {
			p.x * matrix[0] + p.y * matrix[4] + p.z * matrix[8] + matrix[12],
			p.x * matrix[1] + p.y * matrix[5] + p.z * matrix[9] + matrix[13],
			p.x * matrix[2] + p.y * matrix[6] + p.z * matrix[10] + matrix[14]
	};
	return newPoint;
}

G308_Point Skeleton::normalise(G308_Point temp) {
	float l = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
	temp = {temp.x/l, temp.y/l, temp.z/l};
	return temp;
}

/**
 * Reading methods below this point
 * ---------------------------------------------------------------------------------
 */


/**
 * Read in a ASF file and create the bones they represent.
 */
bool Skeleton::readASF(char* filename) {
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to open file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	char* buff = new char[BUFF_SIZE];
	char *p;

	printf("Starting reading skeleton file\n");
	while ((p = fgets(buff, BUFF_SIZE, file)) != NULL) {
		//Reading actually happened!

		char* start = buff;
		trim(&start);

		//Check if it is a comment or just empty
		if (buff[0] == '#' || buff[0] == '\0') {
			continue;
		}

		start = strchr(buff, ':');
		if (start != NULL) {
			//This actually is a heading
			//Reading may actually consume the next heading
			//so headings need to be a recursive construct?
			readHeading(buff, file);
		}
	}

	delete[] buff;
	fclose(file);
	printf("Completed reading skeleton file\n");
	return true;
}

/**
 * Trim the current string, by adding a null character into where the comments start
 */
void Skeleton::decomment(char * buff) {
	char* comStart = strchr(buff, '#');
	if (comStart != NULL) {
		//remove irrelevant part of string
		*comStart = '\0';
	}
}

void Skeleton::readHeading(char* buff, FILE* file) {
	char* temp = buff;
	decomment(buff);
	trim(&temp);

	char head[50];
	char rest[200];
	int num = sscanf(temp, ":%s %s", head, rest);
	if (num == 0) {
		printf("Could not get heading name from %s, all is lost\n", temp);
		exit(EXIT_FAILURE);
	}
	if (strcmp(head, "version") == 0) {
		//version string - must be 1.10
		char* version = rest;
		if (num != 2) {
			char *p = { '\0' };
			while (strlen(p) == 0) {
				char* p = fgets(buff, BUFF_SIZE, file);
				decomment(p);
				trim(&p);
				version = p;
			}
		}
		if (strcmp(version, "1.10") != 0) {
			printf("Invalid version: %s, must be 1.10\n", version);
			exit(EXIT_FAILURE);
		}
		//Finished reading version so read the next thing?
	} else if (strcmp(head, "name") == 0) {
		//This allows the skeleton to be called something
		//other than the file name
		//We don't actually care what the name is, so can
		//ignore this whole section

	} else if (strcmp(head, "documentation") == 0) {
		//Documentation section has no meaningful information
		//only of use if you want to copy the file. So we skip it
	} else if (strcmp(head, "units") == 0) {
		//Has factors for the units
		//To be able to model the real person,
		//these must be parsed correctly
		//Only really need to check if deg or rad, but even
		//that is probably not needed for the core or extension
	} else if (strcmp(head, "root") == 0) {
		//Read in information about root
		//Or be lazy and just assume it is going to be the normal CMU thing!
	} else if (strcmp(head, "bonedata") == 0) {
		//Description of each bone
		//This does need to actually be read :(
		char *p;
		while ((p = fgets(buff, BUFF_SIZE, file)) != NULL) {
			decomment(p);
			trim(&p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin for bone data %d, found \"%s\"", numBones, p);
					exit(EXIT_FAILURE);
				} else {
					readBone(buff, file);
					numBones++;
				}

			}
		}
	} else if (strcmp(head, "hierarchy") == 0) {
		//Description of how the bones fit together
		char *p;
		while ((p = fgets(buff, BUFF_SIZE, file)) != NULL) {
			trim(&p);
			decomment(p);
			if (strlen(p) > 0) {
				if (p[0] == ':') {
					return readHeading(buff, file);
				} else if (strcmp(p, "begin") != 0) {
					printf("Expected begin in hierarchy, found %s", p);
					exit(EXIT_FAILURE);
				} else {
					readHierarchy(buff, file);
				}

			}
		}
	} else {
		printf("Unknown heading %s\n", head);
	}

}

void Skeleton::readHierarchy(char* buff, FILE* file) {
	char *p;
	char t1[200];
	while ((p = fgets(buff, BUFF_SIZE, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of hierarchy
				return;
			} else {
				//Read the root node
				sscanf(p, "%s ", t1);
				bone* rootBone = NULL;
				for (int i = 0; i < numBones; i++) {
					if (strcmp(root[i].name, t1) == 0) {
						rootBone = root + i;
						break;
					}
				}
				//Read the connections
				p += strlen(t1);
				bone* other = NULL;
				while (*p != '\0') {
					sscanf(p, "%s ", t1);

					for (int i = 0; i < numBones; i++) {
						if (strcmp(root[i].name, t1) == 0) {
							other = root + i;
							break;
						}
					}
					if (other == NULL) {
						printf("Unknown bone %s found in hierarchy. Failure", t1);
						exit(EXIT_FAILURE);
					}
					rootBone->children[rootBone->numChildren] = other;
					rootBone->numChildren++;
					p += strlen(t1) + 1;

				}
			}
		}

	}
}

void Skeleton::readBone(char* buff, FILE* file) {
	char *p;
	char t1[50];
	while ((p = fgets(buff, BUFF_SIZE, file)) != NULL) {
		trim(&p);
		decomment(p);
		if (strlen(p) > 0) {
			if (strcmp(p, "end") == 0) {
				//end of this bone
				return;
			} else {
				sscanf(p, "%s ", t1);
				if (strcmp(t1, "name") == 0) {
					//Read the name and actually remember it
					char* name = (char*) malloc(sizeof(char) * 10);
					sscanf(p, "name %s", name);
					root[numBones].name = name;
				} else if (strcmp(t1, "direction") == 0) {
					//Also actually important
					float x, y, z;
					sscanf(p, "direction %f %f %f", &x, &y, &z);
					root[numBones].dir.x = x;
					root[numBones].dir.y = y;
					root[numBones].dir.z = z;
				} else if (strcmp(t1, "length") == 0) {
					//Also actually important
					float len;
					sscanf(p, "length %f", &len);
					root[numBones].length = len;
				} else if (strcmp(t1, "dof") == 0) {
					//Read the degrees of freedom!
					char d1[5];
					char d2[5];
					char d3[5];
					int num = sscanf(p, "dof %s %s %s", d1, d2, d3);
					switch (num) {
					DOF dof;
					case 3:
						dof = dofFromString(d3);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 2:
						dof = dofFromString(d2);
						root[numBones].dof = root[numBones].dof | dof;
						//fallthrough!!
						/* no break */
					case 1:
						dof = dofFromString(d1);
						root[numBones].dof = root[numBones].dof | dof;
						break;
					}
				} else if (strcmp(t1, "axis") == 0) {
					//Read the rotation axis
					float x, y, z;
					int num = sscanf(p, "axis %f %f %f XYZ", &x, &y, &z);
					if (num != 3) {
						printf("axis format doesn't match expected\n");
						printf("Expected: axis %%f %%f %%f XYZ\n");
						printf("Got: %s", p);
						exit(EXIT_FAILURE);
					}
					//generate a quaternion rotation for the axis offsets
					G308_Point t;
					t.x = 1;
					t.y = 0;
					t.z = 0;
					quaternion q_x(x, t);
					//q_x.print();
					t.x = 0;
					t.y = 1;
					quaternion q_y(y, t);
					//q_y.print();
					t.y = 0;
					t.z = 1;
					quaternion q_z(z, t);
					//q_z.print();

					quaternion complete = q_x * q_y * q_z;
					root[numBones].rotation= complete;
				}
				//There are more things but they are not needed for the core
			}

		}
	}
}

/**
 * Helper function to turn a DOF from the AMC file into the correct DOF value
 */
DOF Skeleton::dofFromString(char* s) {
	if (strcmp(s, "rx") == 0)
		return DOF_RX;
	if (strcmp(s, "ry") == 0)
		return DOF_RY;
	if (strcmp(s, "rz") == 0)
		return DOF_RZ;
	printf("Unknown DOF found: %s", s);
	return DOF_NONE;
}

/*
 * Remove leading and trailing whitespace. Increments the
 * pointer until it points to a non whitespace char
 * and then adds nulls to the end until it has no
 * whitespace on the end
 */
void trim(char **p) {
	if (p == NULL) {
		printf("File terminated without version number!\n");
		exit(EXIT_FAILURE);
	}

	//Remove leading whitespace
	while (**p == ' ' || **p == '\t') {
		(*p)++;
	}

	int len = strlen(*p);
	while (len > 0) {
		char last = (*p)[len - 1];
		if (last == '\r' || last == '\n' || last == ' ' || last == '\t') {
			(*p)[--len] = '\0';
		} else {
			return;
		}
	}
}



