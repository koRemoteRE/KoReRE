#include "H264LiveVideoSubsession.h"
#include "H264FramedSource.h"
#include "live555/H264VideoRTPSink.hh"
#include "live555/H264VideoStreamDiscreteFramer.hh"


H264LiveVideoSubsession::H264LiveVideoSubsession(UsageEnvironment& env, 
                                                 char const* fileName, 
                                                 Boolean reuseFirstSource)
                                                 : OnDemandServerMediaSubsession(env,reuseFirstSource)
{
}

H264LiveVideoSubsession* H264LiveVideoSubsession::createNew( UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource )
{
  return new H264LiveVideoSubsession(env,fileName,reuseFirstSource);
}



H264LiveVideoSubsession::~H264LiveVideoSubsession(void)
{
}

FramedSource* H264LiveVideoSubsession::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
  estBitrate = 400;
  FramedSource* h264NALSource = H264FramedSource::createNew(envir());
  return H264VideoStreamDiscreteFramer::createNew(envir(), h264NALSource);

}

RTPSink* H264LiveVideoSubsession::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
  return H264VideoRTPSink::createNew(envir(),rtpGroupsock,rtpPayloadTypeIfDynamic);
}
