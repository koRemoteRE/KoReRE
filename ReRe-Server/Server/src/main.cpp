/*
 Copyright (c) 2012 The KoReRE Project

  This file is part of KoReRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoReRE.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>



#include "NoSerialServer.h"
#include "Encoder.h"
#include "KoRE/Timer.h"
#include "Scene.h"
#include "ImageQueue.h"
#include "MatrixQueue.h"
#include "SerializableMatrix.hpp"
#include "SerializableImage.hpp"
#include "KoRE/Components/Camera.h"
#include "KoRE/SceneManager.h"
#include "KoRE/GLerror.h"
#include "KoRE/RenderManager.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>


Encoder* encoder;
const int _screenWidth = 1280;
const int _screenHeight = 720;

ImageQueue *imageQueue;
MatrixQueue *matrixQueue;

unsigned int currID = 0;

void init(){
   // Initialize GLFW
  if (!glfwInit()) {
    kore::Log::getInstance()->write("[ERROR] could not load window manager\n");
    exit(EXIT_FAILURE);
  }

  /*glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);*/

  // Open an OpenGL window
  if (!glfwOpenWindow(_screenWidth,_screenHeight, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
    kore::Log::getInstance()->write("[ERROR] could not open render window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  kore::RenderManager::getInstance()->setScreenResolution(glm::ivec2(_screenWidth,_screenHeight));

  // disable v-sync
  glfwSwapInterval(0);

  // initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit()) {
    kore::Log::getInstance()->write("[ERROR] could not open initialize extension manager\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
    
  // log versions
  int GLFWmajor, GLFWminor, GLFWrev;
  glfwGetVersion(&GLFWmajor, &GLFWminor, &GLFWrev);
  kore::Log::getInstance()
    ->write("Render Device: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_RENDERER)));
  kore::Log::getInstance()
    ->write("Vendor: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_VENDOR)));
  kore::Log::getInstance()
    ->write("OpenGL version: %s\n",
            reinterpret_cast<const char*>(
            glGetString(GL_VERSION)));
  kore::Log::getInstance()
    ->write("GLSL version: %s\n",
             reinterpret_cast<const char*>(
             glGetString(GL_SHADING_LANGUAGE_VERSION)));
  kore::Log::getInstance()
    ->write("GLFW version %i.%i.%i\n",
             GLFWmajor, GLFWminor, GLFWrev);
  kore::Log::getInstance()
    ->write("GLEW version: %s\n",
            reinterpret_cast<const char*>(
            glewGetString(GLEW_VERSION)));

  encoder = new Encoder();
  imageQueue = ImageQueue::getInstance();
  matrixQueue = MatrixQueue::getInstance();
}

void renderOnDemand(Scene* scene, SerializableMatrix transformation){

  scene->getCam()->getSceneNode()->setTransform(transformation.mat);
  SceneManager::getInstance()->update();
  kore::GLerror::gl_ErrorCheckStart();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
  kore::RenderManager::getInstance()->renderFrame();
  kore::GLerror::gl_ErrorCheckFinish("Main Loop");
  glfwSwapBuffers();
  
  SerializableImage imPkt;

  //imPkt.id = id;
  imPkt.matrix = transformation;
  imPkt.image = encoder->encodeFrame();

  imageQueue->push(imPkt);

  std::cout << "pushed Image size: " << imPkt.image->size() << std::endl;

  //std::cout << "queue: " << imageQueue->getLenght() << std::endl;
}

void serverThread(){
  try{
    unsigned short port = 9999;
    NoSerialServer server(port);

  }catch (std::exception &e){

    std::cerr << e.what() << std::endl;
  }
}

int main(void) {

  int running = GL_TRUE;
  init();

  Scene scene;
  scene.init();

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;

  kore::Camera* pCamera = scene.getCam();
  pCamera->setProjectionPersp(60,(16.0/9.0),1,1000);
  float cameraMoveSpeed = 4.0f;

  int oldMouseX = 0;
  int oldMouseY = 0;
  glfwGetMousePos(&oldMouseX,&oldMouseY);

  bool _oldR = false;
  
  boost::thread servThread = boost::thread(serverThread);
  // Main loop
  while (running) {
    time = the_timer.timeSinceLastCall();
    scene.update(time);
  
    if (glfwGetKey(GLFW_KEY_UP) || glfwGetKey('W')) {
      pCamera->moveForward(cameraMoveSpeed * static_cast<float>(time));
    }

    if (glfwGetKey(GLFW_KEY_DOWN) || glfwGetKey('S')) {
      pCamera->moveForward(-cameraMoveSpeed * static_cast<float>(time));
    }

    if (glfwGetKey(GLFW_KEY_LEFT) || glfwGetKey('A')) {
      pCamera->moveSideways(-cameraMoveSpeed * static_cast<float>(time));
    }

    if (glfwGetKey(GLFW_KEY_RIGHT) || glfwGetKey('D')) {
      pCamera->moveSideways(cameraMoveSpeed * static_cast<float>(time));
    }
    if (glfwGetKey('U')) {
      pCamera->getSceneNode()->setTranslation(glm::vec3(0,0,0));
    }

    int mouseX = 0;
    int mouseY = 0;
    glfwGetMousePos(&mouseX,&mouseY);

    int mouseMoveX = mouseX - oldMouseX;
    int mouseMoveY = mouseY - oldMouseY;

    if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS ) {
      if (glm::abs(mouseMoveX) > 0 || glm::abs(mouseMoveY) > 0) {
        pCamera->rotateFromMouseMove((float)-mouseMoveX / 5.0f,
          (float)-mouseMoveY / 5.0f);
      }
    }

    oldMouseX = mouseX;
    oldMouseY = mouseY;
   
    //lightNode->rotate(-40.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f), kore::SPACE_WORLD);

    if (glfwGetKey('R')) {
      if (!_oldR) {
        _oldR = true;
        //ENCODER TEST 
        //pCamera->moveForward(-cameraMoveSpeed * static_cast<float>(time));
        SceneManager::getInstance()->update();
        kore::GLerror::gl_ErrorCheckStart();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
        kore::RenderManager::getInstance()->renderFrame();
        kore::GLerror::gl_ErrorCheckFinish("Main Loop");
        glfwSwapBuffers();
        encoder->encodeFrame();
      }
    } else {
      _oldR = false;
    }

  SerializableMatrix transformMat;

  if(matrixQueue->tryPop(transformMat)){

    //std::cout << "popped Matrix Id: " << transformMat.id << std::endl;
    renderOnDemand(&scene, transformMat);
  }

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  glfwPollEvents();
  }
  
  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
