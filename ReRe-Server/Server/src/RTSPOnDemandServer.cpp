#include "RTSPOnDemandServer.h"


RTSPOnDemandServer::RTSPOnDemandServer(void)
{
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);
  RTSPServer* rtspServer = RTSPServer::createNew(*env,8554);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  OutPacketBuffer::maxSize = 400000;
  char const* streamName = "h264Test";
  char const* inputFileName = "test.h264";
  ServerMediaSession* sms
    = ServerMediaSession::createNew(*env, streamName, streamName);
  sms->addSubsession(H264VideoFileServerMediaSubsession
    ::createNew(*env, inputFileName, true));


  
  rtspServer->addServerMediaSession(sms);
  char* url = rtspServer->rtspURL(sms);
  *env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
  //env->taskScheduler().createEventTrigger()
}
void RTSPOnDemandServer::streamFrame(){
  env->taskScheduler().triggerEvent(0);
}

void RTSPOnDemandServer::startStreaming(){

  env->taskScheduler().doEventLoop(); // does not return
}


RTSPOnDemandServer::~RTSPOnDemandServer(void)
{
}
