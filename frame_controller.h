/*
 * frame_controller.h
 *
 *  Created on: 10/09/2012
 *      Author: francis
 */

#ifndef FRAME_CONTROLLER_H
#define FRAME_CONTROLLER_H

class FrameController {
private:
	int animationState, currentFrame, maxFrame;

public:
	FrameController(int maxFrames);
	void play();
	void pause();
	void stop();
	void reverse();
	void fast_forward();
	int next_frame();
	int current_frame();
};



#endif /* FRAME_CONTROLLER_H_ */
