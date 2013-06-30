#ifndef SERIALIZABLE_MATRIX
#define SERIALIZABLE_MATRIX

#include <glm/glm.hpp>

struct SerializableMatrix{
	unsigned int id;

	glm::mat4 mat;

	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & id;

		ar & mat[0][0];
		ar & mat[0][1];
		ar & mat[0][2];
		ar & mat[0][3];

		ar & mat[1][0];
		ar & mat[1][1];
		ar & mat[1][2];
		ar & mat[1][3];

		ar & mat[2][0];
		ar & mat[2][1];
		ar & mat[2][2];
		ar & mat[2][3];

		ar & mat[3][0];
		ar & mat[3][1];
		ar & mat[3][2];
		ar & mat[3][3];
	}
};

#endif

