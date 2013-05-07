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
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GL3/gl3.h>

#include "CSceneNode.h"
#include "CCamera.h"

using namespace std;

class CSceneManager
{
private:
    struct st_meshVAO
    {
        GLuint glui_vaoBuffer;
        GLuint glui_textureIndex;
        GLuint glui_numFace;
    };
    
    vector<st_meshVAO> stm_meshList;
    
    Assimp::Importer imp_asImporter;
    const aiScene* ais_asScene;
    
    CSceneNode* sn_p_rootSceneNode;
    CCamera* c_cameraNode;
    
    CLight* cl_p_lightNode;
    int* i_p_numLightNode;
    
    aiMesh** aim_p_asMesh;
    int* i_p_numMesh;
    aiMaterial** aim_p_asMaterial;
    int* i_p_numMaterial;
    aiTexture** ait_p_mTexture;
    int* i_p_numTexture;
    
public:
    CSceneManager(std::string st_filename);         // Konstruktor
    
    //
    CSceneNode* returnRootSceneNode();              // Gibt den Wurzelknoten des Szenegraphen zurück
    
    //
    void drawScene();
    
    //
    CCamera* returnCameraNode();                    // Gibt die Kamera zurück
    
    //
    CLight* returnLightNode();                      // Gibt Lichtquelle zurück
    int* returnLightNodeSize();                      // Gibt die Anzahl der Lichtquellen zurück

private:
    //
    void bindVAO();
    
    //
    CCamera* createCameraNode();                    // Legt eine Kamera an
    CLight* createLightNode();                      // Legt eine neue Lichtquelle an
};

#endif /* defined(__ReRe_Client__SceneManager__) */
