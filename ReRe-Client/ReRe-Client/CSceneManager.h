//
//  SceneManager.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__SceneManager__
#define __ReRe_Client__SceneManager__

#include "CSceneNode.h"
#include "CCamera.h"

using namespace std;

class CSceneManager
{
private:
    CSceneNode* sn_p_rootSceneNode;
    vector<CCamera*> l_cameraNode;
    vector<CLight*> l_lightNode;
    
public:
    CSceneManager();
    
    //
    CSceneNode* returnRootSceneNode();
    
    //
    CCamera* createCameraNode();
    void deleteCameraNode(int i_cameraNodeID = 0);
    CCamera* returnCameraNode(int i_cameraNodeID = 0);
    
    //
    CLight* createLightNode();
    void deleteLightNode(int i_lightNodeID = 0);
    CLight* returnLightNode(int i_lightNodeID = 0);
};

#endif /* defined(__ReRe_Client__SceneManager__) */
