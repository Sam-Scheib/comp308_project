/*
 * IKSolver.h
 *
 *  Created on: 14/10/2012
 *      Author: francis
 */

#ifndef IKSOLVER_H_
#define IKSOLVER_H_

#include <GL/glut.h>
#include "define.h"
#include "G308_Skeleton.h"

struct IK_Rotation {
	quaternion B_ROT;
	quaternion B_ROT_END;
	G308_Point B_POS;
	int bone_id;
	IK_Rotation** children;
}IK_ROT;

class IKSolver {
private:
	//Variables:

	//Pointer to robot skeleton
	Skeleton* ROBOT;

	IK_Rotation* B_DATA;

	//number of bones in our robot
	int NUM_BONES;

	//Methods:

	//take a rotation matrix and the current bone id and update
	//all the children's locations with this rotation
	void applyRotation(GLfloat* matrix, int bone_id);

	//calculate a rotation around x/y/z at start that will bring end closest to goal
	void calculateRotation(G308_Point goal, G308_Point start, G308_Point end);

	//given a bone pointer, find the IK_Rotation that matches that bone's id
	IK_Rotation* find_IK_Rotation(int bone_id);

public:
	//construct a new ik solver from a skeleton and the skeletons bone array
	IKSolver(Skeleton *, bone*);
	virtual ~IKSolver();

	//return a quaternion that is the current angle of the bone given
	//by bone_id, this will also increase the rotation of that angle
	//for the next call
	quaternion getRotation(int bone_id);

	//attempt to generate a solution for this end goal
	void solveIK(G308_Point end);

};

#endif /* IKSOLVER_H_ */
