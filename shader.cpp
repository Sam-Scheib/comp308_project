
#include <GL/glew.h>
#include "shader.h"

Shader::Shader(char* source, GLenum shaderType) {
	int status;
	name = glCreateShader(shaderType);
	glShaderSource(name, 1, (const GLchar**)&source, 0);
	glCompileShader(name);
	glGetShaderiv(name, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		printf("something bad went wrong!!!!\n");
	}
}

ShaderProgram::ShaderProgram(const Shader &s) {
	int status;
	program = glCreateProgram();
	glAttachShader(program, s.name);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		printf("something else bad went wrong!!!!\n");
	}
}

void ShaderProgram::enable() {
	glUseProgram(program);
}

void ShaderProgram::disable() {
	glUseProgram(0);
}

GLint ShaderProgram::uniformLocation(char* name) {
	return glGetUniformLocation(program, name);
}
