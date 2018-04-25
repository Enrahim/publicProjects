#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <string.h>

#include <GL/glew.h>
#include <Gl/glut.h>

#pragma comment (lib, "glew32.lib")

class Shader
{
	GLuint vertex, fragment;
	GLuint program;

	int printOglError(char *file, int line);
	void printShaderInfoLog(GLuint obj);
	void printProgramInfoLog(GLuint obj);
	char *textFileRead(char *fn); 
public :
	Shader();
	~Shader();
	GLuint setShader(char *sp);
};

#endif