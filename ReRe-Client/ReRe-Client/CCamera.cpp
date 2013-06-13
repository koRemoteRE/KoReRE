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
    m_viewMatrix = new glm::mat4(1);
    m_projectionMatrix = new glm::mat4(1);
    
    // Intrinsische Kameraparameter definieren
    stcpi_intrinsic.f_fieldOfView = 27;
    stcpi_intrinsic.f_aspect = 1.7;
    stcpi_intrinsic.f_near = 0.1;
    stcpi_intrinsic.f_far = 100;
    setProjectionPerspMatrix();
    
    // vertical angle : 0, look at the horizon
    f_VerticalAngle = 0.0f;
    // horizontal angle : toward -Z
    f_HorizontalAngle = 3.1454f;
    
    f_RotationSpeed = 0.005f;
    f_MoveSpeed = 3.0f; // 3 units per second
    
    d_LastTime = glfwGetTime();
    
    setViewMatrix(glm::vec3(3,10,30), glm::vec3(0,0,0), glm::vec3(0,1,0));
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
    glfwGetMousePos(&i_MouseX, &i_MouseY);
    
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - d_LastTime);
    d_LastTime = currentTime;
    
    // Compute new orientation
    f_HorizontalAngle -= f_RotationSpeed * deltaTime * float( WIDTH/2 - i_MouseX );
    f_VerticalAngle   -= f_RotationSpeed * deltaTime * float( HEIGHT/2 - i_MouseY );
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(cos(f_VerticalAngle) * sin(f_HorizontalAngle),
                        sin(f_VerticalAngle),
                        cos(f_VerticalAngle) * cos(f_HorizontalAngle));
    
    // Right vector
    glm::vec3 right = glm::vec3(sin(f_HorizontalAngle - 3.14f/2.0f),
                                0,
                                cos(f_HorizontalAngle - 3.14f/2.0f));
    
    // Up vector : perpendicular to both direction and right
    v_eyeUp = glm::cross( right, direction );
    
    // Move forward
    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS || glfwGetKey( 'W' ) == GLFW_PRESS){
        v_eyePosition -= direction * deltaTime * f_MoveSpeed;
    }
    // Move backward
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS || glfwGetKey( 'S' ) == GLFW_PRESS){
        v_eyePosition += direction * deltaTime * f_MoveSpeed;
    }
    // Strafe right
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS || glfwGetKey( 'D' ) == GLFW_PRESS){
        v_eyePosition -= right * deltaTime * f_MoveSpeed;
    }
    // Strafe left
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey( 'A' ) == GLFW_PRESS){
        v_eyePosition += right * deltaTime * f_MoveSpeed;
    }
    // Move up
    if (glfwGetKey( 'Q' ) == GLFW_PRESS){
        v_eyePosition -= v_eyeUp * deltaTime * f_MoveSpeed;
    }
    // Move down
    if (glfwGetKey( 'E' ) == GLFW_PRESS){
        v_eyePosition += v_eyeUp * deltaTime * f_MoveSpeed;
    }
    
    v_eyeLookAt = v_eyePosition + direction;
    
    // TODO: Down here something goes definitely very very very wrong...
    
    *m_viewMatrix= glm::lookAt(v_eyePosition,
                               v_eyeLookAt,
                               v_eyeUp);
    
    //*m_viewMatrix = glm::rotate(glm::mat4(1.0f), f_VerticalAngle, right);
    //*m_viewMatrix = glm::rotate(*m_viewMatrix, f_HorizontalAngle, v_eyeUp);
    //*m_viewMatrix = glm::translate(*m_viewMatrix, glm::vec3(-v_eyePosition.x, -v_eyePosition.y, -v_eyePosition.z));

    
   // *m_viewMatrix = glm::inverse( *CTransformAiToGlm::TransformMat4P(*m_Transform) * (*m_viewMatrix));
    
    glm::vec3 up = glm::vec3(glm::inverse(*m_viewMatrix)[1]);
    cout << up.x << " " << up.y << " " << up.z << endl;

    
    // Reset mouse position for next frame
    glfwSetMousePos(WIDTH/2, HEIGHT/2);
}

bool CCamera::viewVisible(CSceneNode* sc_rootSceneNode)
{
    //View Frustum Culling
    return false;
}

