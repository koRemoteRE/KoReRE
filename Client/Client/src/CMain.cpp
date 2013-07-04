//
//  main.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//
#ifdef _WIN32 
# define _WIN32_WINNT 0x0501 
#endif 

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "CPreRendering.h"
#include "NoSerialClient.h"
#include "ImageQueue.h"
#include "MatrixQueue.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

glm::mat4 lastView;
glm::mat4 lastProj;

ImageQueue *imageQueue;
MatrixQueue *matrixQueue;

bool _oldR = false;

void MainLoop(void)
{
    CPreRendering* renderer = new CPreRendering();
   // CRemoteNetwork* network = new CRemoteNetwork();
    
    int frameCounter = 0;
    int frameThreshold = 5;
    
    unsigned short * d = new unsigned short[9];
    for(int i = 0; i < 9; i++){
        d[i] = i;
    }
    cv::Mat bla(3,3,CV_16U,d);
    
    std::cout << bla << std::endl;
    
    
    do{
        // Update Camera Matrix
        renderer->getSceneGraph()->returnCameraNode()->updateCameraView();
        
        if(frameCounter == 0){
            renderer->writeToFBO();
            frameCounter =frameThreshold;

            lastView = renderer->getViewMatrix();
            lastProj = renderer->getProjectionMatrix();
        }else{
            renderer->testWarpDraw(lastView,lastProj);

            frameCounter--;
        }



        // Swap buffers
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
		}

    } // Check if the ESC key was pressed or the window was closed
    while(true);
		/*glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );*/
    
    delete renderer;
}

void serverThread(){
	try{
		const std::string host = "192.168.2.113";
		const std::string port = "9999";

		NoSerialClient client(host, port);

	}catch (std::exception &e){

		std::cerr << e.what() << std::endl;
	}

}

int main(int argc, const char * argv[])
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // 4x antialiasing
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // OpenGL 3.2
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( WIDTH, HEIGHT, 0,0,0,0, 32,0, GLFW_WINDOW ) )
    {
        fprintf( stderr, "Failed to open GLFW window\n" );
        glfwTerminate();
        return -1;
    }

    // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glfwSetWindowTitle( "Hello World!" );
    
    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );
    
    glEnable(GL_DEPTH_TEST);
    
    // Initialize Mouse Position
    glfwDisable( GLFW_MOUSE_CURSOR);
    glfwSetMousePos(WIDTH/2, HEIGHT/2);
    
	imageQueue = ImageQueue::getInstance();
	matrixQueue = MatrixQueue::getInstance();

	boost::thread servThread = boost::thread(serverThread);

    try
    {
        MainLoop();
    }
    catch (int i_ex)
    {
        cout << "Fehlercode: " << i_ex << endl;
        if (i_ex == 0x00)
            cout << "Keine .dae-Datei" << endl;
        else if (i_ex == 0x01)
            cout << "Fehler beim Laden der Datei" << endl;
    }
    
    // Close window and terminate GLFW
    glfwTerminate();
    
    std::cout << "Hello, World!\n";
    return 0;
}

