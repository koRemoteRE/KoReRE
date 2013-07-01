#ifndef SERIALIZABLE_IMAGE
#define SERIALIZABLE_IMAGE

#include <opencv2/core/core.hpp> 
//#include <boost/serialization/vector.hpp>

struct SerializableImage{
	unsigned int id;
	std::vector<uchar> image;

	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & id;
		ar & image;
	}

};

#endif