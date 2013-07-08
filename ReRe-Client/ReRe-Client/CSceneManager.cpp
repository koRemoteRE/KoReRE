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
        // Mit Assimp Collada-Datei laden
        const aiScene* ais_p_asImportScene = imp_asImporter.ReadFile(st_filename,  aiProcess_CalcTangentSpace |
                                                                     aiProcess_Triangulate |
                                                                     aiProcess_JoinIdenticalVertices |
                                                                     aiProcess_SortByPType);
        if (ais_p_asImportScene == NULL)
            throw 0x01;
        
        // Neuen Root-SceneNode für eigenen Szenegraph anlegen
        sn_p_rootSceneNode = new CSceneNode(ais_p_asImportScene->mRootNode);
        
        // Licht und Kamera anlegen
        createLightNode();
        createCameraNode();

        gli_numMesh = ais_p_asImportScene->mNumMeshes;
        gli_numMaterial = ais_p_asImportScene->mNumMaterials;
        gli_numTexture = ais_p_asImportScene->mNumTextures;
        
        aim_p_asMesh = new aiMesh*[gli_numMesh];
        for (int i_mesh = 0; i_mesh < gli_numMesh; i_mesh++)
            aim_p_asMesh[i_mesh] = new aiMesh(*ais_p_asImportScene->mMeshes[i_mesh]);
        
        aim_p_asMaterial = new aiMaterial*[gli_numMaterial];
        for (int i_material = 0; i_material < gli_numMaterial; i_material++)
            aim_p_asMaterial[i_material] = new aiMaterial(*ais_p_asImportScene->mMaterials[i_material]);
        
        ait_p_asTexture = new aiTexture*[gli_numTexture];
        for (int i_texture = 0; i_texture < gli_numTexture; i_texture++)
            ait_p_asTexture[i_texture] = new aiTexture(*ais_p_asImportScene->mTextures[i_texture]);
        
        // Verwendete Texturen laden
        loadTexture();
        
        // Alle Daten Vertex Array Object binden
        bindVAO();
        
        // Assimp-Szene löschen
       //delete ais_p_asImportScene;
    }
    else
    {
        throw 0x00;     // .dae-Datei benötigt
    }
}

CSceneManager::~CSceneManager()
{
    delete sn_p_rootSceneNode;
    delete c_p_cameraNode;
    
    v_stm_meshList.~vector();
    v_stmat_materialList.~vector();
    v_p_lightNode.~vector();
    
    map_strglui_textureID.~map();
    
    delete[] aim_p_asMesh;
    delete  aim_p_asMesh;
    
    delete[] aim_p_asMaterial;
    delete aim_p_asMaterial;
    
    delete[] ait_p_asTexture;
    delete ait_p_asTexture;
}

void CSceneManager::drawScene(GLuint& glui_shaderProgram)
{
    bindUniform(glui_shaderProgram);
    drawScene(sn_p_rootSceneNode, glui_shaderProgram);
}

void CSceneManager::drawScene(CSceneNode* sn_p_drawNode, GLuint& glui_shaderProgram)
{
    // Vertexliste zeichnen
    // Für alle Knoten zeichnen
    bindUniformModelMatrix(sn_p_drawNode, glui_shaderProgram);
    
    // Texture Uniform Location
    GLint gli_texture = glGetUniformLocation(glui_shaderProgram,"texture2D");
    GLint* gli_meshIndex = new GLint;
    
    for (GLuint glui_numMesh = 0; glui_numMesh <  *(sn_p_drawNode->returnNumberOfMesh()); glui_numMesh++)
    {
        *gli_meshIndex = sn_p_drawNode->returnMeshIndex()[glui_numMesh];
        
        bindUniformTextureMaterial(v_stm_meshList[*gli_meshIndex].glui_materialIndex, glui_shaderProgram);
        // Texture anbinden (GL_TEXTURE_0) und an Shader übergeben
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, v_stm_meshList[sn_p_drawNode->returnMeshIndex()[glui_numMesh]].glui_textureIndex);
        glUniform1i(gli_texture,0);
        
        glBindVertexArray(v_stm_meshList[sn_p_drawNode->returnMeshIndex()[glui_numMesh]].glui_vaoBuffer);
        glDrawElements(GL_TRIANGLES,v_stm_meshList[sn_p_drawNode->returnMeshIndex()[glui_numMesh]].glui_numFace*3, GL_UNSIGNED_INT, 0);
    }
    
    //Rekursiv alle Kinderknoten aufrufen und zeichnen
    for (unsigned int ui_numNode = 0; ui_numNode < sn_p_drawNode->returnChildren().size(); ui_numNode++)
    {
        drawScene(sn_p_drawNode->returnChildren()[ui_numNode], glui_shaderProgram);
    }
}
void CSceneManager::bindUniform(GLuint& glui_shaderProgram)
{
    // ProjectionsMatrix an Shader übergeben
    GLint gli_uniformProjectionMatrix = glGetUniformLocation(glui_shaderProgram,"m_projection");
	glUniformMatrix4fv(gli_uniformProjectionMatrix, 1, GL_FALSE, glm::value_ptr(returnCameraNode()->returnProjectionMatrix()));
    
    // ViewMatrix an Shader übergeben
    GLint gli_uniformViewMatrix = glGetUniformLocation(glui_shaderProgram,"m_view");
	glUniformMatrix4fv(gli_uniformViewMatrix, 1, GL_FALSE, glm::value_ptr(returnCameraNode()->returnViewMatrix()));
    
    // Lichtposition an Shader übergeben
    GLint gli_uniformLightVector = glGetUniformLocation(glui_shaderProgram,"lightPos");
	glUniform3f(gli_uniformLightVector,
                (returnLightNode()[0]->returnPosition())->x,
                (returnLightNode()[0]->returnPosition())->y,
                (returnLightNode()[0]->returnPosition())->z);
    
    // Diffuses Licht an Shader übergeben
    gli_uniformLightVector = glGetUniformLocation(glui_shaderProgram,"diffuseLightColor");
    glUniform3f(gli_uniformLightVector,
                (returnLightNode()[0]->returnDiffuse())->x,
                (returnLightNode()[0]->returnDiffuse())->y,
                (returnLightNode()[0]->returnDiffuse())->z);
}

