#include "ShaderTesterMain.h"
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

ShaderMain mainobj;

int main(int argc, char* argv[]) {

	mainobj.run(argc, argv);
	return 0;
}

void ShaderMain::initGL() {
	glewInit(); // enable advanced commands

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, width/(float) height, 0.1, 1000);

	// Camera enable 
	camera.setEye(Point3D(0,2,0));
	camera.setLookat(Point3D(1, 0, 0));

	// Shadow fix hack
	//glPolygonOffset(-1.0, 0.0);

	// Blue background
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClearStencil(0);

	// enabelings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);
	//glEnable(GL_RESCALE_NORMAL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glPolygonMode(GL_FRONT, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonOffset(-1.0f, 0.0f); //shadow fix
}

void ShaderMain::initGLUT(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA |GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Shader testing project");

	glutDisplayFunc(::display);
	glutSpecialFunc(::specialKeys);
	glutKeyboardFunc(::handleKeys);
	glutMouseFunc(::mouseButtonClick);
	glutMotionFunc(::mouseMove);
	glutReshapeFunc(::reshape);
}

void ShaderMain::initAssets() {
	shaders.initiate();
}


void ShaderMain::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	

	camera.setModelViewMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
}

void ShaderMain::mouseButtonClick(int button, int state, int x, int y) {

}

void ShaderMain::mouseMove(int x, int y) {

}

void ShaderMain::specialKeys(int key, int x, int y) {
	switch(key) {
	case GLUT_KEY_LEFT:
		;
	}
	glutPostRedisplay();
}

void ShaderMain::handleKeys(unsigned char key, int x, int y) {
	switch (key) {

	}
}

void ShaderMain::tick(int v) {

}

void ShaderMain::run(int argc, char* argv[]) {
	printInstructions();
	initGLUT(argc, argv);
	initGL();
	initAssets();
	
	glutMainLoop();
}

void ShaderMain::printInstructions() {
	cout<<""<<endl;

}