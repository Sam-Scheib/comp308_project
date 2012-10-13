/*
 * IKSolver.cpp
 *
 *  Created on: 14/10/2012
 *      Author: francis
 */

#include "IKSolver.h"

IKSolver::IKSolver(Skeleton * robot, bone* root) {
	//save a pointer to our skeleton robot arm
	ROBOT = robot;
	NUM_BONES = ROBOT->numBones;

	//Initialise our Bone_Data array
	B_DATA = (IK_Rotation*) malloc(sizeof(IK_Rotation) * NUM_BONES);

	for (int i = 0; i<NUM_BONES; i++) {
		//zero Bone position for each Bone Data struct
		B_DATA[i].B_POS.x = 0;
		B_DATA[i].B_POS.y = 0;
		B_DATA[i].B_POS.z = 0;
		//set up the bone_id's for each Bone Data struct
		B_DATA[i].bone_id = root[i].id;
		//set up empty quaternions
		//B_ROTS[i] = quaternion(0, B_POS[1]);
		//B_ROTS[i] = quaternion(0, B_POS[1]);
	}
	//generate child structure by going through bones again
	//and adding all as per the bone array
	for (int i = 0; i<NUM_BONES; i++) {
		bone* current_bone = root[i];
		for(int j = 0; j<current_bone->numChildren; j++) {
			B_DATA[i].children[j] = find_IK_Rotation(current_bone->children[j]->id);
		}
	}

}

IKSolver::~IKSolver() {
	// free the Bone Data array
	free(B_DATA);

}

//Public Methods:

/**
 * Get the rotation for the bone given by the bone_id
 */
quaternion IKSolver::getRotation(int bone_id) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			return B_DATA[i].B_ROT;
		}
	}
	return NULL;
}

/**
 * Attempt to generate a solution for the given end point
 */
void IKSolver::solveIK(G308_Point end) {

}

//Private Methods:
IK_Rotation* IKSolver::find_IK_Rotation(int bone_id) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			return B_DATA[i].B_ROT;
		}
	}
	return NULL;
}
/**
 * Take the rotation matrix and apply it to the position of the bone
 * given by bone id then save that updated position. Do this for all
 * the children of bone_id as well
 */
void IKSolver::applyRotation(GLfloat* matrix, int bone_id) {

}

/**
 * Find a rotation around start that brings the end point as close as possible
 * to the goal point
 */
void IKSolver::calculateRotation(G308_Point goal, G308_Point start, G308_Point end) {

}


