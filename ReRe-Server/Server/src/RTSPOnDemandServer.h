#ifndef SERVER_RTSPSERVER_H
#define SERVER_RTSPSERVER_H 

#include "live555/liveMedia.hh"
#include "live555/BasicUsageEnvironment.hh"

class RTSPOnDemandServer
{
public:
  RTSPOnDemandServer(void);
  ~RTSPOnDemandServer(void);
  void startStreaming(char* watch);
  void streamFrame();
private:
  UsageEnvironment* env;

};
#endif //SERVER_RTSPSERVER_H
