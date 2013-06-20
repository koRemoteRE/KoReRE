//
//  PreRendering.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CPreRendering.h"


CPreRendering::CPreRendering()
{
    createTextures();
    createFBO();
    initGLSL();
    
    glEnable(GL_CULL_FACE);

    sceneMgr = new CSceneManager(PATH "city_all.dae");
}

CPreRendering::~CPreRendering()
{
    // Delete resources
    glDeleteTextures(1, &currentImgTexture);
    
    // Delete Framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &currentImgFBO);
}

GLuint CPreRendering::getCurrentDiffImg()
{
    return currentImgTexture;
}

CSceneManager* CPreRendering::getSceneGraph()
{
    return sceneMgr;
}

glm::mat4 CPreRendering::getProjectionMatrix()
{
    return sceneMgr->returnCameraNode()->returnProjectionMatrix();
}

glm::mat4 CPreRendering::getViewMatrix()
{
    return sceneMgr->returnCameraNode()->returnViewMatrix();
}

glm::mat4 CPreRendering::getCameraMatrix()
{
    return (sceneMgr->returnCameraNode()->returnProjectionMatrix() *
            sceneMgr->returnCameraNode()->returnViewMatrix());
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

void CPreRendering::createTextures()
{
    // Current DiffImg
    // Create texture and bind to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &currentImgTexture);
	glBindTexture(GL_TEXTURE_2D, currentImgTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0,
                 GL_RGBA, GL_FLOAT, NULL);
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
        currentImgTexture,
        0);
    
    // Attach depth buffer
	GLuint depthBuffer;
	glGenRenderbuffersEXT(1, &depthBuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
                             WIDTH, HEIGHT);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                 GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, 
                                 depthBuffer);

    // Disable FBO for now
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CPreRendering::writeToFBO()
{
    // Bind framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, currentImgFBO);
    checkFrameBuffer();

    // Draw into textures
    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0_EXT};
    glDrawBuffers(1, buffers);
    
    // Use FBO shader
    glUseProgram(currentImgShaderProgram);
    
    // Clear content of FBO
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
    // Draw Scene
    sceneMgr->drawScene(currentImgShaderProgram);
    
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
}

void CPreRendering::initGLSL()
{
    // Create shader which renders to the FBO
    
    // Create empty shader object (vertex shader)
    currentImgVertexShader = glCreateShader(GL_VERTEX_SHADER);
    
    string shaderSource;
    // Read vertex shader source
#ifdef __APPLE_CC__
    shaderSource = readFile("../Shader/CDiffuseImage.vert");
#else
    shaderSource = readFile("./Shader/CDiffuseImage.vert");
#endif
    const char* sourcePtr = shaderSource.c_str();
    
    // Attach shader code
    glShaderSource(currentImgVertexShader, 1, &sourcePtr, NULL);
    
    // Compile
    glCompileShader(currentImgVertexShader);
    printShaderInfoLog(currentImgVertexShader);
    
    // Create empty shader object (fragment shader)
    currentImgFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read fragment shader source
#ifdef __APPLE_CC__
    shaderSource = readFile("../Shader/CDiffuseImage.frag");
#else
    shaderSource = readFile("./Shader/CDiffuseImage.frag");
#endif
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
    
    // Bind Attributes
    glBindAttribLocation(currentImgShaderProgram, SHADER_POSITION_LOC, "v_position");
    glBindAttribLocation(currentImgShaderProgram, SHADER_NORMAL_LOC, "v_normal");
    //glBindAttribLocation(currentImgShaderProgram, SHADER_TEX_COORD_LOC, "v_texture");
    
    // Link program
    glLinkProgram(currentImgShaderProgram);
    printProgramInfoLog(currentImgShaderProgram);




    // Initialize warping shaders
    warpingVertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Read vertex shader source
#ifdef __APPLE_CC__
    shaderSource = readFile("../Shader/warping.vert");
#else
    shaderSource = readFile("./Shader/warping.vert");
#endif
    sourcePtr = shaderSource.c_str();

    // Attach shader code
    glShaderSource(warpingVertexShader, 1, &sourcePtr, NULL);

    // Compile
    glCompileShader(warpingVertexShader);
    printShaderInfoLog(warpingVertexShader);


    // Create empty shader object (fragment shader)
    warpingFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read fragment shader source
#ifdef __APPLE_CC__
    shaderSource = readFile("../Shader/warping.frag");
#else
    shaderSource = readFile("./Shader/warping.frag");
#endif
    sourcePtr = shaderSource.c_str();

    // Attach shader code
    glShaderSource(warpingFragmentShader, 1, &sourcePtr, NULL);

    // Compile
    glCompileShader(warpingFragmentShader);
    printShaderInfoLog(warpingFragmentShader);

    // Create shader program
    warpingShaderProgram = glCreateProgram();

    // Attach shader
    glAttachShader(warpingShaderProgram, warpingVertexShader);
    glAttachShader(warpingShaderProgram, warpingFragmentShader);

    // Bind Attributes
    glBindAttribLocation(warpingShaderProgram, SHADER_POSITION_LOC, "v_position");
    glBindAttribLocation(warpingShaderProgram, SHADER_NORMAL_LOC, "v_normal");
    //glBindAttribLocation(currentImgShaderProgram, SHADER_TEX_COORD_LOC, "v_texture");

    // Link program
    glLinkProgram(warpingShaderProgram);
    printProgramInfoLog(warpingShaderProgram);

}

// --- Just Testing ---- TODO: Delete! -------------------------------

void CPreRendering::testDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(currentImgShaderProgram);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    sceneMgr->drawScene(currentImgShaderProgram);
    
    glUseProgram(0);
}

void CPreRendering::testWarpDraw(glm::mat4 &oldView,glm::mat4 &oldProj)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(warpingShaderProgram);
    glm::ivec2 dim(WIDTH,HEIGHT);
    glUniformMatrix4fv(glGetUniformLocation(warpingShaderProgram, "oldProjMatrix"), 1,
                false, glm::value_ptr(oldProj));
        glUniformMatrix4fv(glGetUniformLocation(warpingShaderProgram, "oldViewMatrix"), 1,
                false, glm::value_ptr(oldView));
    glUniform2iv(glGetUniformLocation(warpingShaderProgram,"texDim"),1,glm::value_ptr(dim));
    GLuint texLoc = glGetUniformLocationARB(warpingShaderProgram, "frameTex");

    glActiveTextureARB(GL_TEXTURE6_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, currentImgTexture);
    glUniform1i(texLoc, 6);
    glActiveTextureARB(GL_TEXTURE3_ARB);
    sceneMgr->drawScene(warpingShaderProgram);
 glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

// -------------------------------------------------------------------
