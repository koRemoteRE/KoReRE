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

class Camera
{
public:
    Camera();
    ~Camera();
    
    inline const std::string& getName() const {return _name;}
    inline void setName(const std::string& name) {_name = name;}
    
    inline const glm::mat4&  getView() const           {return _matView;}
    inline const glm::mat4&  getProjection() const   {return _matProjection;}
    inline const glm::mat4   getViewProjection() const {return _matViewProj;}
    
    inline glm::mat4 const   getViewInvT() const
    {return glm::transpose(glm::inverse(_matView));}
    
    inline glm::mat4 const   getViewInv() const
    {return glm::inverse(_matView);}
    
    inline glm::mat4 const   getViewInvInvT() const
    {return glm::inverse(getViewInvT());}
    
    inline glm::mat4 const   getViewT() const
    {return glm::transpose(_matView);}
    
    inline void              setFarPlane(float fFar) {_fFar = fFar;}
    inline float             getFarPlane() const       {return _fFar;}
    inline float             getNearPlane() const      {return _fNear;}
    
    inline float             getFovRad() const
    {return glm::radians(_fFovDeg);}
    
    inline float             getFovDeg() const         {return _fFovDeg;}
    
    inline float             getAspectRatio() const
    {return _fWidth / _fHeight;}
    
    glm::vec3 getPosition() const;
    glm::vec3 getSide() const;
    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    
    void      moveForward(float fSpeed);
    void      moveSideways(float fSpeed);
    
    bool      isVisible(const glm::vec3& rSphereCenterWS,
                        const float fRadius) const;
    
    void      setProjectionPersp(float yFov_deg, float fWidth,
                                 float fHeight, float fNear, float fFar);
    
    void      setProjectionPersp(float yFov_deg, float fAspect,
                                 float fNear, float fFar);
    
    void      setProjectionOrtho(float fLeft, float fRight,
                                 float fBottom, float fTop,
                                 float fNear, float fFar);
    
    std::vector<glm::vec3>  getWSfrustumCorners();
    void      rotateFromMouseMove(float dx, float dy);
    
private:
    enum EFrustumPlane {
        PLANE_LEFT = 0,
        PLANE_RIGHT,
        PLANE_BOTTOM,
        PLANE_TOP,
        PLANE_NEAR,
        PLANE_FAR
    };
    
    glm::mat4   _matView;
    glm::mat4   _matViewInverse;
    glm::mat4   _matProjection;
    glm::mat4   _matViewProj;
    
    float       _fFovDeg;
    float       _fFar;
    float       _fNear;
    glm::vec4   _v4FrustumPlanesVS[6];
    float       _fFocalLength;
    bool        _bIsOrtho;
    float       _fWidth;
    float       _fHeight;
    
    void        updateFrustumPlanes();
    void        paramsChanged();
    void        rotateViewQuat(const float angle, const glm::vec3 v3Axis);
    
    std::string _name;
};


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
    CCamera(aiCamera* aic_asCamera);
    
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
