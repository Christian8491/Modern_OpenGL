/*
Properties:
1. C/C++ -> general -> directorios de inclusion adicionales ->
C:\openGL\glew-2.1.0-win32\glew-2.1.0\include;C:\openGL\glut\include;

2. Vinculador -> General -> directorios de biblioteca adicionales ->
C:\openGL\glut\lib;C:\openGL\glew-2.1.0-win32\glew-2.1.0\lib\Release\Win32;

3. Vinculador (Active - Debug) -> Entrada -> opengl32.lib; glut32.lib; glew32.lib;
*/

#ifndef COMMON_H
#define COMMON_H

#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <sstream>

/* Perlin Noise */
#include <cstdlib>
#include <ctime>
#include <cmath>

/* Bitmap_image */
#include <vector>

using namespace std;

#define WIDTH  600  //410
#define HEIGHT 500 //280
#define M_PI 3.14159265358979323846
#define L 10											// Number of layers
#define BUFFER_OFFSET(offset)  ((GLvoid*) (offset))		// Define offsets into buffer objects

const GLfloat  DivideByZeroTolerance = GLfloat(1.0e-07);

const GLfloat  DegreesToRadians = M_PI / 180.0;			// Degrees-to-radians constant 

#endif 
