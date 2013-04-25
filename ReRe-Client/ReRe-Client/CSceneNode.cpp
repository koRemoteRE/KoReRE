//
//  SceneNode.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CSceneNode.h"

CSceneNode::CSceneNode()
{

}

CSceneNode::CSceneNode(int* i_p_sceneNodeID)
{
    i_p_nodeID = i_p_sceneNodeID;
}

//-----------------------------------------------
//-----------------------------------------------

CLight::CLight()
{

}

CLight::CLight(int* i_p_lightNodeID)
{
    i_p_lightID = i_p_lightNodeID;
}