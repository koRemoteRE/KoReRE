#ifndef SERVER_H264LIVEVIDEOSUBSESSION_H
#define SERVER_H264LIVEVIDEOSUBSESSION_H 

#include "live555/OnDemandServerMediaSubsession.hh"
class H264LiveVideoSubsession : public OnDemandServerMediaSubsession
{
public:
  static H264LiveVideoSubsession* createNew(UsageEnvironment& env, Boolean reuseFirstSource);
  EventTriggerId eventTriggerId;

protected:
  H264LiveVideoSubsession(UsageEnvironment& env, Boolean reuseFirstSource);
  virtual  ~H264LiveVideoSubsession(void);

  virtual FramedSource* createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate );
  virtual RTPSink* createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource );

private:
   RTPSink* fDummyRTPSink;
};
#endif //SERVER_H264LIVEVIDEOSUBSESSION_H
