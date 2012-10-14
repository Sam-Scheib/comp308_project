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
	int num_children;
	IK_Rotation** children;
	IK_Rotation* parent;
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
	//it and all it's children's positions by rotating them around the rot point
	//by the rotation matrix, stopping if it reaches the goal point
	void applyRotation(GLfloat* matrix, IK_Rotation* bone_data, G308_Point rot_point);

	//calculate a rotation around x/y/z at rot_point that will bring end closest to goal
	float calculateRotation(G308_Point goal, G308_Point rot_point, G308_Point end);

	//given a bone pointer, find the IK_Rotation that matches that bone's id
	IK_Rotation* find_IK_Rotation(int bone_id);

	//return a vector representing teh vector p rotated by matrix matrix
	G308_Point getRotatedVector(G308_Point p, float* matrix);

public:
	//construct a new ik solver from a skeleton and the skeletons bone array
	IKSolver(Skeleton *, bone*);
	virtual ~IKSolver();

	//return a quaternion that is the current angle of the bone given
	//by bone_id, this will also increase the rotation of that angle
	//for the next call
	int getRotation(int bone_id, quaternion* q);

	//attempt to generate a solution for this end goal and effector pair
	void solveIK(G308_Point Goal, bone* end_effector);

};

#endif /* IKSOLVER_H_ */
