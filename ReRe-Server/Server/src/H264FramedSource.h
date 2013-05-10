#ifndef _DEVICE_SOURCE_HH
#define _DEVICE_SOURCE_HH

#ifndef _FRAMED_SOURCE_HH
#include "live555/FramedSource.hh"
#endif
#include "Encoder.h"

// The following class can be used to define specific encoder parameters
class DeviceParameters {
  //%%% TO BE WRITTEN %%%
};

class H264FramedSource: public FramedSource {
public:
  static H264FramedSource* createNew(UsageEnvironment& env);

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.

protected:
  H264FramedSource(UsageEnvironment& env);
  // called only by createNew(), or by subclass constructors
  virtual ~H264FramedSource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

private:
  static void deliverFrame0(void* clientData);
  void deliverFrame();

private:
  static unsigned referenceCount; // used to count how many instances of this class currently exist
};

#endif
