#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include <glm/glm.hpp>
/**
 *
 * 3--2
 * |  |
 * 0--1
 *
 */

glm::vec4 rectangleVertices[] = {
		glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f)
};

GLushort rectangleIndices[] = {
		0,1,2,
		0,2,3
};

glm::vec4 rectangleColor[] = {
		glm::vec4(0.8392f, 0.8392f, 0.8392f, 1.0f),
		glm::vec4(0.8392f, 0.8392f, 0.8392f, 1.0f),
		glm::vec4(0.8392f, 0.8392f, 0.8392f, 1.0f),
		glm::vec4(0.8392f, 0.8392f, 0.8392f, 1.0f)
};
glm::vec2 rectangle2DVertices[] = {
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
};


#endif /* RECTANGLE_H_ */
