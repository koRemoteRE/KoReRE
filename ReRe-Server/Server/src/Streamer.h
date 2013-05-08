#ifndef SERVER_STREAMER_H
#define SERVER_STREAMER_H 

#include "live555/BasicUsageEnvironment.hh"
#include "live555/GroupsockHelper.hh"
#include "live555/liveMedia.hh"

class Streamer
{
public:
  static Streamer* getInstance(){
    static Streamer instance; 
    return &instance;}
  ~Streamer(void);

  void startStreaming();
  void afterPlaying(void* clientData);
private:
  Streamer(void);
  void init();
  void play();
  
  UsageEnvironment* env;
  char const* inputFileName;
  H264VideoStreamFramer* videoSource;
  RTPSink* videoSink;
};
#endif //SERVER_STREAMER_H
