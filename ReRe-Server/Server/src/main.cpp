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

#include "KoRE/GLerror.h"
#include "KoRE/ShaderProgram.h"
#include "KoRE/Components/MeshComponent.h"
#include "KoRE/Components/TexturesComponent.h"
#include "KoRE/Operations/RenderMesh.h"
#include "KoRE/Operations/BindOperations/BindAttribute.h"
#include "KoRE/Operations/BindOperations/BindUniform.h"
#include "KoRE/Operations/BindOperations/BindTexture.h"
#include "KoRE/Operations/UseFBO.h"
#include "KoRE/Operations/UseShaderProgram.h"
#include "KoRE/ResourceManager.h"
#include "KoRE/RenderManager.h"
#include "KoRE/Components/Camera.h"
#include "KoRE/SceneNode.h"
#include "KoRE/Timer.h"
#include "KoRE/Texture.h"
#include "KoRE/FrameBuffer.h"
#include "Kore/Passes/FrameBufferStage.h"
#include "Kore/Passes/ShaderProgramPass.h"
#include "KoRE/Passes/NodePass.h"
#include "KoRE/Events.h"
#include "Kore/Operations/OperationFactory.h"
#include "encoder.h"

#include "live555/BasicUsageEnvironment.hh"
#include "live555/GroupsockHelper.hh"
#include "live555/liveMedia.hh"
#include "Streamer.h"

kore::SceneNode* rotationNode = NULL;
kore::SceneNode* lightNode = NULL;
kore::Camera* pCamera = NULL;

void setUpSimpleRendering(kore::SceneNode* renderNode, kore::ShaderProgramPass*
                          programPass, kore::Texture* texture, 
                          kore::LightComponent* light) {

        kore::NodePass* nodePass = new kore::NodePass;
        const kore::ShaderProgram* simpleShader = 
            programPass->getShaderProgram();
        kore::MeshComponent* pMeshComponent =
            static_cast<kore::MeshComponent*>
            (renderNode->getComponent(kore::COMPONENT_MESH));

        // Add Texture
        kore::GLerror::gl_ErrorCheckStart();
        kore::TexturesComponent* pTexComponent = new kore::TexturesComponent;
        pTexComponent->addTexture(texture);
        renderNode->addComponent(pTexComponent);
        kore::GLerror::gl_ErrorCheckFinish("Initialization");

              
        kore::BindAttribute* normAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_normal"),
            simpleShader->getAttribute("v_normal"));

        kore::BindAttribute* uvAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_uv0"),
            simpleShader->getAttribute("v_uv0"));

        // Bind Uniform-Ops
        kore::BindUniform* modelBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("model Matrix"), simpleShader->getUniform("model"));

        kore::BindUniform* viewBind =
            new kore::BindUniform(pCamera->getShaderData("view Matrix"),
            simpleShader->getUniform("view"));

        kore::BindUniform* projBind =
            new kore::BindUniform(pCamera->getShaderData("projection Matrix"),
            simpleShader->getUniform("projection"));

        kore::BindTexture* texBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(texture->getName()),
            simpleShader->getUniform("tex"));

        kore::BindUniform* lightPosBind =
            new kore::BindUniform(light->getShaderData("position"),
            simpleShader->getUniform("pointlightPos"));

        kore::RenderMesh* pRenderOp = new kore::RenderMesh();
        pRenderOp->connect(pMeshComponent);

        nodePass->addOperation(kore::OperationFactory::create(kore::OP_BINDATTRIBUTE, "v_position", pMeshComponent, "v_position", simpleShader));
        nodePass->addOperation(normAttBind);
        nodePass->addOperation(uvAttBind);
        nodePass->addOperation(modelBind);
        nodePass->addOperation(viewBind);
        nodePass->addOperation(projBind);
        nodePass->addOperation(lightPosBind);
        nodePass->addOperation(texBind);
        nodePass->addOperation(pRenderOp);

        programPass->addNodePass(nodePass);
}

