/*
 * MatrixStates.h
 *
 *  Created on: 09.10.2012
 *      Author: Bastian
 */

#ifndef MATRIXSTATES_H_
#define MATRIXSTATES_H_
#include <vector>
#include <glm/glm.hpp>
#include <windows.h>
#include <GL/glut.h>

class MatrixState {

public:
	enum MatrixMode {
		MODEL, VIEW, PROJECTION, TEXTURE
	};
	MatrixState();
	virtual ~MatrixState();

	void pushMatrix();

	void popMatrix();

	void lookAt(float eyeX, float eyeY, float eyeZ, float centerX,
			float centerY, float centerZ, float upX, float upY, float upZ);

	void perspective(float fovy, float aspect, float zNear, float zFar);

	void sendMatrices(GLuint prog);

	void rotate(float angle, float x, float y, float z);

	void translate(float x, float y, float z);

	void scale(float x, float y, float z);

	void loadIdentity();

	void matrixMode(MatrixMode m);

	void resetStacks();

	glm::mat4 getCurrentMatrix();

private:

	std::vector<glm::mat4>& getCurrentStack();

	std::vector<glm::mat4>* projectionStack;
	std::vector<glm::mat4>* viewStack;
	std::vector<glm::mat4>* modelStack;
	std::vector<glm::mat4>* textureStack;
	MatrixMode state;
};

#endif /* MATRIXSTATES_H_ */
