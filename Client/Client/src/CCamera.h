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
    
    st_cameraParameterIntern stcpi_intrinsic;
    
    struct st_cameraVectors
    {
        glm::vec3 v_eyePosition;
        glm::vec3 v_eyeLookAt;
        glm::vec3 v_eyeUp;
        
        glm::vec3 v_rightVec;
        glm::vec3 v_viewVec;
    };
    
    st_cameraVectors stcv_update;
    
    struct st_cameraMovementParameters
    {
        int i_mouseX;
        int i_mouseY;
        float f_verticalAngle;
        float f_horizontalAngle;
        float f_rotationSpeed;
        float f_moveSpeed;
    };
    
    st_cameraMovementParameters stcmp_update;
    
    double d_LastTime;
    
    glm::mat4* m_viewMatrix;
    glm::mat4* m_projectionMatrix;
    
public:
    CCamera();
    
    glm::vec3 returnPosition();
    
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
    void setViewMatrix(glm::vec3 v_eyePosition, glm::vec3 v_eyeLookAt, glm::vec3 v_eyeUp)
        { *m_viewMatrix = glm::lookAt(v_eyePosition, v_eyeLookAt, v_eyeUp); };
    void setProjectionPerspMatrix()
        {*m_projectionMatrix = glm::perspective(stcpi_intrinsic.f_fieldOfView,
                                                stcpi_intrinsic.f_aspect,
                                                stcpi_intrinsic.f_near,
                                                stcpi_intrinsic.f_far);};
};



#endif /* defined(__ReRe_Client__Camera__) */
