//
//  SceneManager.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CSceneManager.h"

CSceneManager::CSceneManager()
{
    sn_p_rootSceneNode = new CSceneNode();
    ais_asScene = imp_asImporter.ReadFile("XXX", aiProcess_SortByPType);
}

CSceneNode* CSceneManager::returnRootSceneNode()
{
    return sn_p_rootSceneNode;
}

CCamera* CSceneManager::createCameraNode()
{
    l_cameraNode.push_back(new CCamera());
    return l_cameraNode.back();
}

void CSceneManager::deleteCameraNode(int i_cameraNodeID)
{
    
}

CCamera* CSceneManager::returnCameraNode(int i_cameraNodeID)
{
    return NULL;
}

//
CLight* CSceneManager::createLightNode()
{
    l_lightNode.push_back(new CLight(new int()));
    return l_lightNode.back();
}

void CSceneManager::deleteLightNode(int i_lightNodeID)
{
    
}

CLight* CSceneManager::returnLightNode(int i_lightNodeID)
{
    return NULL;
}
