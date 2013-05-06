#include <stdio.h>
#include <string.h>

#include "Util.h"

Util::Util() {
}

Util::~Util() {
}

// Convert text file into a char array. Terminate with zero to
// Follow cstring conventions
char* Util::textFileRead(const char *fn) {

	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *) malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

// Creates an OpenGL shader object of the specified type, with the
// source's location specified by path
int Util::computeShader(GLenum type, std::string path) {
	char *vs;
	GLuint v;
	v = glCreateShaderObjectARB(type);
	vs = textFileRead(path.c_str());
	const char *vv = vs;
	glShaderSourceARB(v, 1, &vv, NULL);
	free(vs);

	glCompileShaderARB(v);
	int isCompiled_vs;
	int maxLength;
	char* vertexInfoLog;
	glGetShaderiv(v, GL_COMPILE_STATUS, &isCompiled_vs);
	if (isCompiled_vs == false) {
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);

		vertexInfoLog = new char[maxLength];

		std::cout << "Shader used: " << path << std::endl;
		glGetShaderInfoLog(v, maxLength, &maxLength, vertexInfoLog);
		std::cout << vertexInfoLog << std::endl;

	}

	return v;
}

void Util::generateProgram(std::string vertPath, std::string fragPath,
		GLuint& vertexLocation, GLuint& fragmentLocation,
		GLuint& programLocation) {

	fragmentLocation = computeShader(GL_FRAGMENT_SHADER_ARB, fragPath);

	vertexLocation = computeShader(GL_VERTEX_SHADER_ARB, vertPath);
	GLuint p;

	p = glCreateProgramObjectARB();

	glAttachShader(p, fragmentLocation);
	glAttachShader(p, vertexLocation);
	glLinkProgramARB(p);

	programLocation = p;
}
