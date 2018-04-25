#ifndef SHADERTESTERMAIN_H
#define SHADERTESTERMAIN_H


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Camera.h"
#include "ShaderSystem.h"

/** Basic framework for shadertesting */
class ShaderMain {
	/** Shader system */
	ShaderSystem shaders;

	/** Camera object */
	Camera camera;

	/** width of window */
	int width;
	/** height of window */
	int height;


	/** ID of texture */
	GLuint textureID;
	/** ID of texture buffer */
	GLuint textureBufferID;

	/** initializes the glut system */
	void initGLUT(int argc, char* argv[]);

	/** Initializes the openGL system */
	void initGL();

	/** Initializes assets */
	void initAssets();

public:
	/** run the program */
	void run(int argc, char* argv[]);

	/** Prints the instructions in a consolle window */
	void printInstructions();

	// Functions called by callback
	/** Mouse function that handles gui draging */
	void mouseButtonClick(int button, int state, int x, int y);

	/** Mouse function that handles gui draging */
	void mouseMove(int x, int y);

	/** Handles specialkeys, used for camera controll */
	void specialKeys(int key, int x, int y);

	/** Handles + and -, used for camera controll */
	void handleKeys(unsigned char key, int x, int y);

	/** Display function. Should only be called by callback */
	void display();

	/** Timer function, updates animation. Should only be called by callback */
	void tick(int v);

	/** reshape function. Should only be called by callback */
	void reshape(int w, int h) {
		glViewport(0, h-height, width, height); 
	}

	/** Default values */
	ShaderMain() : width(800), height(800) {;}
};

extern ShaderMain mainobj;

// Callback functions calling object functions 
/** Mouse function that handles gui sellecting */
void mouseButtonClick(int button, int state, int x, int y) {
	mainobj.mouseButtonClick(button, state, x, y);
}

/** Mouse function that handles gui draging */
void mouseMove(int x, int y) {
	mainobj.mouseMove(x, y);
}

/** Handles specialkeys, used for camera controll */
void specialKeys(int key, int x, int y) {
	mainobj.specialKeys(key, x, y);
}

/** Handles + and -, used for camera controll */
void handleKeys(unsigned char key, int x, int y) {
	mainobj.handleKeys(key, x, y);
}

/** Display function. Should only be called by callback */
void display() {
	mainobj.display();
}

/** reshape function. Should only be called by callback */
void reshape(int w, int h) {
	mainobj.reshape(w, h);
}

/** Timer function. Should onlyh be called by callback */
void tick(int v) {
	mainobj.tick(v);
}

#endif