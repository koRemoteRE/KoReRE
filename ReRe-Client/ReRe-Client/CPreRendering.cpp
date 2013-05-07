//
//  PreRendering.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CPreRendering.h"

// GLSL related variables
GLuint currentImgVertexShader;
GLuint currentImgFragmentShader;
GLuint currentImgShaderProgram;

GLuint diffImgTexture;
GLuint depthImgTexture;

GLuint currentImgFBO;
GLuint depthBuffer;

// Window size
int width = 1024;
int height = 768;

CPreRendering::CPreRendering()
{
    sceneMgr = new CSceneManager("cube.dae");
    
    // --- Just Testing! TODO: Delete!
    sceneMgr->returnRootSceneNode();
    // ---------------------------------------
    
    createTextures();
    createFBO();
    initGLSL();
}

CPreRendering::~CPreRendering()
{
    // Delete resources
    glDeleteTextures(1, &diffImgTexture);
    glDeleteTextures(1, &depthImgTexture);
    
    // Delete Framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &currentImgFBO);
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
    currentImgVertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    // Read vertex shader source
    string shaderSource = readFile("Shader/CDiffuseImage.vert");
    const char* sourcePtr = shaderSource.c_str();
    
    // Attach shader code
    glShaderSource(currentImgVertexShader, 1, &sourcePtr, NULL);
    
    // Compile
    glCompileShader(currentImgVertexShader);
    printShaderInfoLog(currentImgVertexShader);
    
    // Create empty shader object (fragment shader)
    currentImgFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read fragment shader source
    shaderSource = readFile("Shader/CDiffuseImage.frag");
    sourcePtr = shaderSource.c_str();
    
    // Attach shader code
    glShaderSource(currentImgFragmentShader, 1, &sourcePtr, NULL);
    
    // Compile
    glCompileShader(currentImgFragmentShader);
    printShaderInfoLog(currentImgFragmentShader);
    
    // Create shader program
    currentImgShaderProgram = glCreateProgram();
    
    // Attach shader
    glAttachShader(currentImgShaderProgram, currentImgVertexShader);
    glAttachShader(currentImgShaderProgram, currentImgFragmentShader);
    
    // TODO: Compare to VertexAttributePointers!!!
    glBindAttribLocation(currentImgShaderProgram, 0, "v_position");
    glBindAttribLocation(currentImgShaderProgram, 1, "v_normal");
    glBindAttribLocation(currentImgShaderProgram, 2, "v_texture");
    glBindAttribLocation(currentImgShaderProgram, 3, "v_lightPos");
    
    // Link program
    glLinkProgram(currentImgShaderProgram);
    printProgramInfoLog(currentImgShaderProgram);
}

void CPreRendering::createTextures()
{
    // Current DiffImg
    // Create texture and bind to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &diffImgTexture);
	glBindTexture(GL_TEXTURE_2D, diffImgTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    
    // Current DepthImg
	// Create texture and bind to texture unit 1
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &depthImgTexture);
	glBindTexture(GL_TEXTURE_2D, depthImgTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Texture unit 0 should be active again
	glActiveTexture(GL_TEXTURE0);
}

void CPreRendering::createFBO()
{
    // --- Remove extensions when using OpenGL >= 3.0
    
    // Create FBO
    glGenFramebuffersEXT(1, &currentImgFBO);
    // Bind it as the target for rendering and reading commands
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentImgFBO);
    
    // Attach textures to FBO
    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D,
        diffImgTexture,
        0);

    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthImgTexture,
        0);
    
    // Disable FBO for now
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CPreRendering::writeToFBO()
{
    // Bind framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentImgFBO);
    checkFrameBuffer();
    
    // Draw into textures
    GLenum buffers[2];
    buffers[0] = GL_COLOR_ATTACHMENT0_EXT;
    buffers[1] = GL_DEPTH_ATTACHMENT_EXT;
    glDrawBuffers(2, buffers);
    
    // Use FBO shader
    glUseProgram(currentImgShaderProgram);
    
    // TODO: Attach Uniform Variables
    
    // Clear content of FBO
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
    // TODO: Draw DiffImg - Buffers[0]
    // TODO: Draw DepthImg - Buffers[1]
    
    sceneMgr->drawScene();
    
    // Stop rendering to FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glUseProgram(0);
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
    
    // TODO: Softwaretesting 
    
}