//
//  PreRendering.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CPreRendering.h"

// GLSL related variables
GLuint diffImgVertexShader;
GLuint diffImgFragmentShader;
GLuint diffImgShaderProgram;

GLuint texturePosition;
GLuint textureNormal;

GLuint diffImgFBO;
GLuint depthBuffer;

// TODO: Update Window size

// Window size
int width = 512;
int height = 512;

CPreRendering::CPreRendering()
{

}

// Print information about the compiling step
void CPreRendering::printShaderInfoLog(GLuint shader)
{
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;
    
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&infologLength);
	infoLog = (char *)malloc(infologLength);
	glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
	free(infoLog);
}

// Print information about the linking step
void CPreRendering::printProgramInfoLog(GLuint program)
{
	GLint infoLogLength = 0;
	GLsizei charsWritten  = 0;
	char *infoLog;
    
	glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infoLogLength);
	infoLog = (char *)malloc(infoLogLength);
	glGetProgramInfoLog(program, infoLogLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
	free(infoLog);
}

// Reads a file and returns the content as a string
string CPreRendering::readFile(string fileName)
{
	string fileContent;
	string line;
    
	ifstream file(fileName.c_str());
	if (file.is_open()) {
		while (!file.eof()){
			getline (file,line);
			line += "\n";
			fileContent += line;
		}
		file.close();
	}
	else
		cout << "ERROR: Unable to open file " << fileName << endl;
    
	return fileContent;
}

void CPreRendering::initGLSL()
{
    // Create shader which renders to the FBO
    
    // Create empty shader object (vertex shader)
    diffImgVertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    // Read vertex shader source
    string shaderSource = readFile("Shader/CDiffuseImage.vert");
    const char* sourcePtr = shaderSource.c_str();
    
    // Attach shader code
    glShaderSource(diffImgVertexShader, 1, &sourcePtr, NULL);
    
    // Compile
    glCompileShader(diffImgVertexShader);
    printShaderInfoLog(diffImgVertexShader);
    
    // Create empty shader object (fragment shader)
    diffImgFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read fragment shader source
    shaderSource = readFile("Shader/CDiffuseImage.frag");
    sourcePtr = shaderSource.c_str();
    
    // Attach shader code
    glShaderSource(diffImgFragmentShader, 1, &sourcePtr, NULL);
    
    // Compile
    glCompileShader(diffImgFragmentShader);
    printShaderInfoLog(diffImgFragmentShader);
    
    // Create shader program
    diffImgShaderProgram = glCreateProgram();
    
    // Attach shader
    glAttachShader(diffImgShaderProgram, diffImgVertexShader);
    glAttachShader(diffImgShaderProgram, diffImgFragmentShader);
    
    // Link program
    glLinkProgram(diffImgShaderProgram);
    printProgramInfoLog(diffImgShaderProgram);
}

void CPreRendering::createTextures()
{
    // TODO: How many textures? 2+3*i ( i = #savedServerImages)
    //       = currentImg + currentDepthImg +
    //         i * ( serverImage + properClientImg + properDepthImg )
    
    // Create texture and bind to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texturePosition);
	glBindTexture(GL_TEXTURE_2D, texturePosition);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
	// Create texture and bind to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &textureNormal);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
	// Texture unit 0 should be active again
	glActiveTexture(GL_TEXTURE0);
}

void CPreRendering::createFBO()
{
    // TODO: Extension needed?
    
    // Create FBO
    glGenFramebuffersEXT(1, &diffImgFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, diffImgFBO);
    
    // Attach textures to FBO
    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT
        GL_TEXTURE_2D,
        texturePosition,
        0);

    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT1_EXT
        GL_TEXTURE_2D,
        textureNormal,
        0);
    
    // Attach depth buffer
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbufferEXT(
        GL_FRAMEBUFFER_EXT,
        GL_DEPTH_ATTACHMENT_EXT,
        GL_RENDERBUFFER_EXT,
        depthBuffer);
    
    // Disable FBO for now
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CPreRendering::writeToFBO()
{
    // TODO: How to create and handle depth images?
    
    // Bind framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, diffImgFBO);
    checkFrameBuffer();
    
    // Draw into textures
    GLenum buffers[2];
    buffers[0] = GL_COLOR_ATTACHMENT0_EXT;
    buffers[1] = GL_COLOR_ATTACHMENT1_EXT;
    glDrawBuffers(2, buffers);
    
    // Use FBO shader
    glUseProgram(diffImgShaderProgram);
    
    // TODO: Attach Uniform Variables
    
    // Clear content of FBO
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // TODO: Draw
    
    // Stop rendering to FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CPreRendering::checkFrameBuffer()
{
	// Check wheter the current bound FBO ist OK or not
	// If not, display the cause of the error
	// See http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
	// for more information
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            cout << "GL_FRAMEBUFFER_UNSUPPORTED_EXT\n";	  
            break;
        default:
            cout << "Unknown ERROR\n";	  
	}
}