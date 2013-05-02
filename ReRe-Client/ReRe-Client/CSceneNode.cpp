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

CSceneNode::CSceneNode(aiNode* ain_asNode)
{
    // Für Root
    // Überprüfen, ob Mesh vorhanden
    if (ain_asNode->mNumMeshes != 0)
    {
        // Mesh speichern
        i_p_nodeMesh = ain_asNode->mMeshes;
        // Anzahl der Meshes speichern
        i_nodeMeshNum = ain_asNode->mNumMeshes;
    }
    
    // Transformation im Objekt speichern
    aiMatrix4x4 m_transBuffer_AI_GLM = ain_asNode->mTransformation;
    m_sceneNodeTransform = new glm::mat4x4(m_transBuffer_AI_GLM.a1, m_transBuffer_AI_GLM.a2, m_transBuffer_AI_GLM.a3, m_transBuffer_AI_GLM.a4,
                                           m_transBuffer_AI_GLM.b1, m_transBuffer_AI_GLM.b2, m_transBuffer_AI_GLM.b3, m_transBuffer_AI_GLM.b4,
                                           m_transBuffer_AI_GLM.c1, m_transBuffer_AI_GLM.c2, m_transBuffer_AI_GLM.c3, m_transBuffer_AI_GLM.c4,
                                           m_transBuffer_AI_GLM.d1, m_transBuffer_AI_GLM.d2, m_transBuffer_AI_GLM.d3, m_transBuffer_AI_GLM.d4);
    
    findNextMeshNode(ain_asNode, *m_sceneNodeTransform);
}

CSceneNode::CSceneNode(aiNode* ain_asNode, CSceneNode* sn_parentNode, glm::mat4x4 m_parnetTransform)
{
    // Mesh speichern
    i_p_nodeMesh = ain_asNode->mMeshes;
    // Anzahl der Meshes speichern
    i_nodeMeshNum = ain_asNode->mNumMeshes;
    
    // Transformation im Objekt speichern
    aiMatrix4x4 m_transBuffer_AI_GLM = ain_asNode->mTransformation;
    m_sceneNodeTransform = new glm::mat4x4(m_transBuffer_AI_GLM.a1, m_transBuffer_AI_GLM.a2, m_transBuffer_AI_GLM.a3, m_transBuffer_AI_GLM.a4,
                                           m_transBuffer_AI_GLM.b1, m_transBuffer_AI_GLM.b2, m_transBuffer_AI_GLM.b3, m_transBuffer_AI_GLM.b4,
                                           m_transBuffer_AI_GLM.c1, m_transBuffer_AI_GLM.c2, m_transBuffer_AI_GLM.c3, m_transBuffer_AI_GLM.c4,
                                           m_transBuffer_AI_GLM.d1, m_transBuffer_AI_GLM.d2, m_transBuffer_AI_GLM.d3, m_transBuffer_AI_GLM.d4);
    
    findNextMeshNode(ain_asNode, *m_sceneNodeTransform);
}

void CSceneNode::findNextMeshNode(aiNode* ain_asNode, glm::mat4x4 m_nextTransform)
{
    // Überprüfen wie viele Kinder es gibt
    // Schleife über alle Kinder
    for(int i_childNum = 0; i_childNum < ain_asNode->mNumChildren; i_childNum++)
    {
        // Überprüfen, ob Mesh in Kind vorhanden
        // Wenn ja: Neuen SceneNode anlegen und (Assimp-)Kind mitgeben
        if (ain_asNode->mChildren[i_childNum]->mNumMeshes > 0)
            sn_p_sceneNodeChildren[i_childNum] = CSceneNode(ain_asNode->mChildren[i_childNum], this, *m_sceneNodeTransform);
        
        // Wenn Nein: Merke Transformation und gehe Unterkinder durch
        else
        {
            aiMatrix4x4 m_transBuffer_AI_GLM = ain_asNode->mTransformation;
            m_nextTransform += glm::mat4x4 (m_transBuffer_AI_GLM.a1, m_transBuffer_AI_GLM.a2, m_transBuffer_AI_GLM.a3, m_transBuffer_AI_GLM.a4,
                                            m_transBuffer_AI_GLM.b1, m_transBuffer_AI_GLM.b2, m_transBuffer_AI_GLM.b3, m_transBuffer_AI_GLM.b4,
                                            m_transBuffer_AI_GLM.c1, m_transBuffer_AI_GLM.c2, m_transBuffer_AI_GLM.c3, m_transBuffer_AI_GLM.c4,
                                            m_transBuffer_AI_GLM.d1, m_transBuffer_AI_GLM.d2, m_transBuffer_AI_GLM.d3, m_transBuffer_AI_GLM.d4);
            findNextMeshNode(ain_asNode->mChildren[i_childNum], m_nextTransform);
        }
    }
}

CSceneNode* CSceneNode::returnChildren()
{
    return sn_p_sceneNodeChildren;
}

unsigned int* CSceneNode::returnMeshIndex()
{
    return i_p_nodeMesh;
}

int* CSceneNode::returnMeshNum()
{
    return &i_nodeMeshNum;
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