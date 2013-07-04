#ifndef SERIALIZABLE_IMAGE
#define SERIALIZABLE_IMAGE

#include "SerializableMatrix.hpp"

#include <opencv2/core/core.hpp> 
#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <boost/lexical_cast.hpp>
//#include <boost/serialization/vector.hpp>

struct SerializableImage{
	//unsigned int id;
	SerializableMatrix matrix;
	std::vector<uchar> *image;

	std::string outBuf;
	
	std::string *serialize(){
		outBuf = matrix.serialize() + "IMAGE";
		for (auto const &i: *image){
			try{
				outBuf += boost::lexical_cast<char>(i);
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableImage::serialize: Bad Lexical Cast" << std::endl;
			}
		}

		return &outBuf;
	}

	void deserialize(std::string &input){
		
		image = new std::vector<uchar>;

		int index = input.find("IMAGE");
		
		matrix.deserialize(input.substr(0, index-1));

		std::string ss(input.substr(index + 5));
		for(size_t i = 0; i < ss.size(); i++){
			try{
				image->push_back(boost::lexical_cast<uchar>(ss[i]));
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableImage::deserialize: Bad Lexical Cast" << std::endl;
			}
		}
	}
};

#endif