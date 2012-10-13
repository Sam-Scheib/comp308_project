/*
 * OctTree.h
 *
 *  Created on: Oct 13, 2012
 *      Author: whyteferg
 */

#ifndef OCTTREE_H_
#define OCTTREE_H_

namespace std {

class OctTree {
public:
	const int MAX_TREE_DEPTH = 6;
	const int MIN_OBJ_PERTREE = 3;
	const int MAX_OBJ_PERTREE = 6;
	G308_Point LowerLeftCorner;
	G308_Point UpperRightcorner;
	G308_Point center;
	G308_Point faceNormal;
	bool childrenpresent;
	bool surfaceNode;

	OctTree *children[2][2][2];


	void add();
	void renderTree();
	OctTree();
	virtual ~OctTree();



};

} /* namespace std */
#endif /* OCTTREE_H_ */
