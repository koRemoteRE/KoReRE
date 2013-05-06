#ifndef BOX_H_
#define BOX_H_

#include <glm/glm.hpp>
/**
 *   7---6
 *  /|  /|
 * 3---2 |
 * | 4-|-5
 * |/  |/
 * 0---1
 *
 */

glm::vec4 boxVertices[] = {
		glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),
		glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f)
};

GLushort boxIndices[] = {

		//Hinten
		5,4,7,
		5,7,6,
		//Oben
		3,2,6,
		3,6,7,
		//Unten
		0,4,5,
		0,5,1,
		//Rechts
		1,5,6,
		1,6,2,
		//Links
		4,0,3,
		4,3,7,
		//Vorne
		0,1,2,
		0,2,3
};

glm::vec4 boxColor[] = {
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
};

#endif /* BOX_H_ */
