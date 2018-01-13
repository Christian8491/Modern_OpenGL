#ifndef MY_GLUT_H
#define MY_GLUT_H

#include "Common.h"

int my_glut_init(int argc, char **argv, const int width = WIDTH, const int height = HEIGHT) {

	glutInit(&argc, argv);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Terrain Rendering by Christian Cordova");

	/* Check if glew work */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cerr << "Glew do not work! " << endl;
		return EXIT_FAILURE;
	}
}



#endif // MY_GLUT_H