#ifndef UTIL_H_
#define UTIL_H_

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <string.h>

class Util {
public:
	Util();
	virtual ~Util();
	void generateProgram(std::string vertPath, std::string fragPath,
			GLuint& vertexLocation, GLuint& fragmentLocation,
			GLuint& programLocation);
	char *textFileRead(const char *fn);
private:
	int computeShader(GLenum type, std::string path);
};

#endif /* UTIL_H_ */
