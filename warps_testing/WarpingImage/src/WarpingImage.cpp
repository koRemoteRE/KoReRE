#define GLEW_STATIC

#include <GL/glew.h>
#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "MatrixState.h"
#include "Util.h"
#include "Box.h"
#include "Rectangle.h"

const int WINDOW_HEIGHT = 1024;
const int WINDOW_WIDTH = 768;

Util* utl;
MatrixState* matState;

GLuint prog, vert, frag;
GLuint progNorm, vertNorm, fragNorm;
GLuint progTxt, vertTxt, fragTxt;

GLuint progTexture, vertTexture, fragTexture;
GLuint boxBuffer[3], recBuffer[3], txtQuadBuffer[2];
GLuint firstFrameTexture, firstFrameFBO;

GLuint vao[3];

glm::vec3 pos(0.0f,0.0f, 0.0f);
float rot = 0.0f;


void displayFirstFrame();
void display();
void setupBuffers();
void keyAction(unsigned char key, int x, int y);

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_HEIGHT, WINDOW_WIDTH);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Wapring Image");

	glutKeyboardFunc(keyAction);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	utl = new Util();
	matState = new MatrixState();

	utl->generateProgram("shaders\\min.vert", "shaders\\min.frag", vert, frag,
			prog);
	utl->generateProgram("shaders\\texture.vert", "shaders\\texture.frag", vertTxt, fragTxt,
				progTxt);
	utl->generateProgram("shaders\\norm.vert", "shaders\\norm.frag", vertNorm,
			fragNorm, progNorm);

	setupBuffers();

	glutDisplayFunc(displayFirstFrame);
	//glutIdleFunc(displayFirstFrame);
	glutMainLoop();

	return 0;
}

void displayFirstFrame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);

	matState->matrixMode(matState->PROJECTION);
	matState->loadIdentity();
	matState->perspective(45.0f, 1.0f, 0.1f, 50.0f);

	matState->matrixMode(matState->VIEW);
	matState->loadIdentity();
	matState->lookAt(0.0f,0.0f,15.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	matState->translate(pos.x,pos.y,pos.z);
	matState->rotate(rot,0.0f,1.0f,0.0f);

	glUseProgram(prog);

	matState->matrixMode(matState->MODEL);
	matState->loadIdentity();
	matState->translate(0.0f,-1.0f,0.0f);
	matState->scale(100.0f,1.0f,100.0f);
	matState->rotate(90.0f,1.0f,0.0f,0.0f);


	matState->sendMatrices(prog);

	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES,sizeof(rectangleIndices), GL_UNSIGNED_SHORT, 0);



	glUseProgram(prog);

	matState->matrixMode(matState->MODEL);
	matState->loadIdentity();

	matState->sendMatrices(prog);

	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES,sizeof(boxIndices), GL_UNSIGNED_SHORT, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glutSwapBuffers();


	glutDisplayFunc(display);
	glutIdleFunc(display);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(progTxt);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glUniform1i(glGetUniformLocation(progTxt, "frameTex"), 0);

	glBindVertexArray(vao[2]);
	glDrawElements(GL_TRIANGLES,sizeof(rectangleIndices), GL_UNSIGNED_SHORT, 0);

	glutSwapBuffers();
}


void setupBuffers() {

	glGenVertexArrays(3, vao);

	// Boden
	glBindVertexArray(vao[0]);

	glGenBuffers(3, recBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, recBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "position"));
	glVertexAttribPointer(glGetAttribLocation(prog, "position"), 4, GL_FLOAT, 0, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, recBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "color"));
	glVertexAttribPointer(glGetAttribLocation(prog, "color"), 4, GL_FLOAT, 0, 0, 0);


	// Box
	glBindVertexArray(vao[1]);

	glGenBuffers(3, boxBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), boxIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "position"));
	glVertexAttribPointer(glGetAttribLocation(prog, "position"), 4, GL_FLOAT, 0,
			0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxColor), boxColor, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "color"));
	glVertexAttribPointer(glGetAttribLocation(prog, "color"), 4, GL_FLOAT, 0, 0,
				0);

	glBindVertexArray(0);

	// Screenspace Quad
	glBindVertexArray(vao[2]);

	glGenBuffers(2,txtQuadBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, txtQuadBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txtQuadBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(progTexture, "position"));
	glVertexAttribPointer(glGetAttribLocation(progTexture, "position"), 4, GL_FLOAT, 0, 0, 0);
	glBindVertexArray(0);

	// Texture for the framebuffer to hold the first frame
	glGenTextures(1, &firstFrameTexture);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
			GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// FBO for first frame
	glGenFramebuffersEXT(1, &firstFrameFBO);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	// Texture for the FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, firstFrameFBO, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void keyAction(unsigned char key, int x, int y){
	switch (key) {
		case 'w':
			pos += glm::vec3(0.0f,0.0f,1.0f);
			break;
		case 'a':
			pos += glm::vec3(1.0f,0.0f,0.0f);
			break;
		case 'd':
			pos += glm::vec3(-1.0f,0.0f,0.0f);
			break;
		case 's':
			pos += glm::vec3(0.0f,0.0f, -1.0f);
			break;
		case 'r':
			pos = glm::vec3(0.0f,0.0f,0.0f);
			break;
		case 'q':
			exit(1);
		default:
			break;
	}
}
