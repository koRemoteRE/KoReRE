#ifndef SERIALIZABLE_MATRIX
#define SERIALIZABLE_MATRIX

#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>


struct SerializableMatrix{
	glm::mat4 mat;

	std::string serialize(){
		std::string output = "";

		/*cv::Mat m2 = cv::Mat(4,4,CV_32F, glm::value_ptr(mat));
		std::cout << "Bevor: " << m2 << std::endl;*/

		for (unsigned int i = 0; i < 4; i++){
			for (unsigned int j = 0; j < 4; j++){
				try{
					output += boost::lexical_cast<std::string>(mat[i][j]);
				}catch(boost::bad_lexical_cast const&){
					std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
				}
				output += "&";
			}
		}
		//std::cout << "Output: " << output << std::endl;
		return output;
	}

	void serializeInto(std::string &outBuff){
		outBuff = "";
		for (unsigned int i = 0; i < 4; i++){
			for (unsigned int j = 0; j < 4; j++){
				try{
					outBuff += boost::lexical_cast<std::string>(mat[i][j]);
				}catch(boost::bad_lexical_cast const&){
					std::cerr << "SerializableMatrix::serialize: Bad Lexical Cast" << std::endl;
				}
				outBuff += "&";
			}
		}
	}

	void deserialize(std::string &input){
		std::string inString = input;

		//std::cout << "Input: " << inString << std::endl;

		std::vector<float> elems;
		std::stringstream ss(inString);
		std::string item;
		char delim = '&';
		while(std::getline(ss, item, delim)){
			try{
				elems.push_back(boost::lexical_cast<float>(item));
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableMatrix::deserialize: Bad Lexical Cast" << std::endl;
			}
		}
		mat = glm::mat4(
			elems.at(0), elems.at(1), elems.at(2), elems.at(3),
			elems.at(4), elems.at(5), elems.at(6), elems.at(7),
			elems.at(8), elems.at(9), elems.at(10), elems.at(11),
			elems.at(12), elems.at(13), elems.at(14), elems.at(15));
	}
};

#endif

