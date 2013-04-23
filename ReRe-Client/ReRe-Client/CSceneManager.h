//
//  SceneManager.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__SceneManager__
#define __ReRe_Client__SceneManager__

#include <iostream>

#include "CSceneNode.h"
#include "CCamera.h"

class CSceneManager
{
public:
    CSceneManager();
    ~CSceneManager();
    
    CCamera* createCameraNode();
    CCamera* returnCameraNode();
    
    CSceneNode* createRootSceneNode();
    CSceneNode* returnRootSceneNode();
    
    CLight* createLight();
    CLight* returnLight();
};

#endif /* defined(__ReRe_Client__SceneManager__) */
