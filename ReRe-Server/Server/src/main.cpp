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

#include "NoSerialClient.h"
#include "KoRE/Timer.h"
#include "ImageQueue.h"
#include "MatrixQueue.h"
//#include "SerializableMatrix.hpp"
//#include "SerializableImage.hpp"
#include "KoRE/Components/Camera.h"
#include "KoRE/SceneManager.h"
#include "KoRE/GLerror.h"
#include "KoRE/RenderManager.h"



const int _screenWidth = 800;
const int _screenHeight = 600;

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

  //encoder = new Encoder();
  imageQueue = ImageQueue::getInstance();
  matrixQueue = MatrixQueue::getInstance();
}
//typedef boost::shared_ptr<Client> client_ptr;
//typedef boost::shared_ptr<boost::asio::io_service> service_ptr;
void serverThread(){
	try{
		const std::string host = "192.168.2.113";
		const std::string port = "9999";

		//boost::asio::io_service io_service;

		NoSerialClient client(host, port);
		//io_service.run_one();

		/*while(true){
			if(matrixQueue->getLenght() > 0){
				io_service.reset();
				io_service.run();
			}		
		}*/

	}catch (std::exception &e){

		std::cerr << e.what() << std::endl;
	}

}

int main(void) {

  int running = GL_TRUE;
  init();

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;

  int oldMouseX = 0;
  int oldMouseY = 0;
  glfwGetMousePos(&oldMouseX,&oldMouseY);

  bool _oldR = false;
  
  boost::thread servThread = boost::thread(serverThread);

  // Main loop
  while (running) {
    time = the_timer.timeSinceLastCall();

    int mouseX = 0;
    int mouseY = 0;
    glfwGetMousePos(&mouseX,&mouseY);

    int mouseMoveX = mouseX - oldMouseX;
    int mouseMoveY = mouseY - oldMouseY;

    oldMouseX = mouseX;
    oldMouseY = mouseY;
   
    //lightNode->rotate(-40.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f), kore::SPACE_WORLD);
    glfwSwapBuffers();
    if (glfwGetKey('R')) {
      if (!_oldR) {
        _oldR = true;
		
		//for testing
			SerializableMatrix mat;
			//mat.id = currID++;
			mat.mat = glm::mat4();

			mat.mat[0][0] = 0.68588f;
			mat.mat[0][1] = 0.727634f;
			mat.mat[0][2] = -0.0108167f;
			mat.mat[0][3] = 0;

			mat.mat[1][0] = -0.31737f;
			mat.mat[1][1] = 0.312468f;
			mat.mat[1][2] = 0.895343f;
			mat.mat[1][3] = 0;

			mat.mat[2][0] = 0.654862f;
			mat.mat[2][1] = -0.610665f;
			mat.mat[2][2] = 0.445245f;
			mat.mat[2][3] = 0;

			mat.mat[3][0] = 6.84897f;
			mat.mat[3][1] = -5.91815f;
			mat.mat[3][2] = 4.91385f;
			mat.mat[3][3] = 1;

			//std::cout << "pushed Matrix Id: " << mat.id << std::endl;

			matrixQueue->push(mat);
      }
    } else {
      _oldR = false;
    }

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

  }
  
  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
