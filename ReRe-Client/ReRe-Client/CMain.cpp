//
//  main.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "CPreRendering.h"
#include "NoSerialClient.h"
#include "MatrixQueue.h"
#include "ImageQueue.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

glm::mat4 lastView;
glm::mat4 lastProj;

MatrixQueue *matrixQueue;
ImageQueue *imageQueue;

void MainLoop(void)
{
    bool camMatrixUpdated = false;
    int numOfUpdates = 0;
    
    CPreRendering* renderer = new CPreRendering();
    
    lastView = renderer->getViewMatrix();
	lastProj = glm::perspectiveFov(60.0 ,1280.0 ,720.0 ,1.0 ,1000.0);
    SerializableImage image;
    SerializableMatrix mat;
    
    //Änderungen:
    // - mat-Variable verschoben
    // - xx durch lastView ersetzt
    // - alte lastView entfernt
    // - zusätzlichen Puffer (imgBuffer) eingefügt
    
	double d_LastTime = glfwGetTime();;

	double updateTime = 0;
	double maxUpdateTime = 0.5;

	double accumulator = 0;
	double frameTime = 0.05;

    do{
		
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - d_LastTime);
		d_LastTime = currentTime;

		accumulator += deltaTime;
		camMatrixUpdated = false;

		while(accumulator >= frameTime){
			// Update Camera Matrix
			camMatrixUpdated = renderer->getSceneGraph()->returnCameraNode()->updateCameraView(frameTime);
			accumulator -= frameTime;
		}   
        //renderer->writeToFBO();
        
        if (imageQueue->tryPop(image))
        {
            renderer->setServerTexture(*image.image);
            lastView = glm::inverse(image.matrix.mat);
            //cout << "Server Image" << endl;
        }
        
		if (camMatrixUpdated == true){
			updateTime += deltaTime;

			if(updateTime > maxUpdateTime){
			//std::cout << updateTime << std::endl;

			// send Matrix to Server
			mat.mat = glm::inverse(renderer->getViewMatrix());
                
			matrixQueue->push(mat);
			//cout << "SendMatrix" << endl;
			updateTime = 0;
			}
		}else{
			if(updateTime < maxUpdateTime){
				mat.mat = glm::inverse(renderer->getViewMatrix());
                
				matrixQueue->push(mat);
			}
			updateTime = maxUpdateTime;
		}
        
        if (camMatrixUpdated == true){
            numOfUpdates++;
        
            if (numOfUpdates == FRAME_COUNT)
                numOfUpdates = 0;
        }
        
        // --- old source code --------------------------------------
        /************** NICHT LÖSCHEN, EMIL BRAUCHTS ZUM TESTEN *******************/

         /*if (glfwGetKey('R'))
         { */
			
		 //if(c % 500 == 0){

		 //	c = 0;

         //    if (frameTime == true)
         //    {
         //    mat.mat = glm::inverse( renderer->getViewMatrix() );
         //       
         //    matrixQueue->push(mat);
         //    //cout << "Send Matrix" << endl;
         //    frameTime = false;
         //    }
         //}
         //else
         //    frameTime = true;
        // ---------------------------------------------------------
        
        renderer->testWarpDraw(lastView,lastProj);
        
        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );
    
    delete renderer;
}


void serverThread(){
	try{
		boost::asio::io_service io_service;
		
		const std::string host = "141.26.66.52";
		const std::string port = "9999";
        
		clients c(new NoSerialClient(io_service, host, port));
		//NoSerialClient client(host, port);

		//io_service.run_one();
        
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
    
    matrixQueue = MatrixQueue::getInstance();
    imageQueue = ImageQueue::getInstance();
    
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