void CSceneManager::bindUniformModelMatrix(CSceneNode* sn_p_drawNode, GLuint& glui_shaderProgram)
{
    // ModelMatrix an Shader übergeben
    GLint gli_uniformModelMatrix = glGetUniformLocation(glui_shaderProgram,"m_model");
	glUniformMatrix4fv(gli_uniformModelMatrix, 1, GL_FALSE, glm::value_ptr( *sn_p_drawNode->returnModelMatrix() ));
    
    // Normalen Matrix berechnen
    glm::mat4 m_normalBuffer = returnCameraNode()->returnViewMatrix() * *sn_p_drawNode->returnModelMatrix();
    m_normalBuffer = glm::transpose(glm::inverse(m_normalBuffer));
    glm::mat3 m_normalMatrix = glm::mat3(m_normalBuffer);
    
    // Normalen Matrix an Shader übergeben
    GLint gli_uniformNormalMatrix = glGetUniformLocation(glui_shaderProgram,"m_normal");
	glUniformMatrix3fv(gli_uniformNormalMatrix, 1, GL_FALSE, glm::value_ptr( m_normalMatrix ) ) ;
}

void CSceneManager::bindUniformTextureMaterial(GLuint& ui_meshNum, GLuint& glui_shaderProgram)
{
    GLint gli_uniformMaterial = glGetUniformLocation(glui_shaderProgram,"diffuseMaterialColor");
    glUniform4f(gli_uniformMaterial,
                v_stmat_materialList[ui_meshNum].v_diffuse[0],
                v_stmat_materialList[ui_meshNum].v_diffuse[1],
                v_stmat_materialList[ui_meshNum].v_diffuse[2],
                v_stmat_materialList[ui_meshNum].v_diffuse[3]);
    
    gli_uniformMaterial = glGetUniformLocation(glui_shaderProgram,"textureCount");
    glUniform1f(gli_uniformMaterial, v_stmat_materialList[ui_meshNum].glf_textureCount);
    
    gli_uniformMaterial = glGetUniformLocation(glui_shaderProgram,"textureActiv");
    glUniform1i(gli_uniformMaterial, TEXTURE_ACTIVITY);
}

