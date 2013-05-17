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

void CSceneManager::drawScene(GLuint glui_shaderProgram)
{
    bindUniform(glui_shaderProgram);
    drawScene(sn_p_rootSceneNode, glui_shaderProgram);
}

void CSceneManager::drawScene(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram)
{
    // Vertexliste zeichnen
    // Für alle Knoten zeichnen
    bindUniformModelMatrix(sn_p_drawNode, glui_shaderProgram);
    
    bindUniformModelMatrix(sn_p_drawNode, glui_shaderProgram);
    
    for (unsigned int ui_numMesh = 0; ui_numMesh <  *(sn_p_drawNode->returnNumberOfMesh()); ui_numMesh++)
    {
        //glBindTexture(GL_TEXTURE_2D, stm_meshList[sn_p_drawNode->returnMeshIndex()[ui_numMesh]].glui_textureIndex);
        glBindVertexArray(stm_meshList[sn_p_drawNode->returnMeshIndex()[ui_numMesh]].glui_vaoBuffer);
        glDrawElements(GL_TRIANGLES,stm_meshList[sn_p_drawNode->returnMeshIndex()[ui_numMesh]].glui_numFace*3, GL_UNSIGNED_INT, 0);
    }
    
    //Rekursiv alle Kinderknoten aufrufen und zeichnen
    for (unsigned int ui_numNode = 0; ui_numNode < sn_p_drawNode->returnChildren().size(); ui_numNode++)
    {
        drawScene(sn_p_drawNode->returnChildren()[ui_numNode], glui_shaderProgram);
    }
}

//
void CSceneManager::bindVAO()
{
    //TODO: Vertexliste anlegen
    GLuint glui_vertexArrayObjBuffer;
    
    for (unsigned int ui_meshNum = 0; ui_meshNum < *i_p_numMesh; ui_meshNum++)
    {
        cout << "Create VAO..." << endl;
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
            cout << "Bind Positions..." << endl;
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 3, aim_p_asMesh[ui_meshNum]->mVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_POSITION_LOC);
            glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, 0, 0, 0);
        }
        
        //
        if (aim_p_asMesh[ui_meshNum]->HasNormals())
        {
            cout << "Bind Normals..." << endl;
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 3, aim_p_asMesh[ui_meshNum]->mNormals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_NORMAL_LOC);
            glVertexAttribPointer(SHADER_NORMAL_LOC, 3, GL_FLOAT, 0, 0, 0);
        }
        
        //Texturen
        if (aim_p_asMesh[ui_meshNum]->HasTextureCoords(0))
        {
            cout << "Bind Texture Coordinates..." << endl;
            float* f_p_textureCoord = (float*) malloc(sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2);
            for (unsigned int ui_textureNum = 0; ui_textureNum < aim_p_asMesh[ui_meshNum]->mNumVertices; ui_textureNum++)
            {
                f_p_textureCoord[ui_textureNum*2] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].x;
                f_p_textureCoord[ui_textureNum*2+1] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].y;
            }
            
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2, f_p_textureCoord, GL_STATIC_DRAW);
            //glEnableVertexAttribArray(SHADER_TEX_COORD_LOC);
            //glVertexAttribPointer(SHADER_TEX_COORD_LOC, 2, GL_FLOAT, 0, 0, 0);
        }
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        
        //TODO: Material und Textur von Objekt
        
        
        stm_meshList.push_back(stm_meshVAO);
    }
}

void CSceneManager::bindUniform(GLuint glui_shaderProgram)
{
    GLint uniformProjectionMatrix = glGetUniformLocation(glui_shaderProgram,"m_projection");
	glUniformMatrix4fv(uniformProjectionMatrix, 1, GL_FALSE, glm::value_ptr(returnCameraNode()->returnProjectionMatrix()));
    
    GLint uniformViewMatrix = glGetUniformLocation(glui_shaderProgram,"m_view");
	glUniformMatrix4fv(uniformViewMatrix, 1, GL_FALSE, glm::value_ptr(returnCameraNode()->returnViewMatrix()));
    
    GLint uniformLightVector = glGetUniformLocation(glui_shaderProgram,"lightPos");
	glUniform3f(uniformLightVector,
                (returnLightNode()[0]->returnPosition())->x,
                (returnLightNode()[0]->returnPosition())->y,
                (returnLightNode()[0]->returnPosition())->z);
    
    // Diffuse Light: (1,1,1)
    uniformLightVector = glGetUniformLocation(glui_shaderProgram,"diffuseLightColor");
    glUniform3f(uniformLightVector,
                (returnLightNode()[0]->returnDiffuse())->x,
                (returnLightNode()[0]->returnDiffuse())->y,
                (returnLightNode()[0]->returnDiffuse())->z);
}

void CSceneManager::bindUniformModelMatrix(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram)
{
    GLint uniformModelMatrix = glGetUniformLocation(glui_shaderProgram,"m_model");
	glUniformMatrix4fv(uniformModelMatrix, 1, GL_FALSE, glm::value_ptr( *sn_p_drawNode->returnModelMatrix() ));
    
    glm::mat4 normalBuffer = returnCameraNode()->returnViewMatrix() * *sn_p_drawNode->returnModelMatrix();
    
    normalBuffer = glm::transpose(glm::inverse(normalBuffer));
    glm::mat3 normalMatrix = glm::mat3(normalBuffer);
    
    GLint uniformNormalMatrix = glGetUniformLocation(glui_shaderProgram,"m_normal");
	glUniformMatrix3fv(uniformNormalMatrix, 1, GL_FALSE, glm::value_ptr( normalMatrix ) ) ;
}

void CSceneManager::createCameraNode()
{
    c_cameraNode = new CCamera(ais_asScene->mCameras[0],
                               &ais_asScene->mRootNode->FindNode("Camera")->mTransformation);
}

void CSceneManager::createLightNode()
{
    for (int i_numLightSource = 0; i_numLightSource < ais_asScene->mNumLights; i_numLightSource++)
    {
        v_p_lightNode.push_back(new CLight(ais_asScene->mLights[i_numLightSource],
                                           &ais_asScene->mRootNode->FindNode("Lamp")->mTransformation));
    }
}

