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

void MainLoop(void)
{
    
    //create preRendering object with complete functionality
    CPreRendering* renderer = new CPreRendering();
    
    int frameCounter = 0;
    //update at least every 5 frames
    int frameThreshold = 5;
    
    do{
        // Update Camera Matrix
        renderer->getSceneGraph()->returnCameraNode()->updateCameraView();
        
        if(frameCounter == 0){
            renderer->writeToFBO();
            frameCounter = frameThreshold;

            lastView = renderer->getViewMatrix();
            lastProj = renderer->getProjectionMatrix();
        }else{
            renderer->testWarpDraw(lastView,lastProj);

            frameCounter--;
        }

        // Swap buffers
        glfwSwapBuffers();

    }
    // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );
    
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

    glfwSetWindowTitle( "KoRemoteRe" );
    
    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );
    
    glEnable(GL_DEPTH_TEST);
    
    // Initialize Mouse Position
    glfwDisable( GLFW_MOUSE_CURSOR);
    glfwSetMousePos(WIDTH/2, HEIGHT/2);
    
    // Singletons of imageQueue and matrixQueue
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
    return 0;
}

