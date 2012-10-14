/*
 * IKSolver.cpp
 *
 *  Created on: 14/10/2012
 *      Author: francis
 */

#include "IKSolver.h"

IKSolver::IKSolver(Skeleton* robot, bone* root) {
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
			IK_Rotation* child;
			find_IK_Rotation(current_bone.children[j]->id, child);
			B_DATA[i].children[j] = child;
			child->parent = &B_DATA[i];
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
int IKSolver::getRotation(int bone_id, quaternion* q) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			q =  &B_DATA[i].B_ROT;
			return 1;
		}
	}
	return NULL;
}

/**
 * Attempt to generate a solution for the given end point
 */
void IKSolver::solveIK(G308_Point goal, bone* end_effector) {
	//
	IK_Rotation* bone_data;
	find_IK_Rotation(end_effector->id, bone_data);
	IK_Rotation* cur_rot_point = bone_data;
	quaternion angle;
	//if our cur_rot_point has a parent
	while(cur_rot_point->parent!=NULL) {
		//set the cur_rot_point to the parent
		cur_rot_point = cur_rot_point->parent;
		//find an angle of rotationg around cur_rot_points position that brings
		//end effector closest to our goal
		angle = calculateRotation(goal, cur_rot_point->B_POS, bone_data->B_POS);
		GLfloat m[16];
		angle.toMatrix(m);
		//apply this rotation around cur_rot_point to cur_rot_point and all of the
		//children of cur_rot_point
		applyRotation(m, cur_rot_point, cur_rot_point->B_POS);
		//check how far end_effector is from our goal
		float dist = vector_length(subtract(goal, bone_data->B_POS));
	}
}

//Private Methods:
int IKSolver::find_IK_Rotation(int bone_id, IK_Rotation* result) {
	for (int i = 0; i<NUM_BONES; i++) {
		if (B_DATA[i].bone_id == bone_id) {
			result = &B_DATA[i];
			return 1;
		}
	}
	return NULL;
}
/**
 * take a rotation matrix and the current bone id and update
 * it and all it's children's positions by rotating them around the rot point
 * by the rotation matrix
 */
void IKSolver::applyRotation(GLfloat* matrix, IK_Rotation* bone_data, G308_Point rot_point) {
	G308_Point pos = bone_data->B_POS;
	//find the vector line between pos and rot_point
	G308_Point line = {pos.x-rot_point.x, pos.y - rot_point.y, pos.z - rot_point.z};
	//apply rotation to the line
	line = getRotatedVector(line, matrix);
	//find new pos
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
quaternion IKSolver::calculateRotation(G308_Point goal, G308_Point rot_point, G308_Point end) {
	return quaternion(0, 0, 0, 0);
}

/**
 * Return a vector that represents the vector p rotated by
 * the rotation matrix matrix.
 */
G308_Point IKSolver::getRotatedVector(G308_Point p, float* matrix) {

	G308_Point newPoint = {
			p.x * matrix[0] + p.y * matrix[4] + p.z * matrix[8] + matrix[12],
			p.x * matrix[1] + p.y * matrix[5] + p.z * matrix[9] + matrix[13],
			p.x * matrix[2] + p.y * matrix[6] + p.z * matrix[10] + matrix[14]
	};
	return newPoint;
}

