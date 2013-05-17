//
//  Camera.h
//  ReRe-Client
//
//  Created by Ina Schröder on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#ifndef __ReRe_Client__Camera__
#define __ReRe_Client__Camera__

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

#include <assimp/camera.h>
#include <glm/ext.hpp>

#include "CSceneNode.h"
#include "CDefinitions.h"

class CCamera
{
private:
    struct st_windowParameter
    {
        float f_width;
        float f_height;
    };
    
    struct st_cameraParameterIntern
    {
        float f_far;
        float f_near;
        float f_deg;
        float f_aspect;
        float f_fieldOfView;
    };
    
    glm::mat4* m_viewMatrix;
    glm::mat4* m_projectionMatrix;
    
public:
    CCamera(aiCamera* aic_asCamera, aiMatrix4x4* aim_nodeTransform);
    
    glm::mat4 returnViewMatrix()
        { return *m_viewMatrix; };
    
    glm::mat4 returnViewMatrixInverse()
        { return glm::inverse(*m_viewMatrix); };
    
    glm::mat4 returnViewMatrixInverseTranspose()
        { return glm::transpose(glm::inverse(*m_viewMatrix)); };
    
    glm::mat4 returnProjectionMatrix()
        { return *m_projectionMatrix; };
    
    bool viewFrustumCullingVisible(CSceneNode* sc_rootSceneNode);
    
private:
    void setViewMatrix(aiCamera* aic_asCamera);
    void setProjectionPerspMatrix(aiCamera* aic_asCamera);
};



#endif /* defined(__ReRe_Client__Camera__) */
