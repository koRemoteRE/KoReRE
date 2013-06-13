//
//   Camera.cpp
//  ReRe-Client
//
//  Created by Ina Schröder on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CCamera.h"

/*
std::vector<glm::vec3> Camera::getWSfrustumCorners() {
    glm::mat4 matViewInv = getViewInv();
    // calculate frustum corner coordinates
    float fFov2 = getFovRad() / 2.0f;
    float tanFov2 = glm::tan(fFov2);
    float h2Far = tanFov2 * _fFar;
    float h2Near = tanFov2 * _fNear;
    float hFar = 2.0f * h2Far;
    float hNear = 2.0f * h2Near;
    
    //to do
    //DUMMY RESOLUTION! Replace the dummy variable by the value read from the scene manager!
    const glm::ivec2& res =  glm::ivec2(1,1);
    
    float aspect = static_cast<float>(res.x) / static_cast<float>(res.y);
    float w2Far = (hFar * aspect) / 2.0f;
    float w2Near = (hNear * aspect) / 2.0f;
    glm::vec3 v3Corners[8];
    
    v3Corners[0] = glm::vec3(-1.0f * w2Near, -1.0f * h2Near, -_fNear);  // lbn
    v3Corners[1] = glm::vec3(1.0f * w2Near, -1.0f * h2Near, -_fNear);  // rbn
    v3Corners[2] = glm::vec3(1.0f * w2Near,  1.0f * h2Near, -_fNear);  // rtn
    v3Corners[3] = glm::vec3(-1.0f * w2Near,  1.0f * h2Near, -_fNear);  // ltn
    
    v3Corners[4] = glm::vec3(-1.0f * w2Far, -1.0f * h2Far, -_fFar);  // lbn
    v3Corners[5] = glm::vec3(1.0f * w2Far, -1.0f * h2Far, -_fFar);  // rbn
    v3Corners[6] = glm::vec3(1.0f * w2Far,  1.0f * h2Far, -_fFar);  // rtn
    v3Corners[7] = glm::vec3(-1.0f * w2Far,  1.0f * h2Far, -_fFar);  // ltn
    
    std::vector<glm::vec3> vReturnCorners;
    
    // transform each corner into WS
    for (int i = 0; i < 8; ++i) {
        glm::vec4 v4Result = matViewInv * glm::vec4(v3Corners[ i ], 1.0f);
        vReturnCorners.push_back(glm::vec3(v4Result.x, v4Result.y, v4Result.z));
    }
    
    return vReturnCorners;
}

void Camera::updateFrustumPlanes() {
    // If the camera's projection matrix is an orthogonal projection,
    // the frustum planes have to be derived
    // from the general projection matrix
    if (_bIsOrtho) {
        glm::mat4 projT = glm::transpose(_matProjection);
        
        _v4FrustumPlanesVS[ PLANE_NEAR ]       = projT[3] + projT[2];
        _v4FrustumPlanesVS[ PLANE_FAR ]        = projT[3] - projT[2];
        _v4FrustumPlanesVS[ PLANE_LEFT ]       = projT[3] + projT[0];
        _v4FrustumPlanesVS[ PLANE_RIGHT ]      = projT[3] - projT[0];
        _v4FrustumPlanesVS[ PLANE_BOTTOM ]     = projT[3] + projT[1];
        _v4FrustumPlanesVS[ PLANE_TOP ]        = projT[3] - projT[1];
        
        // The normals in the plane-vectors (N.x, N.y, N.z, D) have to be normalized
        glm::vec3 v3N;
        for (unsigned int i = 0; i < 6; ++i) {
            v3N = glm::normalize(glm::vec3(_v4FrustumPlanesVS[i]));
            _v4FrustumPlanesVS[ i ].x = v3N.x;
            _v4FrustumPlanesVS[ i ].y = v3N.y;
            _v4FrustumPlanesVS[ i ].z = v3N.z;
        }
    } else {
        // If the camera's projection matrix is a perspective projection,
        // the view-space frustum planes can be
        // determined by the proj-parameters of the camera
        // (more efficient this way)
        float fe1 = glm::sqrt(_fFocalLength * _fFocalLength + 1);
        float fea = glm::sqrt(_fFocalLength * _fFocalLength +
                              getAspectRatio() * getAspectRatio());
        
        _v4FrustumPlanesVS[PLANE_NEAR] =
        glm::vec4(0.0f, 0.0f, -1.0f, -_fNear);
        
        _v4FrustumPlanesVS[PLANE_FAR] =
        glm::vec4(0.0f, 0.0f, 1.0f, _fFar);
        
        _v4FrustumPlanesVS[PLANE_LEFT] =
        glm::vec4(_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);
        
        _v4FrustumPlanesVS[PLANE_RIGHT] =
        glm::vec4(-_fFocalLength / fe1, 0.0f, -1.0f / fe1, 0.0f);
        
        _v4FrustumPlanesVS[PLANE_BOTTOM] =
        glm::vec4(0.0f, _fFocalLength / fea,
                  -getAspectRatio() / fea, 0.0f);
        
        _v4FrustumPlanesVS[PLANE_TOP] =
        glm::vec4(0.0f, -_fFocalLength / fea,
                  -getAspectRatio() / fea, 0.0f);
    }
}

bool Camera::
isVisible(const glm::vec3& rSphereCenterWS, const float fRadius) const {
    glm::vec4 bSphereVS =  _matView * glm::vec4(rSphereCenterWS, 1.0f);
    const glm::vec3 v3Center = glm::vec3(bSphereVS);
    
    // 1) Cull if behind camera
    if (v3Center.z > fRadius)
        return false;
    
    // 2) Cull if behind far plane
    if (glm::abs(v3Center.z) - _fFar > fRadius)
        return false;
    
    // 3) Cull if in front of near plane
    if (v3Center.z < 0.0f && _fNear - glm::abs(v3Center.z) > fRadius)
        return false;
    
    // 4) Cull if outside for all frustum-planes.
    // NOTE/TODO: PLANE_NEAR and PLANE_FAR should be not needed, because they
    // are already accounted for in step 2) and 3)
    if (glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_LEFT]))
        < -fRadius ||
        
        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_RIGHT]))
        < -fRadius ||
        
        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_BOTTOM]))
        < -fRadius ||
        
        glm::dot(v3Center, glm::vec3(_v4FrustumPlanesVS[PLANE_TOP]))
        < -fRadius) {
        return false;
    }
    return true;
}*/

