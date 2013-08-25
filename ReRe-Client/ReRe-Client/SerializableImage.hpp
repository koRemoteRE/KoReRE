#ifndef SERIALIZABLE_IMAGE
#define SERIALIZABLE_IMAGE

#include "SerializableMatrix.hpp"

#include <opencv2/core/core.hpp> 
#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <boost/lexical_cast.hpp>

struct SerializableImage{
	SerializableMatrix matrix;
	std::vector<uchar> *image;

	std::string outBuf;
	
	/*Serianlizes an Image + Matrix into a new String*/
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
	/*Serianlizes an Image + Matrix into a given String (recomended Method)*/
	void serializeInto(std::string &outBuf){
		outBuf += matrix.serialize() + "IMAGE";
		for (auto const &i: *image){
			try{
				outBuf += boost::lexical_cast<char>(i);
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableImage::serialize: Bad Lexical Cast" << std::endl;
			}
		}
	}
	/*Deserializes a String into this instance*/
	void deserialize(std::string &input){
		image = new std::vector<uchar>();

		int index = input.find("IMAGE");
		
        std::string inputBuffer = input.substr(0, index-1);
		matrix.deserialize(inputBuffer);
		std::string ss(input.substr(index + 5));
		for (
			auto i = boost::begin(ss);
			i != boost::end(ss);
			++i){
			try{
				image->push_back(boost::lexical_cast<uchar>(*i));
			}catch(boost::bad_lexical_cast const&){
				std::cerr << "SerializableImage::deserialize: Bad Lexical Cast" << std::endl;
			}
		}
	}
};

#endif