void setUpNMRendering(kore::SceneNode* renderNode, 
                      kore::ShaderProgramPass* programPass, 
                      kore::Texture* texture,
                      kore::Texture* normalmap,
                      kore::LightComponent* light) {

        kore::NodePass* nodePass = new kore::NodePass;
        const kore::ShaderProgram* nmShader = 
            programPass->getShaderProgram();
        kore::MeshComponent* pMeshComponent =
            static_cast<kore::MeshComponent*>
            (renderNode->getComponent(kore::COMPONENT_MESH));

        // Add Texture
        kore::GLerror::gl_ErrorCheckStart();
        kore::TexturesComponent* pTexComponent = new kore::TexturesComponent;
        pTexComponent->addTexture(texture);
        pTexComponent->addTexture(normalmap);
        renderNode->addComponent(pTexComponent);
        kore::GLerror::gl_ErrorCheckFinish("Initialization");

        // Bind Attribute-Ops
        kore::BindAttribute* posAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_position"),
            nmShader->getAttribute("v_position"));

        kore::BindAttribute* normAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_normal"),
            nmShader->getAttribute("v_normal"));

        kore::BindAttribute* tangentAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_tangent"),
            nmShader->getAttribute("v_tangent"));

        kore::BindAttribute* uvAttBind =
            new kore::BindAttribute(pMeshComponent->getShaderData("v_uv0"),
            nmShader->getAttribute("v_uv0"));

        // Bind Uniform-Ops
        kore::BindUniform* modelBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("model Matrix"), nmShader->getUniform("model"));

        kore::BindUniform* normalMatBind = 
            new kore::BindUniform(renderNode->getTransform()->
            getShaderData("normal Matrix"), nmShader->getUniform("normal"));

        kore::BindUniform* viewBind =
            new kore::BindUniform(pCamera->getShaderData("view Matrix"),
            nmShader->getUniform("view"));

        kore::BindUniform* invViewBind =
          new kore::BindUniform(pCamera->getShaderData("inverse view Matrix"),
          nmShader->getUniform("viewI"));

        kore::BindUniform* projBind =
            new kore::BindUniform(pCamera->getShaderData("projection Matrix"),
            nmShader->getUniform("projection"));


        kore::BindTexture* texBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(texture->getName()),
            nmShader->getUniform("tex"));

        kore::BindTexture* normalmapBind =
            new kore::BindTexture(pTexComponent->
            getShaderData(normalmap->getName()),
            nmShader->getUniform("normalmap"));

        kore::BindUniform* lightPosBind =
            new kore::BindUniform(light->getShaderData("position"),
            nmShader->getUniform("pointlightPos"));

        kore::RenderMesh* pRenderOp = new kore::RenderMesh();
        pRenderOp->connect(pMeshComponent);

        nodePass->addOperation(posAttBind);
        nodePass->addOperation(normAttBind);
        nodePass->addOperation(tangentAttBind);
        nodePass->addOperation(uvAttBind);
        nodePass->addOperation(modelBind);
        nodePass->addOperation(normalMatBind);
        nodePass->addOperation(viewBind);
        nodePass->addOperation(invViewBind);
        nodePass->addOperation(projBind);
        nodePass->addOperation(lightPosBind);
        nodePass->addOperation(texBind);
        nodePass->addOperation(normalmapBind);
        nodePass->addOperation(pRenderOp);

        programPass->addNodePass(nodePass);
}


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
  if (!glfwOpenWindow(800, 600, 8, 8, 8, 8, 24, 8, GLFW_WINDOW)) {
    kore::Log::getInstance()->write("[ERROR] could not open render window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // disable v-sync
  glfwSwapInterval(0);

  // initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit()) {
    kore::Log::getInstance()->write("[ERROR] could not open initialize extension manager\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // Init gl-states
  // glEnable(GL_VERTEX_ARRAY);
    
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
}

void initScene(){
  // enable culling and depthtest
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);


  // load shader
  kore::ShaderProgram* simpleShader = new kore::ShaderProgram;
  simpleShader->loadShader("./assets/shader/normalColor.vp",
    GL_VERTEX_SHADER);
  simpleShader->loadShader("./assets/shader/normalColor.fp",
    GL_FRAGMENT_SHADER);
  simpleShader->init();
  simpleShader->setName("cooler Shader");

  kore::ShaderProgram* nmShader = new kore::ShaderProgram;
  nmShader->loadShader("./assets/shader/normalmapping.vert", 
    GL_VERTEX_SHADER);
  nmShader->loadShader("./assets/shader/normalmapping.frag",
    GL_FRAGMENT_SHADER);
  nmShader->init();
  simpleShader->setName("normal mapping Shader");
  // load resources
  kore::ResourceManager::getInstance()
    ->loadScene("./assets/meshes/TestEnv.dae");
  //->loadScene("./assets/meshes/triangle.dae");

  // texture loading
  kore::Texture* testTexture =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/Crate.png");

  kore::Texture* stoneTexture =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/stonewall.png");

  kore::Texture* stoneNormalmap =
    kore::ResourceManager::getInstance()->
    loadTexture("./assets/textures/stonewall_NM_height.png");

  // find camera
  kore::SceneNode* pCameraNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_CAMERA);
  pCamera = static_cast<kore::Camera*>(
    pCameraNode->getComponent(kore::COMPONENT_CAMERA));

  // find light
  lightNode = kore::SceneManager::getInstance()
    ->getSceneNodeByComponent(kore::COMPONENT_LIGHT);
  kore::LightComponent* pLight = static_cast<kore::LightComponent*>(
    lightNode->getComponent(kore::COMPONENT_LIGHT));

  // select render nodes
  std::vector<kore::SceneNode*> vRenderNodes;
  kore::SceneManager::getInstance()->
    getSceneNodesByComponent(kore::COMPONENT_MESH, vRenderNodes);


  GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
  kore::FrameBufferStage* backBufferStage = new kore::FrameBufferStage;
  backBufferStage->setFrameBuffer(kore::FrameBuffer::BACKBUFFER,
    GL_FRAMEBUFFER,
    drawBuffers,
    1);

  kore::ShaderProgramPass* shaderProgPass = new kore::ShaderProgramPass;
  //shaderProgPass->setShaderProgram(simpleShader);
  shaderProgPass->setShaderProgram(nmShader);

  // init operations
  for (uint i = 0; i < vRenderNodes.size(); ++i) {

    //setUpSimpleRendering(vRenderNodes[i],shaderProgPass,testTexture,pLight);
    setUpNMRendering(vRenderNodes[i],shaderProgPass,stoneTexture,stoneNormalmap,pLight);

  }

  backBufferStage->addProgramPass(shaderProgPass);

  kore::RenderManager::getInstance()->addFramebufferStage(backBufferStage);

  std::vector<kore::SceneNode*> vBigCubeNodes;
  kore::SceneManager::getInstance()
    ->getSceneNodesByName("Cube", vBigCubeNodes);
  rotationNode = vBigCubeNodes[0]; 

  glClearColor(1.0f,1.0f,1.0f,1.0f);
}

