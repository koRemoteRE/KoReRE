//
//  SceneNode.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//
// Knotensortierung(Pseudocode): assimp.sourceforge.net

#include "CSceneNode.h"

CSceneNode::CSceneNode(aiNode* ain_asNode)
{
    // Für Root
    // Überprüfen, ob Mesh vorhanden
    if (ain_asNode->mNumMeshes != 0)
    {
        // Anzahl der Meshes speichern
        i_nodeMeshNum = ain_asNode->mNumMeshes;
        
        // Mesh speichern
        ui_p_nodeMesh = new unsigned int[i_nodeMeshNum];
        for (int i_mesh = 0; i_mesh< i_nodeMeshNum; i_mesh++)
            ui_p_nodeMesh[i_mesh] = ain_asNode->mMeshes[i_mesh];
    }
    
    // Transformation im Objekt speichern
    m_sceneNodeTransform = CTransformAiToGlm::TransformMat4P(ain_asNode->mTransformation);
    
    findNextMeshNode(ain_asNode, *m_sceneNodeTransform);
}

CSceneNode::CSceneNode(aiNode* ain_asNode, glm::mat4 m_parnetTransform)
{
    // Anzahl der Meshes speichern
    i_nodeMeshNum = ain_asNode->mNumMeshes;
    
    // Mesh speichern
    ui_p_nodeMesh = new unsigned int[i_nodeMeshNum];
    for (int i_mesh = 0; i_mesh< i_nodeMeshNum; i_mesh++)
        ui_p_nodeMesh[i_mesh] = ain_asNode->mMeshes[i_mesh];
    
    // Transformation im Objekt speichern
    m_sceneNodeTransform = CTransformAiToGlm::TransformMat4P(ain_asNode->mTransformation);
    
    findNextMeshNode(ain_asNode, *m_sceneNodeTransform);
}

void CSceneNode::findNextMeshNode(aiNode* ain_asNode, glm::mat4 m_nextTransform)
{
    // Überprüfen wie viele Kinder es gibt
    // Schleife über alle Kinder
    for(int i_childNum = 0; i_childNum < ain_asNode->mNumChildren; i_childNum++)
    {
        // Überprüfen, ob Mesh in Kind vorhanden
        // Wenn ja: Neuen SceneNode anlegen und (Assimp-)Kind mitgeben
        if (ain_asNode->mChildren[i_childNum]->mNumMeshes > 0)
            v_p_sceneNodeChildren.push_back(new CSceneNode(ain_asNode->mChildren[i_childNum], *m_sceneNodeTransform));
        
        // Wenn Nein: Merke Transformation und gehe Unterkinder durch
        else
        {
            m_nextTransform += CTransformAiToGlm::TransformMat4(ain_asNode->mTransformation);
            findNextMeshNode(ain_asNode->mChildren[i_childNum], m_nextTransform);
        }
    }
}




//-----------------------------------------------
//-----------------------------------------------

CLight::CLight(aiLight* ail_asLight, aiMatrix4x4* aim_nodeTransform)
{
    // Speichern des diffusen Lichts
    v_p_lightDiffuse = CTransformAiToGlm::TransformCol3P(ail_asLight->mColorDiffuse);
    
    // Speichern der Lichtposition (ohne Richtung)
    v_p_lightPosition = CTransformAiToGlm::TransformVec3P(ail_asLight->mPosition);
    v_p_lightPosition->x += aim_nodeTransform->a4;
    v_p_lightPosition->y += aim_nodeTransform->b4;
    v_p_lightPosition->z += aim_nodeTransform->c4;
}