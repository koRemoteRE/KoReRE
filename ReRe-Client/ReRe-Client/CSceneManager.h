//
//  SceneManager.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__SceneManager__
#define __ReRe_Client__SceneManager__

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <GL/glew.h>
#include <GL3/gl3.h>

#include "CDefinitions.h"
#include "CSceneNode.h"
#include "CCamera.h"

using namespace std;

class CSceneManager
{
    // Variablen
private:
    //
    struct st_meshVAO
    {
        GLuint glui_vaoBuffer;
        GLuint glui_textureIndex;
        GLuint glui_numFace;
    };
    vector<st_meshVAO> stm_meshList;
    
    //
    Assimp::Importer imp_asImporter;
    const aiScene* ais_asScene;
    
    //
    CSceneNode* sn_p_rootSceneNode;
    CCamera* c_cameraNode;
    vector<CLight*> v_p_lightNode;
    
    //
    aiMesh** aim_p_asMesh;
    int* i_p_numMesh;
    aiMaterial** aim_p_asMaterial;
    int* i_p_numMaterial;
    aiTexture** ait_p_mTexture;
    int* i_p_numTexture;
    
    // Methoden
public:
    CSceneManager(std::string st_filename);         // Konstruktor
    
    //
    void drawScene(GLuint glui_shaderProgram);
    
    //
    CSceneNode* returnRootSceneNode()              // Gibt den Wurzelknoten des Szenegraphen zurück
        { return sn_p_rootSceneNode; };
    
    //
    CCamera* returnCameraNode()                     // Gibt die Kamera zurück
        { return c_cameraNode; };
    
    //
    vector<CLight*> returnLightNode()               // Gibt Lichtquelle zurück
        { return v_p_lightNode; };
    
    int returnLightNodeSize()                      // Gibt die Anzahl der Lichtquellen zurück
        { return v_p_lightNode.size(); };

private:
    //
    void drawScene(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram);
    
    //
    void bindVAO();
    
    //
    void bindUniform(GLuint glui_shaderProgram);    // View- und Projection-Matrix an Shader binden
    void bindUniformModelMatrix(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram);
                                                    // Model- und Normal-Matrix an Shader binden
    
    //
    void createCameraNode();                     // Legt eine Kamera an
    void createLightNode();                      // Legt eine neue Lichtquelle an
};

#endif /* defined(__ReRe_Client__SceneManager__) */
