//
//  SceneManager.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CSceneManager.h"

CSceneManager::CSceneManager(std::string st_filename)
{
    if (st_filename.substr(st_filename.find_last_of('.')) == ".dae")
    {
    
        ais_asScene = imp_asImporter.ReadFile(st_filename,  aiProcess_CalcTangentSpace |
                                                            aiProcess_Triangulate |
                                                            aiProcess_JoinIdenticalVertices |
                                                            aiProcess_SortByPType);
        sn_p_rootSceneNode = new CSceneNode(ais_asScene->mRootNode);
    }
    else
    {
        std::cout << ".dae-Datei benoetigt" << std::endl;
    }
}

CSceneNode* CSceneManager::returnRootSceneNode()
{
    return sn_p_rootSceneNode;
}

CCamera* CSceneManager::createCameraNode()
{
    c_cameraNode = new CCamera();
    return c_cameraNode;
}

void CSceneManager::deleteCameraNode()
{
    delete c_cameraNode;
}

CCamera* CSceneManager::returnCameraNode()
{
    return c_cameraNode;
}

//
CLight* CSceneManager::createLightNode()
{
    v_lightNode.push_back(new CLight());
    return v_lightNode.back();
}


CLight* CSceneManager::returnLightNode()
{
    return v_lightNode.at(0);
}

int CSceneManager::returnLightNodeSize()
{
    return (int)v_lightNode.size();
}

vector<CLight*>* CSceneManager::returnLightVector()
{
    return &v_lightNode;
}

