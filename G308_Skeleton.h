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

#ifndef SKELETON_H
#define SKELETON_H

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

#include "define.h"
#include "quaternion.h"

// Using bitmasking to denote different degrees of freedom for joint motion
typedef int DOF;

#define DOF_NONE 0
#define DOF_RX 1
#define DOF_RY 2
#define DOF_RZ 4
#define DOF_ROOT 8 // Root has 6, 3 translation and 3 rotation
#define BONE_SIZE 4
#define MAX_IK_RUNS 5
#define STEP_AMOUNT 200

struct IK_Rotation {
	quaternion B_ROT;
	quaternion B_ROT_END;
	G308_Point B_POS;
	int bone_id;
	int num_children;
	IK_Rotation** children;
	IK_Rotation* parent;
};

//Type to represent a bone
typedef struct bone {
	char* name;
	G308_Point dir;
	G308_Point pos;
	quaternion rotation;
	//bone id is this items index to the bone array
	int id;
	//camera rotation
	float currentCamRotation;
	DOF dof;
	float length;
	bone** children;
	int numChildren;
} bone;

void trim(char**);

class Skeleton {
private:
	G308_Point default_pos;
	IK_Rotation* curr_rot_point;
	int maxBones, m_numFrames;
	bone* root;
	float camRotation;
	float stored_angle;
	G308_Point stored_axis;
	bool animationExists;

	//file reading methods for asf
	bool readASF(char*);
	void readHeading(char*, FILE*);
	void decomment(char*);
	void deleteBones(bone*);
	void readBone(char*, FILE*);
	void readHierarchy(char*, FILE*);
	DOF dofFromString(char*);

	//misc methods
	void calculatePositions(bone *);
	void calculateInitialPositions(bone *);
	void setDefaultPostures();

	//display methods
	void displayNormal(bone*, GLUquadric*);
	void drawMagicalBone(bone*, GLUquadric*);
	void drawAxes(GLUquadric*, bone*);
	void updateAnimation(bone*);
	//IK METHODS AND VARIABLES
	//Variables:
	//distance delta
	float DIST_DELTA;
	//Pointer to robot skeleton
	Skeleton* ROBOT;

	IK_Rotation* B_DATA;

	//number of bones in our robot
	int NUM_BONES;

	//Methods:

	//take a rotation matrix and the current bone id and update
	//it and all it's children's positions by rotating them around the rot point
	//by the rotation matrix, stopping if it reaches the goal point
	void applyRotation(GLfloat* matrix, IK_Rotation* bone_data, G308_Point rot_point);

	//calculate a rotation around x/y/z at rot_point that will bring end closest to goal
	quaternion calculateRotation(G308_Point goal, G308_Point rot_point, G308_Point end);

	//given a bone pointer, find the IK_Rotation that matches that bone's id
	IK_Rotation* find_IK_Rotation(int bone_id);

	//return a vector representing teh vector p rotated by matrix matrix
	G308_Point getRotatedVector(G308_Point p, float* matrix);

	//normalise vector
	G308_Point normalise(G308_Point);
	//add vec parts
	float vec_total(G308_Point);



public:
	float rootRotation;
	int numBones, selectedBone, selectedAxis;
	float angle;
	int step;
	int iterations;
	//find a bone by name
	bone* findBone(char*);
	bone* findBone(int);
	//constructor destructor
	Skeleton(char*, G308_Point);
	~Skeleton();
	//draw the skeleton
	void display();
	//IK methods
	void setEndEffector(int, G308_Point);
	//return a quaternion that is the current angle of the bone given
	//by bone_id, this will also increase the rotation of that angle
	//for the next call
	quaternion* getRotation(int bone_id);
	void setIterations(int i);
	//attempt to generate a solution for this end goal and effector pair
	void solveIK(G308_Point Goal, bone* end_effector);
};




#endif

