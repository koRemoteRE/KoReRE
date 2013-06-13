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

#include <GL/glfw.h>
#include "CSceneNode.h"
#include "CDefinitions.h"

class CCamera
{
private:
    struct st_cameraParameterIntern
    {
        float f_far;
        float f_near;
        float f_deg;
        float f_aspect;
        float f_fieldOfView;
    };
    
    int i_MouseX, i_MouseY;
    float f_RotationSpeed, f_MoveSpeed;
    
    float f_VerticalAngle, f_HorizontalAngle;
    float f_FOV;
    
    double d_LastTime;
    
    glm::vec3 v_eyePosition;
    glm::vec3 v_eyeLookAt;
    glm::vec3 v_eyeUp;
    
    glm::vec3 v_rightVec;
    glm::vec3 v_viewVec;
    
    glm::mat4* m_viewMatrix;
    glm::mat4* m_projectionMatrix;
    
    aiMatrix4x4* m_Transform;
    
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
    
    void automaticMovement(int i);
    
    void updateCameraView();
    
    bool viewVisible(CSceneNode* sc_rootSceneNode);     // View Frustum Culling
    
private:
    void setViewMatrix(aiCamera* aic_asCamera, aiMatrix4x4* aim_nodeTransform);
    void setProjectionPerspMatrix(aiCamera* aic_asCamera);
};



#endif /* defined(__ReRe_Client__Camera__) */
