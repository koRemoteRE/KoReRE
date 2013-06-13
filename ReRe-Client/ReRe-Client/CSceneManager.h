//
//  SceneManager.h
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__SceneManager__
#define __ReRe_Client__SceneManager__

#include <map>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <GL/glew.h>
#include <GL3/gl3.h>
#include <GL/glfw.h>
#include <IL/il.h>

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
        GLuint glui_materialIndex = 0;
    };
    vector<st_meshVAO> v_stm_meshList;
    
    struct st_material
    {
        glm::vec4 v_diffuse = glm::vec4(0.8, 0.8, 0.8, 1);
        GLfloat glf_textureCount = 0;
    };
    vector<st_material> v_stmat_materialList;
    
    //
    Assimp::Importer imp_asImporter;
    
    //
    CSceneNode* sn_p_rootSceneNode;
    CCamera* c_p_cameraNode;
    vector<CLight*> v_p_lightNode;
    
    //
    aiMesh** aim_p_asMesh;
    GLint gli_numMesh;
    aiMaterial** aim_p_asMaterial;
    GLint gli_numMaterial;
    aiTexture** ait_p_asTexture;
    GLint gli_numTexture;
    
    //
    map<string, GLuint> map_strglui_textureID;
    
    // Methoden
public:
    //
    CSceneManager(std::string st_filename);         // Konstruktor
    ~CSceneManager();                               // Destruktor
    //
    void drawScene(GLuint glui_shaderProgram);            // Szene zeichnen
    
    //
    inline CSceneNode* returnRootSceneNode()              // Gibt den Wurzelknoten des Szenegraphen zurück
        { return sn_p_rootSceneNode; };
    
    //
    inline CCamera* returnCameraNode()                     // Gibt die Kamera zurück
        { return c_p_cameraNode; };
    
    //
    inline vector<CLight*> returnLightNode()               // Gibt Lichtquelle zurück
        { return v_p_lightNode; };
    
    inline int returnLightNodeSize()                      // Gibt die Anzahl der Lichtquellen zurück
        { return v_p_lightNode.size(); };

private:
    //
    void drawScene(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram);
    
    //
    void bindUniform(GLuint glui_shaderProgram);    // View- und Projection-Matrix an Shader binden
    void bindUniformModelMatrix(CSceneNode* sn_p_drawNode, GLuint glui_shaderProgram); // Model- und Normal-Matrix an Shader binden
    void bindUniformTextureMaterial(unsigned int ui_meshNum, GLuint glui_shaderProgram);    // Material an Shader binden
    
    // Vertex Array Objekt konfigurieren
    void bindVAO();
    
    //
    void loadTexture();
    
    //
    void createCameraNode(const aiScene* ais_asScene);  // Legt eine Kamera an
    void createLightNode(const aiScene* ais_asScene);   // Legt eine neue Lichtquelle an
};

#endif /* defined(__ReRe_Client__SceneManager__) */
