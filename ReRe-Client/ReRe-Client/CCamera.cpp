//
//   Camera.cpp
//  ReRe-Client
//
//  Created by Ina Schröder on 23.04.13.
//

#include "CCamera.h"


CCamera::CCamera()
{
    d_LastTime = glfwGetTime();
    
    m_viewMatrix = new glm::mat4(1);
    m_projectionMatrix = new glm::mat4(1);
    
    // Intrinsische Kameraparameter definieren
    stcpi_intrinsic.f_fieldOfView   = 27.6380615;
    stcpi_intrinsic.f_aspect        = 1.7777;
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

bool CCamera::updateCameraView()
{
    bool cameraUpdated = false;
    
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
    if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS || glfwGetKey( 'W' ) == GLFW_PRESS){
        stcv_update.v_eyePosition += direction * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    // Move backward
    if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS || glfwGetKey( 'S' ) == GLFW_PRESS){
        stcv_update.v_eyePosition -= direction * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    
    // Strafe right
    if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS || glfwGetKey( 'D' ) == GLFW_PRESS){
        stcv_update.v_eyePosition += right * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    // Strafe left
    if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey( 'A' ) == GLFW_PRESS){
        stcv_update.v_eyePosition -= right * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    
    // Move up
    if (glfwGetKey( 'Q' ) == GLFW_PRESS){
        stcv_update.v_eyePosition += stcv_update.v_eyeUp * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    // Move down
    if (glfwGetKey( 'E' ) == GLFW_PRESS){
        stcv_update.v_eyePosition -= stcv_update.v_eyeUp * deltaTime * stcmp_update.f_moveSpeed;
        cameraUpdated = true;
    }
    
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
    
    if (cameraUpdated == true)
        return true;
    else
        return false;
}

bool CCamera::viewVisible(CSceneNode* sc_rootSceneNode)
{
    //View Frustum Culling
    return false;
}

