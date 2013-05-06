/*
 * MatrixState.cpp
 *
 *  Created on: 09.10.2012
 *      Author: Bastian
 */

#include "MatrixState.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>


MatrixState::MatrixState() :
		state(MODEL) {

	// Create stacks
	this->projectionStack = new std::vector<glm::mat4>();
	this->modelStack = new std::vector<glm::mat4>();
	this->textureStack = new std::vector<glm::mat4>();
	this->viewStack = new std::vector<glm::mat4>();

	// Push Identity Matrices
	this->projectionStack->push_back(glm::mat4(1.0));
	this->modelStack->push_back(glm::mat4(1.0));
	this->textureStack->push_back(glm::mat4(1.0));
	this->viewStack->push_back(glm::mat4(1.0));

}

MatrixState::~MatrixState() {

	// Free bound resources
	delete this->projectionStack;
	delete this->modelStack;
	delete this->viewStack;
	delete this->textureStack;
}

void MatrixState::pushMatrix() {

	// Will identify the stack to be used
	std::vector<glm::mat4>& stack = getCurrentStack();
	// Depending on the current state, choose appropriate stack

	// Copy the last matrix and put it on the stack
	stack.push_back(glm::mat4(stack.back()));
}

void MatrixState::popMatrix() {

	std::vector<glm::mat4>& stack = getCurrentStack();
	// Depending on the current state, choose appropriate stack

	// Copy the last matrix and put it on the stack
	stack.pop_back();
}

void MatrixState::lookAt(float eyeX, float eyeY, float eyeZ, float centerX,
		float centerY, float centerZ, float upX, float upY, float upZ) {

	glm::vec3 eye(eyeX, eyeY, eyeZ);
	glm::vec3 center(centerX, centerY, centerZ);
	glm::vec3 up(upX, upY, upZ);
	std::vector<glm::mat4>& stack = getCurrentStack();
	glm::mat4 m = stack.back();

	stack.back() = m * glm::lookAt(eye, center, up);

}

void MatrixState::perspective(float fovy, float aspect, float zNear,
		float zFar) {
	std::vector<glm::mat4>& stack = getCurrentStack();

	glm::mat4 m = glm::perspective(fovy, aspect, zNear, zFar);

	stack.back() = stack.back() * m;

}
glm::mat4 MatrixState::getCurrentMatrix() {
	return getCurrentStack().back();
}
void MatrixState::sendMatrices(GLuint prog) {

	GLuint modelMatrixLocation = glGetUniformLocationARB(prog, "modelMatrix");
	GLuint viewMatrixLocation = glGetUniformLocationARB(prog, "viewMatrix");
	GLuint projMatrixLocation = glGetUniformLocationARB(prog, "projMatrix");
	GLuint normalMatrixLocation = glGetUniformLocationARB(prog, "normalMatrix");
	GLuint textureMatrixLocation = glGetUniformLocationARB(prog, "textureMatrix");

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
			glm::value_ptr(this->modelStack->back()));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE,
			glm::value_ptr(this->viewStack->back()));
	glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE,
			glm::value_ptr(this->projectionStack->back()));
	glUniformMatrix4fv(textureMatrixLocation, 1, GL_FALSE,
			glm::value_ptr(this->textureStack->back()));
	glm::mat3 mNorm = glm::inverseTranspose(
			glm::mat3(this->viewStack->back() * this->modelStack->back()));
	glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE,
			glm::value_ptr(mNorm));

}

void MatrixState::rotate(float angle, float x, float y, float z) {
	glm::vec3 axis(x, y, z);
	std::vector<glm::mat4>& stack = getCurrentStack();

	stack.back() = glm::rotate(stack.back(), angle, axis);

}

void MatrixState::translate(float x, float y, float z) {
	glm::vec3 trans(x, y, z);
	std::vector<glm::mat4>& stack = getCurrentStack();

	stack.back() = glm::translate(stack.back(), trans);

}

void MatrixState::scale(float x, float y, float z) {
	glm::vec3 scale(x, y, z);
	std::vector<glm::mat4>& stack = getCurrentStack();

	stack.back() = glm::scale(stack.back(), scale);
}

void MatrixState::loadIdentity() {
	getCurrentStack().back() = glm::mat4(1.0);

}

void MatrixState::matrixMode(MatrixMode m) {
	this->state = m;
}

void MatrixState::resetStacks() {

	// Free bound resources
	delete this->projectionStack;
	delete this->modelStack;
	delete this->viewStack;
	delete this->textureStack;

	// Create stacks
	this->projectionStack = new std::vector<glm::mat4>();
	this->modelStack = new std::vector<glm::mat4>();
	this->textureStack = new std::vector<glm::mat4>();
	this->viewStack = new std::vector<glm::mat4>();

	// Push Identity Matrices
	this->projectionStack->push_back(glm::mat4(1.0));
	this->modelStack->push_back(glm::mat4(1.0));
	this->textureStack->push_back(glm::mat4(1.0));
	this->viewStack->push_back(glm::mat4(1.0));
}

std::vector<glm::mat4>& MatrixState::getCurrentStack() {

	switch (this->state) {
	case MODEL:
		return *this->modelStack;
		break;
	case VIEW:
		return *this->viewStack;
		break;
	case PROJECTION:
		return *this->projectionStack;
		break;
	case TEXTURE:
		return *this->textureStack;
		break;

	}
	// Just for completeness's sake
	return *this->modelStack;
}
