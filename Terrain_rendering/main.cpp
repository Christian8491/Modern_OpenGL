#include "Common.h"
#include "My_glut.h"
#include "Shader.h"
#include "Background.h"
#include "Camera.h"
#include "Color.h"
#include "PerlinNoise.h"
#include "Clipmap.h"
#include "Texture.h"
#include "bitmap_image.h"

/* Buffers and program */
GLuint bufferMap1, bufferMap2, bufferMap3, bufferMap4, bufferMap5, bufferMap6;
GLuint clipmapProgram, textureProgram;

/* Shaders */
Shader* clipmapShader, *textureShader;

/* Clipmap */
Clipmap clipmap1, clipmap2, clipmap3, clipmap4, clipmap5;

/* Texture */
GLuint texture;

/* Projection transformation parameters */
GLfloat fovy = 45.0;										// Field-of-view in Y direction angle (in degrees)
GLfloat aspect = (GLfloat)WIDTH / (GLfloat)HEIGHT;			// Viewport aspect ratio
GLfloat zNear = 0.5, zFar = 100.0;
GLfloat angle = 0.0;										// rotation angle in degrees

/* Camera */
Camera camera;

int a = N_COORD * N_COORD * N_TRIANG * 2 * N_TRIANG;		// 3 * 3 * 32 * 2 * 32;

void init() {

	/* Clear window */
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.529f, 0.807f, 0.92f, 0.0f);

	/* Init Shaders */
	clipmapShader = new Shader("shaders/clipmap_vshader.glsl", "shaders/clipmap_fshader.glsl");
	clipmapProgram = clipmapShader->use_program();

	textureShader = new Shader("shaders/texture_vshader.glsl", "shaders/texture_fshader.glsl");
	textureProgram = textureShader->use_program();

	camera.initialize();

	clipmap1.initialize(1);
	clipmap1.readCoordinates();
	clipmap1.generateHeigths();
	clipmap1.perlinCoordinates();
	clipmap1.generateCenterTriangulation();

	clipmap2.initialize(2);
	clipmap2.readCoordinates();
	clipmap2.generateHeigths();
	clipmap2.perlinCoordinates();
	clipmap2.generateSidesTriangulation();

	clipmap3.initialize(3);
	clipmap3.readCoordinates();
	clipmap3.generateHeigths();
	clipmap3.perlinCoordinates();
	clipmap3.generateSidesTriangulation();

	clipmap4.initialize(4);
	clipmap4.readCoordinates();
	clipmap4.generateHeigths();
	clipmap4.perlinCoordinates();
	clipmap4.generateSidesTriangulation();

	clipmap5.initialize(5);
	clipmap5.readCoordinates();
	clipmap5.generateHeigths();
	clipmap5.perlinCoordinates();
	clipmap5.generateSidesTriangulation();

	/* Buffers */
	glGenBuffers(1, &bufferMap1);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMap1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_1) + sizeof(texCoords), NULL, GL_STATIC_DRAW); // COMENT
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(background_1), background_1); // COMMENT
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(background_1), sizeof(texCoords), texCoords); // COMMENT
	//glBufferData(GL_ARRAY_BUFFER, sizeof(clipmap1.triangulationCoord) * a, clipmap1.triangulationCoord, GL_STATIC_DRAW); // UNCOMENT

	glGenBuffers(1, &bufferMap2);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMap2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(clipmap2.triangulationCoord) * a, clipmap2.triangulationCoord, GL_STATIC_DRAW);

	glGenBuffers(1, &bufferMap3);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMap3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(clipmap3.triangulationCoord) * a, clipmap3.triangulationCoord, GL_STATIC_DRAW);

	glGenBuffers(1, &bufferMap4);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMap4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(clipmap4.triangulationCoord) * a, clipmap4.triangulationCoord, GL_STATIC_DRAW);

	glGenBuffers(1, &bufferMap5);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMap5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(clipmap5.triangulationCoord) * a, clipmap5.triangulationCoord, GL_STATIC_DRAW);

	/* Load an image and apply Textures */
	bitmap_image image_vec("textures/container.bmp");
	unsigned char* image_arr = image_vec.data_vector_to_array();

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)image_vec.width_, (int)image_vec.height_, 0, GL_RGB, GL_UNSIGNED_BYTE, image_arr);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void drawBackgrounds(Shader* &shader, GLuint& buffer, GLuint& program, int& num_vertices, vec3& color, bool line = false) {

	if (line == true) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2.0);
	}
	else  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/* Get and set: Vertex shader 'in' variables */
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	shader->use();

	/* Vertex Shader */
	mat4 view, projection, model;
	view = LookAt(camera.position, camera.front, camera.up) * view;
	projection = Perspective(fovy, (GLfloat)WIDTH / (GLfloat)HEIGHT, zNear, zFar);

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);

	/* Fragment Shader */
	shader->setVec3("backgroundColor", color);

	/*--- Draw and disable each vertex attribute array being enabled ---*/
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	glDisableVertexAttribArray(vPosition);
}

