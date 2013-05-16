//
//  main.cpp
//  ReRe-Client
//
//  Created by Thomas Kipshagen on 23.04.13.
//  Copyright (c) 2013 Thomas Kipshagen. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "CPreRendering.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

//_______________________________Decoder_______________________________________

#include <thread>
#include <mutex>
#include <vector>
#include <array>

//inlcude
extern "C" {
#include <libav/libavcodec/avcodec.h>
#include <libav/libavformat/avformat.h>
#include <libav/libswscale/swscale.h>
#include <libav/libavutil/log.h>
#include <libav/libavutil/avutil.h>
}

//_____________________________________________________________________________

    //log function, dump information in order to find bugs - SLOW!
    void log(void* ptr, int level, const char* fmt, va_list vl) {
        std::array<char,256> line;
        int printBuffer = 1;
        av_log(ptr, level, fmt, vl, line.data(), line.size(), &printBuffer);
        std::cout << line.data() << std::endl;
    }
    
    //_____________________________________________________________________________
    
    int main(int argc, const char * argv[])
    {
        //____________________Decoder_____________________________________________________
        
        // call log to find bugs and dump information 
        // av_log_set_callback(&log);
        
        //Register all formats and codecs once + initialize the network
        static std:: once_flag initFlag;
        std::call_once(
            initFlag, [](){
            av_register_all();
            avformat_network_init();
            }
        );
        
        //different input types for testing
        const char*  input_mp4  = "/Users/inaschroeder/Documents/Uni/SoSe2013/Forschungspraktikum/Code/KoReRE/ReRe-Client/test.mp4";
        const char*  input_avi  = "/Users/inaschroeder/Documents/Uni/SoSe2013/Forschungspraktikum/Code/KoReRE/ReRe-Client/test.avi";
        const char*  input_udp  = "udp://127.0.0.1:9111"; //make sure there is an open stream while running
        const char*  input_rstp = "rtps://127.0.0.1";
        
        //allocate an AVFormatContext structure
        AVFormatContext* avContext =nullptr;
        
        if (avformat_open_input(&avContext, input_avi, nullptr, nullptr) != 0)
            throw std::runtime_error("Error while calling avformat_open_input (probably invalid file format)");
        
        std::shared_ptr<AVFormatContext> avFormat(avContext, &avformat_free_context);
        
        if (avformat_find_stream_info(avFormat.get(), nullptr) < 0)
            throw std::runtime_error("Error while calling avformat_find_stream_info");
        
        //dump file information onto standard error
        av_dump_format(avContext, 0, argv[1], 0);
        
        //find matching codec to the opened file
        int videoStream = 1;
        for (int i = 0; i < avFormat->nb_streams; i++) {
            if (avFormat->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                // stream found
                videoStream=i;
                break;
            }
        }
        if (videoStream ==-1){
            throw std::runtime_error("Didn't find any video stream in the file (probably audio file)");
        }
        
        // Get a pointer to the codec context for the video stream
        AVCodecContext  *pCodecCtx = NULL;
        pCodecCtx = avFormat->streams[videoStream]->codec;
        
        
        // Find the decoder for the video stream
        AVCodec *pCodec = NULL;
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec == NULL) {
            fprintf(stderr, "Unsupported codec!\n");
            return -1; // Codec not found
        }
        
        // Open codec
        AVDictionary *optionsDict = NULL;
        if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0){
            fprintf(stderr, "Could not open codec!\n");
            return -1; // Could not open codec
        }
        
        // Allocate video frame
        AVFrame *pFrame = NULL;
        pFrame = avcodec_alloc_frame();

        // get picture information
        struct SwsContext *sws_ctx = NULL;
        sws_ctx =
        sws_getContext
        (
         pCodecCtx->width,
         pCodecCtx->height,
         pCodecCtx->pix_fmt,
         pCodecCtx->width,
         pCodecCtx->height,
         PIX_FMT_RGB24,
         SWS_BILINEAR,
         NULL,
         NULL,
         NULL
         );
        
        // Read frames
        // initialize packet, set data to NULL, let the demuxer fill it
        AVPacket packet;
        
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;
        int frameFinished;
        
        // ______Show frame using GLFW_________
        
        // Initialise GLFW
        if( !glfwInit() )
        {
            fprintf( stderr, "Failed to initialize GLFW\n" );
            return -1;
        }
  
        // Open a window and create its OpenGL context
        if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
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
        
        glfwSetWindowTitle( "Show Frame" );
        
        // Enable keyboard (for Escaping)
        glfwEnable( GLFW_STICKY_KEYS );
        
        // bind the frame as a texture to the window
        int texture=0;
        glGenTextures(1,(GLuint*)&texture);
        glBindTexture(GL_TEXTURE_2D,texture);
        
        int i =  0;
        do{            
            if (av_read_frame(avContext, &packet)>=0) {
                // Is this a packet from the video stream?
                if(packet.stream_index==videoStream) {
                    
                    // Decode video frame
                    avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,&packet);
                    
                    // Did we get a video frame?
                    if(frameFinished) {
                        //std::cout << "frame " << i << " finished" << std::endl; // geht auch nicht
                        i++;

                        //allocate a picture
                        AVPicture pict;
                        std::vector<unsigned char> tmpImage(pFrame->linesize[0]*pCodecCtx->height*3);
                    
                        pict.data[0]=&tmpImage[0];
                        pict.data[1]=&tmpImage[pFrame->linesize[0]*pCodecCtx->height];
                        pict.data[2]=&tmpImage[pFrame->linesize[0]*pCodecCtx->height*2];
                        pict.linesize[0]=pict.linesize[1]=pict.linesize[2]=pFrame->linesize[0];
                    
                        sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pict.data, pict.linesize);
                        
                        glBindTexture(GL_TEXTURE_2D, texture);
                        
                        static bool first=true;
                        if (first)
                        {
                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pFrame->linesize[0], pCodecCtx->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                            first=false;
                        }
                        
                        glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, pFrame->linesize[0], pCodecCtx->height, GL_RGB, GL_UNSIGNED_BYTE, &tmpImage[0]);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        
                    }
                    // Free the packet that was allocated by av_read_frame
                    av_free_packet(&packet);
                }
            }
            
            //TODO: CONVERT TO OPGENGL 3.0!
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glMatrixMode (GL_MODELVIEW);
            glPushMatrix ();
            glLoadIdentity ();
            glMatrixMode (GL_PROJECTION);
            glPushMatrix ();
            glLoadIdentity ();
            
            glColor3f(1,1,1);
            glBindTexture(GL_TEXTURE_2D, texture);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glTexCoord2f(0,1);
            glVertex3f(-1,-1,-1);
            
            glTexCoord2f(1,1);
            glVertex3f(1,-1,-1);
            
            glTexCoord2f(1,0);
            glVertex3f(1,1,-1);
            
            glTexCoord2f(0,0);
            glVertex3f(-1,1,-1);
            
            glEnd();
            
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            
            // Swap buffers
            glfwSwapBuffers();
            //usleep(500*1000);
            
        }
        // Check if the ESC key was pressed or the window was closed
        while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ) );
        
        // Free the YUV frame
          av_free(pFrame);
        
        // Close the codec
          avcodec_close(pCodecCtx);
        
        // Close the video file
        //avformat_close_input(&avFormat);
        
        // Close window and terminate GLFW
        glfwTerminate();
        return 0;
    }