//--------------------------------------------------
//--------------------------------------------------

CCamera::CCamera()
{
    d_LastTime = glfwGetTime();
    
    m_viewMatrix = new glm::mat4(1);
    m_projectionMatrix = new glm::mat4(1);
    
    // Intrinsische Kameraparameter definieren
    stcpi_intrinsic.f_fieldOfView   = 27;
    stcpi_intrinsic.f_aspect        = 1.7;
    stcpi_intrinsic.f_near          = 0.1;
    stcpi_intrinsic.f_far           = 100;
    
    setProjectionPerspMatrix();
    
    stcmp_update.f_verticalAngle    = 0.0f;
    stcmp_update.f_horizontalAngle  = 3.1454f;
    stcmp_update.f_rotationSpeed    = CAM_ROTATION_SPEED;
    stcmp_update.f_moveSpeed        = CAM_MOVE_SPEED;
    
    stcv_update.v_eyePosition   = glm::vec3(CAM_POS_X,CAM_POS_Y,CAM_POS_Z);
    stcv_update.v_eyeLookAt     = glm::vec3(CAM_LOOK_AT_X,CAM_LOOK_AT_Y,CAM_LOOK_AT_Z);
    stcv_update.v_eyeUp         = glm::vec3(CAM_UP_X,CAM_UP_Y,CAM_UP_Z);
    
    setViewMatrix(stcv_update.v_eyePosition,
                  stcv_update.v_eyeLookAt,
                  stcv_update.v_eyeUp);
    
    //setViewMatrix(glm::vec3(6.8,-5.9,4.9), glm::vec3(0,0,0), glm::vec3(-0.3,0.3,0.9));
}

glm::vec3 returnPosition()
{
    return glm::vec3(  );
}

void CCamera::automaticMovement(int i)
{
    i /= 30;
    float f_newPositionX = sin(i) / 30;
    *m_viewMatrix = glm::translate(*m_viewMatrix, f_newPositionX, 0.0f, 0.0f);
}

