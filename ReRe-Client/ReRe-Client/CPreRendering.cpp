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

GLuint texturePosition0;
GLuint texturePosition1;
//GLuint texturePosition2;
//GLuint texturePosition3;
//GLuint texturePosition4;

GLuint diffImgFBO;
GLuint depthBuffer;
 
// TODO: Update Window size

// Window size
int width = 1024;
int height = 768;

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
    // Current DiffImg
    // Create texture and bind to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texturePosition0);
	glBindTexture(GL_TEXTURE_2D, texturePosition0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
    // Current DepthImg
	// Create texture and bind to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &texturePosition1);
	glBindTexture(GL_TEXTURE_2D, texturePosition1);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
    // Todo: Where to save the following images?
    /*
    // Sent DiffImg
	// Create texture and bind to texture unit 2
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &texturePosition2);
	glBindTexture(GL_TEXTURE_2D, texturePosition2);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
    // Sent DepthImg
	// Create texture and bind to texture unit 3
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &texturePosition3);
	glBindTexture(GL_TEXTURE_2D, texturePosition3);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
    // Latest ServerImg
	// Create texture and bind to texture unit 4
	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &texturePosition4);
	glBindTexture(GL_TEXTURE_2D, texturePosition4);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    */
	// Texture unit 0 should be active again
	glActiveTexture(GL_TEXTURE0);
}

void CPreRendering::createFBO()
{
    // --- Extensions can be removed when using OpenGL >= 3.0
    
    // Create FBO
    glGenFramebuffersEXT(1, &diffImgFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, diffImgFBO);
    
    // Attach textures to FBO
    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D,
        texturePosition0,
        0);

    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT1_EXT,
        GL_TEXTURE_2D,
        texturePosition1,
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
       
    // TODO: Draw DiffImg - Buffers[0]
    
    // TODO: Draw DepthImg - Buffers[1]
    
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
    
    // ----- Softwaretesting ----------------------------------------------------
    
    // Todo: Show images for testing
    
}