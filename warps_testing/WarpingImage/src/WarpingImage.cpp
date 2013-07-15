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
#include "loadTexture.h"

const int WINDOW_HEIGHT = 768;
const int WINDOW_WIDTH = 1024;
const int TEXTURE_HEIGHT = 1024;
const int TEXTURE_WIDTH = 1024;

Util* utl;
MatrixState* matState;

GLuint prog, vert, frag;
GLuint progNorm, vertNorm, fragNorm;
GLuint progTxt, vertTxt, fragTxt;

float aspect = (float) (WINDOW_WIDTH) / WINDOW_HEIGHT;
float textureAspect = (float) (TEXTURE_WIDTH) / TEXTURE_HEIGHT;
GLuint progTexture, vertTexture, fragTexture;
GLuint boxBuffer[5], recBuffer[3], txtQuadBuffer[2];
GLuint firstFrameTexture, firstFrameFBO;
GLuint textureHandle;
GLuint textureHandle2;

GLuint vao[4];
glm::vec3 pos(0.0f, 0.0f, -15.0f);
float rotY = 0.0f;
float rotZ = 0.0f;

glm::mat4 projMatrix, modelMatrix;
glm::mat4 firstProjMatrix, firstModelMatrix;

long timeCounter = 0;
long timeSwitch = 2000;

bool mouseDown = false;

void displayFirstFrame();
void display();
void setupBuffers();
void keyAction(unsigned char key, int x, int y);
void mouseAction(int button, int state, int x, int y);
void mousePassive(int x, int y);

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Wapring Image");

	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
	glutKeyboardFunc(keyAction);
	glutMouseFunc(mouseAction);
	glutPassiveMotionFunc(mousePassive);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	utl = new Util();
	matState = new MatrixState();

	utl->generateProgram("shaders\\min.vert", "shaders\\min.frag", vert, frag,
			prog);
	utl->generateProgram("shaders\\texture.vert", "shaders\\texture.frag",
			vertTxt, fragTxt, progTxt);
	utl->generateProgram("shaders\\norm.vert", "shaders\\norm.frag", vertNorm,
			fragNorm, progNorm);

	setupBuffers();

	glutDisplayFunc(displayFirstFrame);
	//glutIdleFunc(displayFirstFrame);
	glutMainLoop();

	return 0;
}

void displayFirstFrame() {
	std::cout << "displayFirstFrame()" << std::endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);
	glViewport(0, 0, (GLsizei) TEXTURE_WIDTH, (GLsizei) TEXTURE_HEIGHT);
	matState->matrixMode(matState->PROJECTION);
	matState->loadIdentity();
	matState->perspective(45.0f, textureAspect, 0.1f, 50.0f);
	firstProjMatrix = matState->getCurrentMatrix();

	matState->matrixMode(matState->VIEW);
	matState->loadIdentity();
	matState->lookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	matState->rotate(rotY, 0.0f, 1.0f, 0.0f);
	matState->rotate(rotZ, 1.0f, 0.0f, 0.0f);
	matState->translate(pos.x, pos.y, pos.z);

	firstModelMatrix = matState->getCurrentMatrix();

	glUseProgram(prog);

//	matState->matrixMode(matState->MODEL);
//	matState->loadIdentity();
//	matState->translate(0.0f,-1.0f,0.0f);
//	matState->scale(100.0f,1.0f,100.0f);
//	matState->rotate(90.0f,1.0f,0.0f,0.0f);

	glUniformMatrix4fv(glGetUniformLocation(prog, "projMatrix"), 1, false,
			glm::value_ptr(firstProjMatrix));
	glUniformMatrix4fv(glGetUniformLocation(prog, "viewMatrix"), 1, false,
			glm::value_ptr(firstModelMatrix));

	glBindVertexArray(vao[0]);
	glDrawElements(GL_TRIANGLES, sizeof(rectangleIndices), GL_UNSIGNED_SHORT,
			0);

//	matState->matrixMode(matState->MODEL);
//	matState->loadIdentity();

//	matState->sendMatrices(prog);
	glUniformMatrix4fv(glGetUniformLocation(prog, "projMatrix"), 1, false,
			glm::value_ptr(firstProjMatrix));
	glUniformMatrix4fv(glGetUniformLocation(prog, "viewMatrix"), 1, false,
			glm::value_ptr(firstModelMatrix));
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glUniform1i(glGetUniformLocation(prog, "frameTex"), 0);

	glBindVertexArray(vao[1]);
	glDrawElements(GL_TRIANGLES, sizeof(boxIndices), GL_UNSIGNED_SHORT, 0);
	glDisable(GL_TEXTURE_2D);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glutSwapBuffers();

	glutDisplayFunc(display);
	glutIdleFunc(display);
	timeCounter = glutGet(GLUT_ELAPSED_TIME ) + timeSwitch;
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
	glViewport(0, 0, (GLsizei) WINDOW_WIDTH, (GLsizei) WINDOW_HEIGHT);
	matState->matrixMode(matState->PROJECTION);
	matState->loadIdentity();
	matState->perspective(45.0f, aspect, 0.1f, 50.0f);
	projMatrix = matState->getCurrentMatrix();

	matState->matrixMode(matState->VIEW);
	matState->loadIdentity();
	matState->lookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	matState->rotate(rotY, 0.0f, 1.0f, 0.0f);
	matState->rotate(rotZ, 1.0f, 0.0f, 0.0f);
	matState->translate(pos.x, pos.y, pos.z);

	modelMatrix = matState->getCurrentMatrix();

	glUseProgram(progNorm);
