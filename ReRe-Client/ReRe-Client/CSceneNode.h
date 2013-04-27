//
//  SceneNode.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__SceneNode__
#define __ReRe_Client__SceneNode__

#include <iostream>
#include <vector>

using namespace std;

// CSceneNode-Klasse
class CSceneNode
{
private:
    int* i_p_nodeID;                          //Identifier des Knotens
    vector<CSceneNode*>* l_p_sceneNodeChildren; //Liste aller Kinder eines Knotens
    
public:
    CSceneNode();
    CSceneNode(int* i_p_sceneNodeID);
    
    CSceneNode* createChild();
    CSceneNode* returnChild();
};

//-----------------------------------------------
//-----------------------------------------------

// CLight-Klasse
class CLight
{
private:
    int* i_p_lightID;
    
public:
    CLight();
    CLight(int* i_p_lightNodeID);
    
    int* getID();
};

#endif /* defined(__ReRe_Client__SceneNode__) */