//
void CSceneManager::bindVAO()
{
    //Vertexliste für jedes Mesh in der Szene anlegen
    GLuint glui_vertexArrayObjBuffer;
    
    for (unsigned int ui_meshNum = 0; ui_meshNum < gli_numMesh; ui_meshNum++)
    {
        cout << "Create VAO-" << ui_meshNum << "..." << endl;
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
        
        // Vertices anbinden
        if (aim_p_asMesh[ui_meshNum]->HasPositions())
        {
            cout << "Bind Positions..." << endl;
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 3, aim_p_asMesh[ui_meshNum]->mVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_POSITION_LOC);
            glVertexAttribPointer(SHADER_POSITION_LOC, 3, GL_FLOAT, 0, 0, 0);
        }
        
        // Normalen anbinden
        if (aim_p_asMesh[ui_meshNum]->HasNormals())
        {
            cout << "Bind Normals..." << endl;
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 3, aim_p_asMesh[ui_meshNum]->mNormals, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_NORMAL_LOC);
            glVertexAttribPointer(SHADER_NORMAL_LOC, 3, GL_FLOAT, 0, 0, 0);
        }
        
        // Texturen anbinden
        if (aim_p_asMesh[ui_meshNum]->HasTextureCoords(0))
        {
            cout << "Bind Texture Coordinates... \n" << endl;
            float* f_p_textureCoord = (float*) malloc(sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2);
            for (unsigned int ui_textureNum = 0; ui_textureNum < aim_p_asMesh[ui_meshNum]->mNumVertices; ui_textureNum++)
            {
                f_p_textureCoord[ui_textureNum*2] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].x;
                f_p_textureCoord[ui_textureNum*2+1] = aim_p_asMesh[ui_meshNum]->mTextureCoords[0][ui_textureNum].y;
            }
            
            glGenBuffers(1, &glui_vertexArrayObjBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, glui_vertexArrayObjBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aim_p_asMesh[ui_meshNum]->mNumVertices * 2, f_p_textureCoord, GL_STATIC_DRAW);
            glEnableVertexAttribArray(SHADER_TEX_COORD_LOC);
            glVertexAttribPointer(SHADER_TEX_COORD_LOC, 2, GL_FLOAT, 0, 0, 0);
        }
        // unbind Buffers
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        
        // Material oder Texture anbinden
        st_material stm_matBuffer;
        aiString aistr_texPath;
        
        GLuint glui_matIndexBuffer = aim_p_asMesh[ui_meshNum]->mMaterialIndex;
        
        if (AI_SUCCESS == aim_p_asMaterial[glui_matIndexBuffer]->GetTexture(aiTextureType_DIFFUSE, 0, &aistr_texPath))
        {
            stm_meshVAO.glui_textureIndex = map_strglui_textureID[aistr_texPath.data];
            stm_meshVAO.glui_materialIndex = glui_matIndexBuffer;
            stm_matBuffer.glf_textureCount = 1;
        }
        
        else
        {
            stm_meshVAO.glui_materialIndex = v_stmat_materialList.size();
            stm_matBuffer.glf_textureCount = 0;
        }
        
        aiColor4D diffuse(0.8,0.8,0.8,1);
        if(AI_SUCCESS == aiGetMaterialColor(aim_p_asMaterial[glui_matIndexBuffer], AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        {
            stm_matBuffer.v_diffuse.r = diffuse.r;
            stm_matBuffer.v_diffuse.g = diffuse.g;
            stm_matBuffer.v_diffuse.b = diffuse.b;
            stm_matBuffer.v_diffuse.a = diffuse.a;
        }
        v_stm_meshList.push_back(stm_meshVAO);
        v_stmat_materialList.push_back(stm_matBuffer);
        
        delete ui_p_faceVertexArrayObjArray;
    }
}

void CSceneManager::loadTexture()
{
    // DevIL initialisieren
    ilInit();
    
    // Szenen Material nach Texturen durchsuchen
    for (int i_material = 0, i_texIndex = 0; i_material < gli_numMaterial; i_material++, i_texIndex = 0)
    {
        aiString aistr_path;
        while (aim_p_asMaterial[i_material]->GetTexture(aiTextureType_DIFFUSE, i_texIndex, &aistr_path) == AI_SUCCESS)
        {
            map_strglui_textureID[aistr_path.data] = 0;
            i_texIndex++;
        }
    }
    
    int i_textureIDSize = map_strglui_textureID.size();
    
    // Array mit DevIL-Texture IDs füllen
    ILuint* ilui_imageID = new ILuint[i_textureIDSize];
    ilGenImages(i_textureIDSize, ilui_imageID);
    
    // Array mit GL-Texture IDs füllen
    GLuint* glui_textureID = new GLuint[i_textureIDSize];
    glGenTextures(i_textureIDSize, glui_textureID);
    
    int i_index = 0;
    for (map<string, GLuint>::iterator iter_textureIDNum = map_strglui_textureID.begin(); iter_textureIDNum != map_strglui_textureID.end(); iter_textureIDNum++, i_index++)
    {
        // DevIL-Bild ID speichern
        string str_filename = (*iter_textureIDNum).first;
        (*iter_textureIDNum).second = glui_textureID[i_index];
        
        // DevIL-Bild Name binden
        ilBindImage(ilui_imageID[i_index]);
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
        
        if (ilLoadImage((ILstring)str_filename.c_str()))
        {
            // Bild in RGBA umwandeln
            ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
            glActiveTexture(GL_TEXTURE0);
            // OpenGL Texture erstellen und laden
            glBindTexture(GL_TEXTURE_2D, glui_textureID[i_index]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
        }
    }
    
    // DevIL-Daten löschen (Als OpenGL Texture vorhanden)
    ilDeleteImages(i_textureIDSize, ilui_imageID);
    delete [] ilui_imageID;
    delete [] glui_textureID;
}

void CSceneManager::createCameraNode(void)
{
    // Kamera anlegen
    c_p_cameraNode = new CCamera();
}

void CSceneManager::createLightNode(void)
{
    // Lichtquelle anlegen
    v_p_lightNode.push_back(new CLight());
}

