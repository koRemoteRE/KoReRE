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

#include "CSceneNode.h"
#include "CCamera.h"

using namespace std;

class CSceneManager
{
private:
    Assimp::Importer imp_asImporter;
    const aiScene* ais_asScene;
    
    CSceneNode* sn_p_rootSceneNode;
    CCamera* c_cameraNode;
    vector<CLight*> v_lightNode;
    
public:
    CSceneManager();                                // Konstruktor
    
    //
    CSceneNode* returnRootSceneNode();              // Gibt den Wurzelknoten des Szenegraphen zurück
    
    //
    CCamera* createCameraNode();                    // Legt eine Kamera an
    void deleteCameraNode();                        // Löscht die Kamera
    CCamera* returnCameraNode();                    // Gibt die Kamera zurück
    
    //
    CLight* createLightNode();                      // Legt eine neue Lichtquelle an
    void deleteLightNode(int i_lightNodeID = 0);    // Löscht Lichtquelle anhand der ID
    CLight* returnLightNode(int i_lightNodeID = 0); // Gibt Lichtquelle anhand der ID zurück
    int returnLightNodeSize();                      // Gibt die Anzahl der Lichtquellen zurück
    vector<CLight*>* returnLightVector();           // Gibt Pointer auf Vector mit allen Lichtquellen zurück
};

#endif /* defined(__ReRe_Client__SceneManager__) */
