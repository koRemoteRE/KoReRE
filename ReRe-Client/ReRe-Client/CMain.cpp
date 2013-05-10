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

#include "Packet.h"

//inlcude
extern "C" {
#include <libav/libavcodec/avcodec.h>
#include <libav/libavformat/avformat.h>
#include <libav/libswscale/swscale.h>
#include <libav/libavutil/log.h>
#include <libav/libavutil/avutil.h>
}

//find BUG!
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
    
    // call this once at the start of the program
   // av_log_set_callback(&log);



//____________________prepare sample video________________________________________
    //initialize by calling “av_register_al()"
    static std:: once_flag initFlag;
    std::call_once(initFlag, []() { av_register_all(); });
    
    //CONTEXT
    //allocate an AVFormatContext structure. use a shared_ptr to make it exception-safe --> don’t forget to release it later!
    std::shared_ptr<AVFormatContext> avFormat(avformat_alloc_context(), &avformat_free_context);
    
    //open file and parse its header
    auto avFormatPtr = avFormat.get();
    if (avformat_open_input(&avFormatPtr, "/Users/inaschroeder/Documents/Uni/SoSe2013/Forschungspraktikum/Code/KoReRE/ReRe-Client/test.AVI", nullptr, nullptr) != 0)
        throw std::runtime_error("Error while calling avformat_open_input (probably invalid file format)");
    
    if (avformat_find_stream_info(avFormat.get(), nullptr) < 0)
        throw std::runtime_error("Error while calling avformat_find_stream_info");

    //suche passenden codec zur geöffneten Datei
    AVStream* videoStream = nullptr;
    for (int i = 0; i < avFormat->nb_streams; ++i) {
        
        if (avFormat->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            // stream gefunden.
            videoStream = avFormat->streams[i];
            break;
        }
    }
    if (!videoStream)
        throw std::runtime_error("Didn't find any video stream in the file (probably audio file)");
    
    
    //initialize an AVCodecContext structure, which is where the library will store informations throughout the decoding process.
    // getting the required codec structure
    const auto codec = avcodec_find_decoder(videoStream->codec->codec_id);
    if (codec == nullptr) //ginge auch !codec?
        throw std::runtime_error("Codec required by video file not available");
    
    // allocating a structure - CODEC
    std::shared_ptr<AVCodecContext> avVideoCodec(avcodec_alloc_context3(codec), [](AVCodecContext* c) { avcodec_close(c); av_free(c); });
    
    // we need to make a copy of videoStream->codec->extradata and give it to the context
    // make sure that this vector exists as long as the avVideoCodec exists
    
    //Vector Typ richtig? (selbst ausgewählt)
    std::vector<uint8_t> codecContextExtraData(videoStream->codec->extradata, videoStream->codec->extradata + videoStream->codec->extradata_size);
    avVideoCodec->extradata = reinterpret_cast<uint8_t*>(codecContextExtraData.data());
    avVideoCodec->extradata_size = codecContextExtraData.size();
    
    // initializing the structure by opening the codec
    if (avcodec_open2(avVideoCodec.get(), codec, nullptr) < 0)
        throw std::runtime_error("Could not open codec");
    
    //________bis hier OK_________//
    
    //__________packet Teil Anfang______
    // READ DATA
    // allocating an AVFrame
    std::shared_ptr<AVFrame> avFrame(avcodec_alloc_frame(), &av_free);

   AVPacket avpacket;
    // libavformat mallocs a buffer with the data and stores a pointer to it inside AVPacket
    if (av_read_frame(avFormat.get(), &avpacket) < 0)
        return;// we are at EOF
    //obigen X-Teil ersetzen
    
    // the current packet of data
    Packet packet; //versuche AVPacket statt packet
    // data in the packet of data already processed
    size_t offsetInData = 0;
    
    // the decoding loop, running until EOF
    while (true) {
        // reading a packet using libavformat
        
        if (offsetInData >= packet.packet.size) {
            do {
                packet.reset(avFormat.get());
                if (avpacket.stream_index != videoStream->index)//anders im tutorial
                    continue;
            } while(0);
        }
        
        // preparing the packet that we will send to libavcodec
        AVPacket packetToSend;
        
        packetToSend.data = packet.packet.data + offsetInData;
        packetToSend.size = packet.packet.size - offsetInData + FF_INPUT_BUFFER_PADDING_SIZE;
        //__________packet Teil Ende_____
        
        AVStream* videostream = NULL;
        AVCodec* videocodec = NULL;
        for (int i = 0; i < avFormat->nb_streams; i++)
        {
            switch (avFormat->streams[i]->codec->codec_type)
            {
                case AVMEDIA_TYPE_VIDEO:
                    videostream = avFormat->streams[i];
                    break;
            }
        }
        
        if (videostream)
        {
            videocodec = avcodec_find_decoder(videostream->codec->codec_id);
            
            if (!videocodec)
            {
                printf("avcodec cannot find decoder for codec id %d", videostream->codec->codec_id);
                
                return 1;
            }
            
            if (avcodec_open2(videostream->codec, videocodec, NULL) != 0)
            {
                printf("avcodec cannot open decoder for codec id %d", videostream->codec->codec_id);
                
                return 1;
            }
        }
        //___________
        // sending data to libavcodec
        int isFrameAvailable = 0; //frame available = true
        
        //PROBLEM HIER
        //Anmerkung: kein Argument ist nullptr -> überprüft
        const int processedLength = avcodec_decode_video2(avVideoCodec.get(), avFrame.get(), &isFrameAvailable, &packetToSend);
        
        std::cout << "do we finally get here?!" << std::endl;
    
        if (processedLength < 0) {
            av_free_packet(&avpacket);
            throw std::runtime_error("Error while processing the data");
        }
        
        offsetInData += processedLength;
        
        // processing the image if available
        if (isFrameAvailable) {
            // display image on the screen
            
            // sleeping until next frame
            const auto msToWait = avVideoCodec->ticks_per_frame * 1000 * avVideoCodec->time_base.num / avVideoCodec->time_base.den;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
        }
    }
    
    AVPicture pic;
    avpicture_alloc(&pic, PIX_FMT_RGB24, avFrame->width, avFrame->height);
    auto ctxt = sws_getContext(avFrame->width, avFrame->height, static_cast<PixelFormat>(avFrame->format), avFrame->width, avFrame->height, PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (ctxt == nullptr)
        throw std::runtime_error("Error while calling sws_getContext");
    sws_scale(ctxt, avFrame->data, avFrame->linesize, 0, avFrame->height, pic.data, pic.linesize);
    
    // pic.data[0] now contains the image data in RGB format (3 bytes)
    // and pic.linesize[0] is the pitch of the data (ie. size of a row in memory, which can be larger than width*sizeof(pixel))
    
    // we can for example upload it to an OpenGL texture (note: untested code)
    // glBindTexture(GL_TEXTURE_2D, myTex);
    // for (int i = 0; i < avFrame->height; ++i) {
    // 	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, i, avFrame->width, 1, GL_RGB, GL_UNSIGNED_BYTE, avFrame->data[0] + (i * pic.linesize[0]));
    // }
    
    avpicture_free(&pic);
    
//_____________________________________________________________________________
    
    //Decoder second try
    
    
    
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
        
        glfwSetWindowTitle( "Hello World!" );
        
        // Ensure we can capture the escape key being pressed below
        glfwEnable( GLFW_STICKY_KEYS );
        
        do{
            // Draw nothing, see you in tutorial 2 !
            
            // Swap buffers
            glfwSwapBuffers();
            
        } // Check if the ESC key was pressed or the window was closed
        while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
              glfwGetWindowParam( GLFW_OPENED ) );
    
    
        
    // Close window and terminate GLFW
    glfwTerminate();
    
    std::cout << "Hello, World!\n";
    return 0;
}


//schaue nach:

/*
 1) It looks like you should pass it the AVCodecContext* straight from videoStream->codec, rather than allocating a new (blank) structure.
 
 2)
 
 
 */