//	matState->sendMatrices(progNorm);
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "projMatrix"), 1, false,
			glm::value_ptr(projMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "viewMatrix"), 1, false,
			glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "oldProjMatrix"), 1,
			false, glm::value_ptr(firstProjMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "oldViewMatrix"), 1,
			false, glm::value_ptr(firstModelMatrix));

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glUniform1i(glGetUniformLocation(progNorm, "frameTex"), 0);
	glm::ivec2 dim(TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glUniform2iv(glGetUniformLocation(progNorm, "texDim"), 1,
			glm::value_ptr(dim));

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, textureHandle2);
	glUniform1i(glGetUniformLocation(progNorm, "objectTex"), 1);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindVertexArray(vao[3]);
	glDrawElements(GL_TRIANGLES, sizeof(boxIndices), GL_UNSIGNED_SHORT, 0);
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(progTxt);
//	glBindVertexArray(vao[2]);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
//	glUniform1i(glGetUniformLocation(progTxt, "frameTex"), 0);
//
//	glDrawElements(GL_TRIANGLES, sizeof(rectangle2DVertices), GL_UNSIGNED_SHORT,
//			0);
//	glUseProgram(0);
//	glutSwapBuffers();

	if (glutGet(GLUT_ELAPSED_TIME ) >= timeCounter) {
		glutDisplayFunc(displayFirstFrame);
		glutIdleFunc(displayFirstFrame);
		timeCounter = glutGet(GLUT_ELAPSED_TIME ) + timeSwitch;
		std::cout << "switching display func" << std::endl;
	}

}

void setupBuffers() {

	//Textur laden
	textureHandle = loadTexture("resources\\test.png");
	textureHandle2 = loadTexture("resources\\test.png");
	glGenVertexArrays(4, vao);

	// Boden
	glBindVertexArray(vao[0]);

	glGenBuffers(3, recBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, recBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, recBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices),
			rectangleIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "position"));
	glVertexAttribPointer(glGetAttribLocation(prog, "position"), 4, GL_FLOAT, 0,
			0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, recBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor,
			GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "color"));
	glVertexAttribPointer(glGetAttribLocation(prog, "color"), 4, GL_FLOAT, 0, 0,
			0);

	// Box
	glBindVertexArray(vao[1]);

	glGenBuffers(4, boxBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices,
			GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndices), boxIndices,
			GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "position"));
	glVertexAttribPointer(glGetAttribLocation(prog, "position"), 4, GL_FLOAT, 0,
			0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxColor), boxColor, GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "color"));
	glVertexAttribPointer(glGetAttribLocation(prog, "color"), 4, GL_FLOAT, 0, 0,
			0);

	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoordinates), uvCoordinates,
			GL_STATIC_DRAW);

	glEnableVertexAttribArray(glGetAttribLocation(prog, "uvAttr"));
	glVertexAttribPointer(glGetAttribLocation(prog, "uvAttr"), 2, GL_FLOAT, 0,
			0, 0);

	glBindVertexArray(0);

	// Screenspace Quad
	glBindVertexArray(vao[2]);

	glGenBuffers(2, txtQuadBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, txtQuadBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle2DVertices),
			rectangle2DVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txtQuadBuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices),
			rectangleIndices, GL_STATIC_DRAW);


	glEnableVertexAttribArray(glGetAttribLocation(progTxt, "position"));
	glVertexAttribPointer(glGetAttribLocation(progTxt, "position"), 2, GL_FLOAT,
			0, 0, 0);

	// Box
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, boxBuffer[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxBuffer[1]);

	glEnableVertexAttribArray(glGetAttribLocation(progNorm, "position"));
	glVertexAttribPointer(glGetAttribLocation(progNorm, "position"), 4,
			GL_FLOAT, 0, 0, 0);

	glEnableVertexAttribArray(glGetAttribLocation(progNorm, "uvAttr"));
		glVertexAttribPointer(glGetAttribLocation(progNorm, "uvAttr"), 2, GL_FLOAT, 0,
				0, 0);

	glBindVertexArray(0);

	// Texture for the framebuffer to hold the first frame
	glGenTextures(1, &firstFrameTexture);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
			GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// FBO for first frame
	glGenFramebuffersEXT(1, &firstFrameFBO);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	// Texture for the FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, firstFrameTexture, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void keyAction(unsigned char key, int x, int y) {
	float schritt = 0.3f;
	if (key == 'w') {
		pos += glm::vec3(0.0f, 0.0f, schritt);
	}
	if (key == 'a') {
		pos += glm::vec3(schritt, 0.0f, 0.0f);
	}
	if (key == 'd') {
		pos += glm::vec3(-schritt, 0.0f, 0.0f);
	}
	if (key == 's') {
		pos += glm::vec3(0.0f, 0.0f, -schritt);
	}
	if (key == 'r') {
		pos = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	if (key == 'q') {
		exit(1);
	}
}
int old_x, old_y;
bool firstMouse = true;

void mouseAction(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		mouseDown = true;
		old_x = x;
		old_y = y;
	} else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		mouseDown = false;
	}
}

void mousePassive(int x, int y) {
	if (firstMouse) {
		old_x = x;
		old_y = y;
		firstMouse = false;
	} else {
		float deltaX = x - old_x;
		float amount = (deltaX / WINDOW_HEIGHT) * 30.0f;
		rotY += amount;
		float deltaY = y - old_y;
		amount = (deltaY / WINDOW_HEIGHT) * 30.0f;
		rotZ += amount;
		old_x = x;
		old_y = y;
	}
}
