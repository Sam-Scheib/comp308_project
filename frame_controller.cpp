/*
 * frame_controller.cpp
 *
 *  Created on: 10/09/2012
 *      Author: francis
 */

#include "frame_controller.h"
#include <stdio.h>

FrameController::FrameController(int maxFrames) {
	animationState = 2;
	currentFrame = 0;
	maxFrame = maxFrames;
}
void FrameController::play() {
	animationState = 1;
}

void FrameController::pause() {
	animationState = 2;
}

void FrameController::stop() {
	animationState = 3;
}

void FrameController::reverse() {
	animationState = 4;
}

void FrameController::fast_forward() {
	animationState = 5;
}

int FrameController::current_frame() {
	return currentFrame;
}
int FrameController::next_frame() {
	switch(animationState) {
	//normal play
	case 1:
		currentFrame = (currentFrame + 1)%maxFrame;
		return currentFrame;

	//pause
	case 2:
		return currentFrame;

	//stop
	case 3:
		return 0;

	//reverse
	case 4:
		currentFrame = (currentFrame+1)/(maxFrame-1);
		return currentFrame;

	//double speed
	case 5:
		currentFrame = (currentFrame+2)*maxFrame;
		return currentFrame;

	//something has gone wrong!
	default:
		printf("Unrecognized animation state in controller:%d\n", animationState);
		return 0;
	}
}




