#define GLEW_STATIC
#include <iostream>
#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <fstream>
#include <sstream>
#include <ostream>
#include <string.h>
#include <cmath>

// Use the glm library
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

const int WINDOW_SIZE = 600;

float angle = 0, translate = 0;

GLuint prog, vert, frag;
GLuint progNorm, vertNorm, fragNorm;

GLuint progTexture, vertTexture, fragTexture;
GLuint buffersFirst[2], buffers[2];
GLuint firstFrameTexture, firstFrameFBO;

// Vertex data for a simple triangle
float vertices[] = { -1, 0.0f, 0, 1.0f, 1, 0.0f, 0, 1.0f, 0, 1, 0, 1.0f };
// Color data for that triangle
float colors[] = { 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f };

// Position data for the screen-aligned quad
float textureVertices[] = { -1, -1, 1, -1, 1, 1, -1, 1 };
GLuint vertexbuffer;

// Vaos
GLuint vao[3];

glm::mat4 projMatrix, modelMatrix;
glm::mat4 firstProjMatrix, firstModelMatrix;
// Prototypes
char *textFileRead(const char *fn);
void generateProgram(std::string vertPath, std::string fragPath,
		GLuint& vertexLocation, GLuint& fragmentLocation,
		GLuint& programLocation);
int computeShader(GLenum type, std::string path);
void display();
void displayFirstFrame();
void setupBuffers();
void perspective(float fovy, float aspect, float zNear, float zFar,
		glm::mat4 & out);
void lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY,
		float centerZ, float upX, float upY, float upZ, glm::mat4 & out);

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Testwarp");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);

	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Register programs
	generateProgram("shaders\\min.vert", "shaders\\min.frag", vert, frag, prog);
	generateProgram("shaders\\norm.vert", "shaders\\norm.frag", vertNorm,
			fragNorm, progNorm);
	setupBuffers();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(displayFirstFrame);

	glutMainLoop();
	return 0;
}

// Display function for the first frame
void displayFirstFrame() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);
	glUseProgram(prog);
	// Kinda ugly, but this is for demonstration purposes
	// Setup matrices
	lookAt(0, 0, -3, 0, 0, 0, 0, 1, 0, firstModelMatrix);
	perspective(45, 1, 0.1, 50, firstProjMatrix);
	lookAt(0, 0, -3, 0, 0, 0, 0, 1, 0, modelMatrix);
	perspective(45, 1, 0.1, 50, projMatrix);
	glUniformMatrix4fv(glGetUniformLocation(prog, "projMatrix"), 1, false,
			glm::value_ptr(projMatrix));
	glUniformMatrix4fv(glGetUniformLocation(prog, "viewMatrix"), 1, false,
			glm::value_ptr(modelMatrix));

	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glutSwapBuffers();

	// Switch displayfunc to the warping function
	glutDisplayFunc(display);
	glutIdleFunc(display);

}

void display() {
	angle += 0.05;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0, 1, 0));
	glm::mat4 trans = glm::translate(rot, glm::vec3(0, -0.5, 0));
	glm::mat4 look;
	lookAt(0, 0, -3, 0, 0, 0, 0, 1, 0, look);
	modelMatrix = look * trans;
	perspective(45, 1, 0.1, 50, projMatrix);
	glUseProgram(progNorm);
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "projMatrix"), 1, false,
			glm::value_ptr(projMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "viewMatrix"), 1, false,
			glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "oldProjMatrix"), 1,
			false, glm::value_ptr(firstProjMatrix));
	glUniformMatrix4fv(glGetUniformLocation(progNorm, "oldViewMatrix"), 1,
			false, glm::value_ptr(firstModelMatrix));
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glUniform1i(glGetUniformLocation(progNorm, "frameTex"), 0);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

// Convert text file into a char array. Terminate with zero to
// Follow cstring conventions
char *textFileRead(const char *fn) {

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
int computeShader(GLenum type, std::string path) {
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
	if (isCompiled_vs == FALSE) {
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);

		vertexInfoLog = new char[maxLength];

		std::cout << "Shader used: " << path << std::endl;
		glGetShaderInfoLog(v, maxLength, &maxLength, vertexInfoLog);
		std::cout << vertexInfoLog << std::endl;

	}

	return v;
}

// Method to generate a program with shader objects. Will throw errors, if any occured
void generateProgram(std::string vertPath, std::string fragPath,
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

void setupBuffers() {

	// Three VAOs for 3 triangles (could probably be more streamlined)
	glGenVertexArrays(3, vao);

	// First Triangle
	glBindVertexArray(vao[0]);
	glGenBuffers(2, buffersFirst);
	glBindBuffer(GL_ARRAY_BUFFER, buffersFirst[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(prog, "position"));
	glVertexAttribPointer(glGetAttribLocation(prog, "position"), 4, GL_FLOAT, 0,
			0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffersFirst[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(prog, "color"));
	glVertexAttribPointer(glGetAttribLocation(prog, "color"), 4, GL_FLOAT, 0, 0,
			0);

	// Second trianlge will be rendered using the first frame -> no color needed
	glBindVertexArray(vao[1]);
	glGenBuffers(1, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(progNorm, "position"));
	glVertexAttribPointer(glGetAttribLocation(progNorm, "position"), 4,
			GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);

	// In case a texture needs to be displayed -> Screenspace Quad
	glBindVertexArray(vao[2]);
	GLuint bufferTextureQuad[1];
	glGenBuffers(1, bufferTextureQuad);
	glBindBuffer(GL_ARRAY_BUFFER, bufferTextureQuad[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureVertices), textureVertices,
			GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(progTexture, "position"));
	glVertexAttribPointer(glGetAttribLocation(progTexture, "position"), 2,
			GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);

	// Texture for the framebuffer to hold the first frame
	glGenTextures(1, &firstFrameTexture);
	glBindTexture(GL_TEXTURE_2D, firstFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_SIZE, WINDOW_SIZE, 0,
			GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// FBO for first frame
	glGenFramebuffersEXT(1, &firstFrameFBO);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, firstFrameFBO);

	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	// Texture for the FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D, firstFrameFBO, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}
// Super simple convenience methods
void perspective(float fovy, float aspect, float zNear, float zFar,
		glm::mat4 & out) {

	out = glm::perspective(fovy, aspect, zNear, zFar);
}
void lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY,
		float centerZ, float upX, float upY, float upZ, glm::mat4 & out) {

	glm::vec3 eye(eyeX, eyeY, eyeZ);
	glm::vec3 center(centerX, centerY, centerZ);
	glm::vec3 up(upX, upY, upZ);

	out = glm::lookAt(eye, center, up);

}
