#ifndef SERVER_RTSPSERVER_H
#define SERVER_RTSPSERVER_H 

#include "live555/liveMedia.hh"
#include "live555/BasicUsageEnvironment.hh"
#include "H264LiveVideoSubsession.h"
#include "H264FramedSource.h"

class RTSPOnDemandServer
{
public:
  RTSPOnDemandServer(void);
  ~RTSPOnDemandServer(void);
  void startStreaming(char* watch);
  void streamFrame();
private:
  UsageEnvironment* env;
  H264VideoStreamDiscreteFramer *h264VideoStreamDiscreteFramer;

};
#endif //SERVER_RTSPSERVER_H
