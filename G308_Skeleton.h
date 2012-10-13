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

#include "frame_controller.h"
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


//struct G308_Point {
//	float x;
//	float y;
//	float z;
//};

//Posture struct
//contains lots of data about animations
struct Posture
{
	//Root position (x, y, z)
	G308_Point root_pos;
	//Bone rotations (x, y z)
	G308_Point bone_rotation[MAX_BONES_IN_ASF_FILE];
	//Bone rotations in quaternion form
	quaternion bone_rot_q[MAX_BONES_IN_ASF_FILE];
};

//Type to represent a bone
typedef struct bone {
	char* name;
	G308_Point dir;
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
	int maxBones, m_numFrames;
	bone* root;
	float camRotation;
	Posture* m_Postures;
	//file reading methods for asf
	bool readASF(char*);
	void readHeading(char*, FILE*);
	void decomment(char*);
	void deleteBones(bone*);
	void readBone(char*, FILE*);
	void readHierarchy(char*, FILE*);
	DOF dofFromString(char*);

	//misc methods
	void setDefaultPostures();

	//display methods
	void displayNormal(bone*, GLUquadric*);
	void drawMagicalBone(bone*, GLUquadric*);
	void drawAxes(GLUquadric*, bone*);
	void updateAnimation(bone*);



public:
	float rootRotation;
	int numBones, selectedBone, selectedAxis;
	float angle;
	FrameController *frame_ctrl;
	//find a bone by name
	bone* findBone(char*);
	//constructor destructor
	Skeleton(char*);
	~Skeleton();
	//draw the skeleton
	void display();
	//read amc file
	void readAMCFile(char*);
};


#endif

