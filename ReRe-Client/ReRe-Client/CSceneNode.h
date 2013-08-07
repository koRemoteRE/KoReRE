//
//  SceneNode.h
//  ReRe-Client
//

#ifndef __ReRe_Client__SceneNode__
#define __ReRe_Client__SceneNode__

#include <iostream>
#include <vector>

#include "CTransformAiToGlm.h"

using namespace std;

// CSceneNode-Klasse
class CSceneNode
{
    // Variablen
private:
    vector<CSceneNode*> v_p_sceneNodeChildren;  // Liste aller Kinder eines Knotens
    glm::mat4* m_sceneNodeTransform;            // Transformation des Knoten
    unsigned int* ui_p_nodeMesh;                 // Index auf Mesh
    int i_nodeMeshNum;                        // Anzahl an Meshes
    
    // Methoden
public:
    CSceneNode(aiNode* ain_asNode);
    CSceneNode(aiNode* ain_asNode, glm::mat4 m_parnetTransform);
    ~CSceneNode();
    
    vector<CSceneNode*> returnChildren()
        { return v_p_sceneNodeChildren; };
    
    glm::mat4* returnModelMatrix()
        { return m_sceneNodeTransform; };
    
    unsigned int* returnMeshIndex()
        { return ui_p_nodeMesh; };
    
    int* returnNumberOfMesh()
        { return &i_nodeMeshNum; };
    
private:
    void findNextMeshNode(aiNode* ain_asNode, glm::mat4 m_parnetTransform);
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
    ~CLight();
    
    glm::vec3* returnPosition()
        { return v_p_lightPosition; };
    
    glm::vec3* returnDiffuse()
        { return v_p_lightDiffuse; };
};

#endif /* defined(__ReRe_Client__SceneNode__) */
