//
//  PreRendering.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__PreRendering__
#define __ReRe_Client__PreRendering__

#include <GL/glew.h>
#include <iostream>
#include <string>
#include <fstream>

#include "CSceneManager.h"
#include "CRemoteNetwork.h"

using namespace std;

class CPreRendering
{
private:
    void printShaderInfoLog(GLuint shader);
    void printProgramInfoLog(GLuint program);
    string readFile(string fileName);
    
    void initGLSL();
    
    void createTextures();
    void createFBO();
    void writeToFBO();
    
    void checkFrameBuffer();
    
public:
    CPreRendering();
    ~CPreRendering();
};

#endif /* defined(__ReRe_Client__PreRendering__) */