/*void CCamera::updateCameraView()
{
    // Get mouse Position
    glfwGetMousePos(&i_MouseX, &i_MouseY);
    
    
    
    // Reset mouse Position for next frame
    glfwSetMousePos(WIDTH/2, HEIGHT/2);
}*/

void CCamera::updateCameraView()
{
    // Get mouse position
    glfwGetMousePos(&stcmp_update.i_mouseX, &stcmp_update.i_mouseY);
    
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - d_LastTime);
    d_LastTime = currentTime;
    
    // Compute new orientation
    stcmp_update.f_horizontalAngle  += stcmp_update.f_rotationSpeed * deltaTime * float( WIDTH/2 - stcmp_update.i_mouseX );
    stcmp_update.f_verticalAngle    += stcmp_update.f_rotationSpeed * deltaTime * float( HEIGHT/2 - stcmp_update.i_mouseY );
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(cos(stcmp_update.f_verticalAngle) * sin(stcmp_update.f_horizontalAngle),
                        sin(stcmp_update.f_verticalAngle),
                        cos(stcmp_update.f_verticalAngle) * cos(stcmp_update.f_horizontalAngle));
    
    // Right vector
    glm::vec3 right = glm::vec3(sin(stcmp_update.f_horizontalAngle - 3.14f/2.0f),
                                0,
                                cos(stcmp_update.f_horizontalAngle - 3.14f/2.0f));
    
    // Up vector : perpendicular to both direction and right
    stcv_update.v_eyeUp = glm::cross( right, direction );
    
    // Move faster
    if (glfwGetKey( GLFW_KEY_LSHIFT ) == GLFW_PRESS)
        stcmp_update.f_moveSpeed = 2.5f * CAM_MOVE_SPEED;
    // Slow down to standard CAM_MOVE_SPEED
    else
        stcmp_update.f_moveSpeed = CAM_MOVE_SPEED;
    
    // Move forward
    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS || glfwGetKey( 'W' ) == GLFW_PRESS)
        stcv_update.v_eyePosition += direction * deltaTime * stcmp_update.f_moveSpeed;
    // Move backward
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS || glfwGetKey( 'S' ) == GLFW_PRESS)
        stcv_update.v_eyePosition -= direction * deltaTime * stcmp_update.f_moveSpeed;
    
    // Strafe right
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS || glfwGetKey( 'D' ) == GLFW_PRESS)
        stcv_update.v_eyePosition += right * deltaTime * stcmp_update.f_moveSpeed;
    // Strafe left
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey( 'A' ) == GLFW_PRESS)
        stcv_update.v_eyePosition -= right * deltaTime * stcmp_update.f_moveSpeed;
    
    // Move up
    if (glfwGetKey( 'Q' ) == GLFW_PRESS)
        stcv_update.v_eyePosition += stcv_update.v_eyeUp * deltaTime * stcmp_update.f_moveSpeed;
    // Move down
    if (glfwGetKey( 'E' ) == GLFW_PRESS)
        stcv_update.v_eyePosition -= stcv_update.v_eyeUp * deltaTime * stcmp_update.f_moveSpeed;
    
    stcv_update.v_eyeLookAt = stcv_update.v_eyePosition + direction;
    
    *m_viewMatrix= glm::lookAt(stcv_update.v_eyePosition,
                               stcv_update.v_eyeLookAt,
                               stcv_update.v_eyeUp);
    
    // Decide whether to show Camera Data (Position, LookAt, Up)
    if (glfwGetKey( GLFW_KEY_SPACE ) == GLFW_PRESS)
    {
        cout << "Position: ( " << stcv_update.v_eyePosition.x << ", " << stcv_update.v_eyePosition.y << ", " << stcv_update.v_eyePosition.z << " )" <<endl;
        cout << "Look At: ( " << stcv_update.v_eyeLookAt.x << ", " << stcv_update.v_eyeLookAt.y << ", " << stcv_update.v_eyeLookAt.z << " )" << endl;
        cout << "Up: ( " << stcv_update.v_eyeUp.x << ", " << stcv_update.v_eyeUp.y << ", " << stcv_update.v_eyeUp.z << " ) \n" << endl;
    }
        
    // Reset mouse position for next frame
    glfwSetMousePos(WIDTH/2, HEIGHT/2);
}

bool CCamera::viewVisible(CSceneNode* sc_rootSceneNode)
{
    //View Frustum Culling
    return false;
}

