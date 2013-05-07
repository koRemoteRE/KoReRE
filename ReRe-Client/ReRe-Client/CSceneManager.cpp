//
//  SceneManager.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//
// VAO -> Lighthouse3d.com

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
        
        createLightNode();
        createCameraNode();
        
        aim_p_asMesh = ais_asScene->mMeshes;
        aim_p_asMaterial = ais_asScene->mMaterials;
        ait_p_mTexture = ais_asScene->mTextures;
        
        i_p_numMesh = new int;
        i_p_numMaterial = new int;
        i_p_numTexture = new int;
        
        *i_p_numMesh = ais_asScene->mNumMeshes;
        *i_p_numMaterial = ais_asScene->mNumMaterials;
        *i_p_numTexture = ais_asScene->mNumTextures;
        
        bindVAO();
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

void CSceneManager::drawScene()
{
    //TODO: Vertexliste zeichnen
    for (unsigned int numMesh = 0; numMesh < *sn_p_rootSceneNode->returnNumberOfMesh(); numMesh++)
    {
        glBindVertexArray(stm_meshList[sn_p_rootSceneNode->returnMeshIndex()[numMesh]].glui_vaoBuffer);
        glDrawElements(GL_TRIANGLES,stm_meshList[sn_p_rootSceneNode->returnMeshIndex()[numMesh]].glui_numFace*3, GL_UNSIGNED_INT, 0);
    }
}

//

CCamera* CSceneManager::returnCameraNode()
{
    return c_cameraNode;
}

//

CLight* CSceneManager::returnLightNode()
{
    return v_p_lightNode.back();
}

int* CSceneManager::returnLightNodeSize()
{
    return i_p_numLightNode;
}

//
void CSceneManager::bindVAO()
{
    //TODO: Vertexliste anlegen
    GLuint glui_vertexArrayObjBuffer;
    
    for (unsigned int ui_meshNum = 0; ui_meshNum < *i_p_numMesh; ui_meshNum++)
    {
        //
        st_meshVAO stm_meshVAO;
        stm_meshVAO.glui_numFace = aim_p_asMesh[ui_meshNum]->mNumFaces;
        
        unsigned int* ui_p_faceVertexArrayObjArray = (unsigned int*) malloc(sizeof(unsigned int) * stm_meshVAO.glui_numFace * 3);
        
        //
        for (unsigned int ui_faceNum = 0; ui_faceNum < stm_meshVAO.glui_numFace; ui_faceNum++)
        {
            memcpy(&ui_p_faceVertexArrayObjArray[ui_faceNum * 3], aim_p_asMesh[ui_meshNum]->mFaces[ui_faceNum].mIndices, sizeof(float) * 3);
        }
        
        //
        glGenVertexArrays(1,&(stm_meshVAO.glui_vaoBuffer));
        glBindVertexArray(stm_meshVAO.glui_vaoBuffer);
        
        //
        glGenBuffers(1, &glui_vertexArrayObjBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * stm_meshVAO.glui_numFace * 3, ui_p_faceVertexArrayObjArray, GL_STATIC_DRAW);
        
        //
        if (aim_p_asMesh[ui_meshNum]->HasPositions())
        {
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 3, aim_p_asMesh[ui_meshNum]->mVertices, GL_STATIC_DRAW);
            //glEnableVertexAttribArray(vertexLoc);
            //glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, 0, 0, 0);
        }
        
        //
        if (aim_p_asMesh[ui_meshNum]->HasNormals())
        {
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            
        }
        
        //Texturen
        if (aim_p_asMesh[ui_meshNum]->HasTextureCoords(0))
        {
            float* f_p_textureCoord = (float*) malloc(sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2);
            for (unsigned int ui_textureNum = 0; ui_textureNum < aim_p_asMesh[ui_meshNum]->mNumVertices; ui_textureNum++)
            {
                f_p_textureCoord[ui_textureNum*2] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].x;
                f_p_textureCoord[ui_textureNum*2+1] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].y;
            }
            
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2, f_p_textureCoord, GL_STATIC_DRAW);
            //glEnableVertexAttribArray(texCoordLoc);
            //glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, 0, 0, 0);
        }
        
        //Material??
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        
        stm_meshList.push_back(stm_meshVAO);
    }
}

CCamera* CSceneManager::createCameraNode()
{
    c_cameraNode = new CCamera();    
    return c_cameraNode;
}

CLight* CSceneManager::createLightNode()
{
    for (int i_numLightSource = 0; i_numLightSource < ais_asScene->mNumLights; i_numLightSource++)
    {
        v_p_lightNode.push_back(new CLight(ais_asScene->mLights[i_numLightSource]));
    }
    return v_p_lightNode.back();
}