UsageEnvironment* env;
char const* inputFileName = "slamtv60.264";
H264VideoStreamFramer* videoSource;
RTPSink* videoSink;

void play(); // forward

void afterPlaying(void* /*clientData*/) {
  *env << "...done reading from file\n";
  videoSink->stopPlaying();
  Medium::close(videoSource);
  // Note that this also closes the input file that this source read from.

  // Start playing once again:
  play();
}

void play() {
  // Open the input file as a 'byte-stream file source':
  ByteStreamFileSource* fileSource
    = ByteStreamFileSource::createNew(*env, inputFileName);
  if (fileSource == NULL) {
    *env << "Unable to open file \"" << inputFileName
      << "\" as a byte-stream file source\n";
    exit(1);
  }

  FramedSource* videoES = fileSource;

  // Create a framer for the Video Elementary Stream:
  videoSource = H264VideoStreamFramer::createNew(*env, videoES);

  // Finally, start playing:
  *env << "Beginning to read from file...\n";
  videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}

int main(void) {

  int running = GL_TRUE;
  
  init();
  initScene();

  

  kore::Timer the_timer;
  the_timer.start();
  double time = 0;
  float cameraMoveSpeed = 4.0f;

  int oldMouseX = 0;
  int oldMouseY = 0;
  glfwGetMousePos(&oldMouseX,&oldMouseY);
  
  Encoder* encoder = new Encoder();
  encoder->init("test2.h264",800,600);
  
  
  Streamer* streamer = Streamer::getInstance();

  // Main loop
  while (running) {
    time = the_timer.timeSinceLastCall();
    kore::SceneManager::getInstance()->update();

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

    if (glfwGetKey(GLFW_KEY_F1)) {
      encoder->start();
    }
    if (glfwGetKey(GLFW_KEY_F2)) {
      encoder->stop();
    }
    if (glfwGetKey(GLFW_KEY_F3)) {
      streamer->startStreaming();
      //encoder->finish();
    }

    if (glfwGetKey(GLFW_KEY_F4)) {
      // Begin by setting up our usage environment:
      TaskScheduler* scheduler = BasicTaskScheduler::createNew();
      env = BasicUsageEnvironment::createNew(*scheduler);

      // Create 'groupsocks' for RTP and RTCP:
      struct in_addr destinationAddress;
      destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
      // Note: This is a multicast address.  If you wish instead to stream
      // using unicast, then you should use the "testOnDemandRTSPServer"
      // test program - not this test program - as a model.

      const unsigned short rtpPortNum = 18888;
      const unsigned short rtcpPortNum = rtpPortNum+1;
      const unsigned char ttl = 255;

      const Port rtpPort(rtpPortNum);
      const Port rtcpPort(rtcpPortNum);

      Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
      rtpGroupsock.multicastSendOnly(); // we're a SSM source
      Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
      rtcpGroupsock.multicastSendOnly(); // we're a SSM source

      // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
      OutPacketBuffer::maxSize = 400000;
      videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

      // Create (and start) a 'RTCP instance' for this RTP sink:
      const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
      const unsigned maxCNAMElen = 100;
      unsigned char CNAME[maxCNAMElen+1];
      gethostname((char*)CNAME, maxCNAMElen);
      CNAME[maxCNAMElen] = '\0'; // just in case
      RTCPInstance* rtcp
        = RTCPInstance::createNew(*env, &rtcpGroupsock,
        estimatedSessionBandwidth, CNAME,
        videoSink, NULL /* we're a server */,
        True /* we're a SSM source */);
      // Note: This starts RTCP running automatically

      RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
      if (rtspServer == NULL) {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
      }
      ServerMediaSession* sms
        = ServerMediaSession::createNew(*env, "testStream", inputFileName,
        "Session streamed by \"testH264VideoStreamer\"",
        True /*SSM*/);
      sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
      rtspServer->addServerMediaSession(sms);

      char* url = rtspServer->rtspURL(sms);
      *env << "Play this stream using the URL \"" << url << "\"\n";
      delete[] url;

      // Start the streaming:
      *env << "Beginning streaming...\n";
      play();

      env->taskScheduler().doEventLoop(); // does not return
    }

    oldMouseX = mouseX;
    oldMouseY = mouseY;

    if (rotationNode) {
      rotationNode->rotate(90.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    //lightNode->rotate(-40.0f * static_cast<float>(time), glm::vec3(0.0f, 0.0f, 1.0f), kore::SPACE_WORLD);

    kore::GLerror::gl_ErrorCheckStart();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_STENCIL_BUFFER_BIT);
    kore::RenderManager::getInstance()->renderFrame();

    glfwSwapBuffers();
    encoder->encodeFrame();
    kore::GLerror::gl_ErrorCheckFinish("Main Loop");

    // Check if ESC key was pressed or window was closed
    running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
  }

  // Test XML writing
  kore::ResourceManager::getInstance()->saveProject("xmltest.kore");
  

  delete encoder;

  // Close window and terminate GLFW
  glfwTerminate();

  // Exit program
  exit(EXIT_SUCCESS);
};
