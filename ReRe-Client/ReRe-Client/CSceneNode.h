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

#include "scene.h"
#include "glm.hpp"


using namespace std;

// CSceneNode-Klasse
class CSceneNode
{
private:
    CSceneNode* sn_p_sceneNodeChildren;     // Liste aller Kinder eines Knotens
    glm::mat4x4* m_sceneNodeTransform;      // Transformation des Knoten
    unsigned int* i_p_nodeMesh;             // Index auf Mesh
    int i_nodeMeshNum;                      // Anzahl an Meshes
    
public:
    CSceneNode();
    CSceneNode(aiNode* ain_asNode);
    CSceneNode(aiNode* ain_asNode, CSceneNode* sn_parentNode, glm::mat4x4 m_parnetTransform);
    
    void findNextMeshNode(aiNode* ain_asNode, glm::mat4x4 m_parnetTransform);
    CSceneNode* returnChildren();
    unsigned int* returnMeshIndex();
    int* returnMeshNum();
};








//-----------------------------------------------
//-----------------------------------------------

// CLight-Klasse
class CLight
{
private:
    glm::vec3* v_p_lightPosition;
    glm::vec3* v_p_lightDiffuse;
    
public:
    CLight();
    CLight(aiLight ail_asLight);
    
    glm::vec3* getPosition();
    glm::vec3* getDiffuse();
};

#endif /* defined(__ReRe_Client__SceneNode__) */
