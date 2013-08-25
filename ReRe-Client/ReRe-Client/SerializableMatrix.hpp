#ifndef SERIALIZABLE_MATRIX
#define SERIALIZABLE_MATRIX

#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <boost/lexical_cast.hpp>

struct SerializableMatrix{
	double sendingTime;
	glm::mat4 matrix;

	/*Serializes this Matrix into a new String and returns it*/
	std::string serialize(){
		std::string output = "";
		try{
			output += boost::lexical_cast<std::string>(sendingTime);
		}catch(boost::bad_lexical_cast const&){
			std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
		}
		output += "MAT";
		for (unsigned int i = 0; i < 4; i++){
			for (unsigned int j = 0; j < 4; j++){
				try{
					output += boost::lexical_cast<std::string>(matrix[i][j]);
				}catch(boost::bad_lexical_cast const&){
					std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
				}
				output += "&";
			}
		}
		return output;
	}
	/*Serializes this Matrix into a given String (recomended Method)*/
	void serializeInto(std::string &outBuff){
		outBuff = "";
		try{
			outBuff += boost::lexical_cast<std::string>(sendingTime);
		}catch(boost::bad_lexical_cast const&){
			std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
		}
		outBuff += "MAT";
		for (unsigned int i = 0; i < 4; i++){
			for (unsigned int j = 0; j < 4; j++){
				try{
					outBuff += boost::lexical_cast<std::string>(matrix[i][j]);
				}catch(boost::bad_lexical_cast const&){
					std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
				}
				outBuff += "&";
			}
		}
	}
	/*Deserializes a String into this instance*/
	void deserialize(std::string &input){
		std::string inString = input;

		int index = inString.find("MAT");
		std::string timestampString = inString.substr(0, index-1);
		try{
			sendingTime = boost::lexical_cast<double>(timestampString);
		}catch(boost::bad_lexical_cast const&){
			std::cerr << "SerializableMatrix::deserialize: Bad Lexical Cast" << std::endl;
		}

		std::vector<float> elems;
		std::stringstream ss(inString.substr(index+3));
		std::string item;
		char delim = '&';
		while(std::getline(ss, item, delim)){
			try{
				elems.push_back(boost::lexical_cast<float>(item));
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableMatrix::deserialize: Bad Lexical Cast" << std::endl;
			}
		}
		matrix = glm::mat4(
			elems.at(0), elems.at(1), elems.at(2), elems.at(3),
			elems.at(4), elems.at(5), elems.at(6), elems.at(7),
			elems.at(8), elems.at(9), elems.at(10), elems.at(11),
			elems.at(12), elems.at(13), elems.at(14), elems.at(15));
	}
};

#endif

