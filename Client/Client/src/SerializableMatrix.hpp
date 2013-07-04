#ifndef SERIALIZABLE_MATRIX
#define SERIALIZABLE_MATRIX

#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <boost/lexical_cast.hpp>

struct SerializableMatrix{
	glm::mat4 mat;

	std::string serialize(){
		std::string output = "";
		for (unsigned int i = 0; i < 4; i++){
			for (unsigned int j = 0; j < 4; j++){
				output += boost::lexical_cast<std::string>(mat[i][j]);
				output += "&";
			}
		}
		return output;
	}

	void deserialize(std::string &input){
		std::vector<float> elems;
		std::stringstream ss(input);
		std::string item;
		char delim = '&';
		while(std::getline(ss, item, delim)){
			elems.push_back(boost::lexical_cast<float>(item));
		}
		mat = glm::mat4(
			elems.at(0), elems.at(1), elems.at(2), elems.at(3),
			elems.at(4), elems.at(5), elems.at(6), elems.at(7),
			elems.at(8), elems.at(9), elems.at(10), elems.at(11),
			elems.at(12), elems.at(13), elems.at(14), elems.at(15));
	}
};

#endif

