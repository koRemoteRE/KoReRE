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


CCamera::CCamera(aiCamera* aic_asCamera, aiMatrix4x4* aim_nodeTransform)
{
    m_viewMatrix = new glm::mat4(1);
    m_projectionMatrix = new glm::mat4(1);
    
    setViewMatrix(aic_asCamera, aim_nodeTransform);
    setProjectionPerspMatrix(aic_asCamera);
}

void CCamera::updateCameraView(int i_mouseWayX, int i_mouseWayY)
{
    float f_newPositionX = 0, f_newPositionY = 0, f_newPositionZ = 0;
    
    f_RotationX-=i_mouseWayX-WIDTH/2;
    f_RotationY-=i_mouseWayY-HEIGHT/2;
    
    *m_viewMatrix = glm::rotate(*m_viewMatrix, f_RotationX, glm::vec3(1,0,0));
    *m_viewMatrix = glm::rotate(*m_viewMatrix, f_RotationY, glm::vec3(0,1,0));
    *m_viewMatrix = glm::translate(*m_viewMatrix, f_newPositionX, f_newPositionY, f_newPositionZ);
}

bool CCamera::viewVisible(CSceneNode* sc_rootSceneNode)
{
    //View Frustum Culling
    return false;
}

void CCamera::setViewMatrix(aiCamera* aic_asCamera, aiMatrix4x4* aim_nodeTransform)
{
    glm::vec3 v_eyePosition = glm::vec3(aic_asCamera->mPosition.x,
                                        aic_asCamera->mPosition.y,
                                        aic_asCamera->mPosition.z);
    
    glm::vec3 v_eyeLookAt = glm::vec3(aic_asCamera->mLookAt.x,
                                      aic_asCamera->mLookAt.y,
                                      aic_asCamera->mLookAt.z);
    
    glm::vec3 v_eyeUp = glm::vec3(aic_asCamera->mUp.x,
                                  aic_asCamera->mUp.y,
                                  aic_asCamera->mUp.z);
    
    *m_viewMatrix = glm::lookAt(v_eyePosition, v_eyeLookAt, v_eyeUp);
    
    //Transformation mit Kamerakoordinaten kombinieren
    *m_viewMatrix = glm::inverse( *CTransformAiToGlm::TransformMat4P(*aim_nodeTransform) * (*m_viewMatrix) );
}

void CCamera::setProjectionPerspMatrix(aiCamera* aic_asCamera)
{
    float fov = glm::degrees(aic_asCamera->mHorizontalFOV)/ aic_asCamera->mAspect;
//  float aspect = (float)WIDTH/HEIGHT;
    *m_projectionMatrix = glm::perspective(fov,
                                           aic_asCamera->mAspect,
                                           aic_asCamera->mClipPlaneNear,
                                           aic_asCamera->mClipPlaneFar);
}


