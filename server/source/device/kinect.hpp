#ifndef _KINECT_HPP_
#define _KINECT_HPP_

#include <OpenNI.h>

class Kinect {
public:
  Kinect();
  ~Kinect();

  void initDevice();
  //void readFrameData(openni::VideoFrameRef *pFrame, bool readColor);
  void readFrames(openni::VideoFrameRef& depthFrame, openni::VideoFrameRef& colorFrame);
  
protected:
  openni::Device device;
  openni::VideoStream depthStream;
  openni::VideoStream colorStream;

  openni::VideoStream** allStreams;
};
#endif //_KINECT_HPP_
