#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

class Shader {

public:
	Shader(char*, GLenum);
	GLuint name;

private:
};

class ShaderProgram {
public:
	ShaderProgram(const Shader&);
	void enable(void);
	void disable(void);
	GLint uniformLocation(char*);

private:
	GLint program;
};