void drawTexture(Shader* &shader, GLuint& buffer, GLuint& program, int& num_vertices) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/* Get and set: Vertex shader 'in' variables */
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(background_1)));

	shader->use();

	/* Vertex Shader */
	mat4 view, projection, model;
	view = LookAt(camera.position, camera.front, camera.up) * view;
	projection = Perspective(fovy, (GLfloat)WIDTH / (GLfloat)HEIGHT, zNear, zFar);

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);

	/*--- Draw and disable each vertex attribute array being enabled ---*/
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vTexCoord);

}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Draw terrain */
	drawTexture(textureShader, bufferMap1, textureProgram, numVer);  // to texture
	//drawBackgrounds(clipmapShader, bufferMap1, clipmapProgram, a, grayColor3); // UNCOMENT
	drawBackgrounds(clipmapShader, bufferMap2, clipmapProgram, a, someRed3);
	drawBackgrounds(clipmapShader, bufferMap3, clipmapProgram, a, someGreen3);
	drawBackgrounds(clipmapShader, bufferMap4, clipmapProgram, a, someBlue3);
	drawBackgrounds(clipmapShader, bufferMap5, clipmapProgram, a, someYellow3);

	/* Draw triangulation */
	//drawBackgrounds(clipmapShader, bufferMap1, clipmapProgram, a, grayDarkColor3, true); // UNCOMENT
	drawBackgrounds(clipmapShader, bufferMap2, clipmapProgram, a, grayDarkColor3, true);
	drawBackgrounds(clipmapShader, bufferMap3, clipmapProgram, a, grayDarkColor3, true);
	drawBackgrounds(clipmapShader, bufferMap4, clipmapProgram, a, grayDarkColor3, true);
	drawBackgrounds(clipmapShader, bufferMap5, clipmapProgram, a, grayDarkColor3, true);

	glutSwapBuffers();
}

void idle(void) {
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {

	case 'q': case 'Q': case 033:
		exit(EXIT_SUCCESS); break;

	case 'X': camera.position.x += 0.5; break;
	case 'x': camera.position.x -= 0.5; break;
	case 'Y': camera.position.y += 0.5; break;
	case 'y': camera.position.y -= 0.5; break;
	case 'Z': camera.position.z += 0.5; break;
	case 'z': camera.position.z -= 0.5; break;

	}

	glutPostRedisplay();
}


int main(int argc, char **argv) {
	/* function to generate coordinates to triangulate
	ofstream outputFile("clipmap_data/triangulization_6.txt", ofstream::out);

	double x = -51.2;
	double constant = 51.2;
	double y = 51.2;
	double offset = 3.2;
	int cont = 0;
	for (int i = 0; i < 33; i++) {
	for (int j = 0; j < 33; j++) {
	outputFile << x;
	outputFile << " 0.0 ";
	outputFile << y;
	outputFile << "\n";
	x += offset;
	}
	x = -51.2;
	constant -= offset;
	y = constant;
	}
	outputFile.close();
	*/

	/* Init glut and check if glew works */
	my_glut_init(argc, argv, WIDTH, HEIGHT);

	init();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	//glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}