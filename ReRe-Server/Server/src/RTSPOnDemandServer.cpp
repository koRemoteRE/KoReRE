#include "RTSPOnDemandServer.h"
#include "H264LiveVideoSubsession.h"



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
  //sms->addSubsession(H264LiveVideoSubsession
  //  ::createNew(*env, inputFileName, true));

  sms->addSubsession(H264VideoFileServerMediaSubsession
    ::createNew(*env, inputFileName, true));

  
  rtspServer->addServerMediaSession(sms);
  char* url = rtspServer->rtspURL(sms);
  *env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
  //env->taskScheduler().createEventTrigger()
}
void RTSPOnDemandServer::streamFrame(){
 // env->taskScheduler().triggerEvent();
}

void RTSPOnDemandServer::startStreaming(char* watch){

  env->taskScheduler().doEventLoop(watch); // does not return

}


RTSPOnDemandServer::~RTSPOnDemandServer(void)
{
}
