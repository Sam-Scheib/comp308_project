/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */

#include <GL/glew.h>
#include "FBO.h"
#include <stdio.h>

void FBO::glInit() {
	GLint status;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	GLuint depthBuf;
//	glGenRenderbuffers(1, &depthBuf);
//	glBindRenderbuffer(GL_RENDERBUFFER_EXT, depthBuf);
//	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size, size);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	glFramebufferRenderbufferEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glReadBuffer(GL_NONE);
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("bad\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
