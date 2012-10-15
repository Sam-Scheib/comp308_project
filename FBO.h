/**
 *
 * Fluid simulation module
 *
 * by Sam Scheib
 *
 */

#include <GL/glut.h>

class FBO {
private:
	GLuint fbo;
public:
	int size;
	GLuint texture;
	void bind();
	void unbind();
	void glInit();
};
