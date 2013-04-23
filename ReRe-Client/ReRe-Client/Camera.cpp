//
//   CCamera.cpp
//  ReRe-Client
//
//  Created by Ina Schröder on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include "CCamera.h"

#include <math.h>
#include <stdio.h>
#include <vector>

CCamera::CCamera(){
    :_fMovementSpeed(50.0f),
_fFovDeg(0.0f),
_fFar(0.0f),
_fNear(0.0f),
_matView(1.0f),
_matProjection(1.0f),
_matViewInverse(1.0f),
_matViewProj(1.0f),
_fFocalLength(0.0f),
_bIsOrtho(false),
_fWidth(1.0f),
_fHeight(1.0),
_name(""),}

glm::vec3  CCamera::getSide() const {
    // Note: assume the  CCamera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 0 ]);
}

glm::vec3  CCamera::getUp() const {
    // Note: assume the  CCamera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 1 ]);
}

glm::vec3  CCamera::getForward() const {
    // Note: assume the  CCamera's view matrix is not scaled
    // (otherwise the return-vec would have to be normalized)
    return glm::vec3(_matViewInverse[ 2 ]);
}

glm::vec3  CCamera::getPosition() const {
    return glm::vec3(_matViewInverse[ 3 ]);
}

void  CCamera::setProjectionOrtho(float fLeft, float fRight, float fBottom,
                                      float fTop, float fNear, float fFar) {
    _matProjection = glm::ortho(fLeft, fRight, fBottom, fTop, fNear, fFar);
    _fFar = fFar;
    _fNear = fNear;
    _fFovDeg = -1.0f;  // Not valid in this case -> Mark as negative.
    _bIsOrtho = true;
    
    paramsChanged();
}

void  CCamera::setProjectionPersp(float yFov_deg, float fWidth,
                                      float fHeight, float fNear, float fFar) {
    _matProjection = glm::perspectiveFov(yFov_deg, fWidth,
                                         fHeight, fNear, fFar);
    _fNear = fNear;
    _fFar = fFar;
    _fFovDeg = yFov_deg;
    _bIsOrtho = false;
    _fWidth = fWidth;
    _fHeight = fHeight;
    
    // Calculate focal length
    float fFovHor2 = glm::atan(
                               getAspectRatio() * glm::tan(getFovRad() / 2.0f));
    
    _fFocalLength = 1.0f / glm::tan(fFovHor2);
    paramsChanged();
}

void  CCamera::setProjectionPersp(float yFov_deg, float fAspect,
                                      float fNear, float fFar) {
    float fWidth = 1.0f;
    float fHeight = fWidth / fAspect;
    
    _matProjection = glm::perspectiveFov(yFov_deg, fWidth, fHeight, fNear, fFar);
    _fNear = fNear;
    _fFar = fFar;
    _fFovDeg = yFov_deg;
    _bIsOrtho = false;
    _fWidth = fWidth;
    _fHeight = fHeight;
    
    // Calculate focal length
    float fFovHor2 = glm::atan(
                               getAspectRatio() * glm::tan(getFovRad() / 2.0f));
    
    _fFocalLength = 1.0f / glm::tan(fFovHor2);
    paramsChanged();
}

void  CCamera::paramsChanged() {
    _matViewProj = _matProjection * _matView;
    updateFrustumPlanes();
}


void  CCamera::rotateFromMouseMove(float dx, float dy) {
    // Ugly hack incoming here...
    static bool bFistTime = true;
    
    if (bFistTime) {
        bFistTime = false;
        return;
    }
    
    rotateViewQuat(dx, glm::vec3(0.0f, 1.0f, 0.0f));
    rotateViewQuat(dy, getSide());
}

void  CCamera::rotateViewQuat(const float angle, const glm::vec3 v3Axis) {
    /* glm::vec3 v3View = getForward();
     glm::vec3 v3Up(0.0f, 1.0f, 0.0f);
     glm::quat quatView(0, v3View);
     glm::quat quatViewResult = glm::rotate(quatView, angle, v3Axis);
     v3View.x = quatViewResult.x;
     v3View.y = quatViewResult.y;
     v3View.z = quatViewResult.z;
     v3View    = glm::normalize(v3View);
     glm::vec3 v3Side = glm::cross(v3Up, v3View);
     v3Side = glm::normalize(v3Side);
     v3Up = glm::cross(v3View, v3Side);
     v3Up = glm::normalize(v3Up); */
    
    if (_sceneNode) {
        _sceneNode->rotate(angle, v3Axis, SPACE_WORLD);
        // _sceneNode->setOrientation(v3Side, v3Up, v3View, SPACE_WORLD);
    }
}

void  CCamera::moveForward(float fSpeed) {
    if (!_sceneNode) {
        return;
    }
    
    _sceneNode->translate(-getForward() * fSpeed, SPACE_WORLD);
}

void  CCamera::moveSideways(float fSpeed) {
    if (!_sceneNode) {
        return;
    }
    
    _sceneNode->translate(getSide() * fSpeed, SPACE_WORLD);
}

void  CCamera::updateFrustumPlanes() {
    // If the  CCamera's projection matrix is an orthogonal projection,
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
        // If the  CCamera's projection matrix is a perspective projection,
        // the view-space frustum planes can be
        // determined by the proj-parameters of the  CCamera
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

bool  CCamera::isVisible(const glm::vec3& rSphereCenterWS, const float fRadius) const {
    glm::vec4 bSphereVS =  _matView * glm::vec4(rSphereCenterWS, 1.0f);
    const glm::vec3 v3Center = glm::vec3(bSphereVS);
    
    // 1) Cull if behind  CCamera
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
}
