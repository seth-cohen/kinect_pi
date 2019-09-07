#include "kinect.hpp"
#include <iostream>

using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000

int Kinect::count = 0;

Kinect::Kinect() {
  count++;
  std::cout << "Number of Kinect Instances: " << count << std::endl;

  allStreams = new VideoStream*[2];
  allStreams[0] = &depthStream;
  allStreams[1] = &colorStream;
}

Kinect::~Kinect() {
  std::cout << "shutting down" << std::endl;
  
  depthStream.stop();
  depthStream.destroy();
  
  colorStream.stop();
  colorStream.destroy();

  device.close();

  OpenNI::shutdown();
  
  delete []allStreams;
}

void Kinect::initDevice() {
  if (count > 1) {
    std::cout << "Can't initialize more than one device" << std::endl;
    return;
  }
  // Initialize OpenNI Driver
  Status rc = OpenNI::initialize();
  if (rc != STATUS_OK) {
    printf("Initialization failed\n%s\n", OpenNI::getExtendedError());
  }
 
  rc = device.open(ANY_DEVICE);
  if (rc != STATUS_OK)
  {
    printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
    return;
  }

  if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
  {
    rc = depthStream.create(device, SENSOR_DEPTH);
    if (rc != STATUS_OK)
    {
      printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
      return;
    }
  }

  if (device.getSensorInfo(SENSOR_COLOR) != NULL)
  {
    rc = colorStream.create(device, SENSOR_COLOR);
    if (rc != STATUS_OK)
    {
      printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
      return;
    }
  }

  rc = device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
  if (rc != STATUS_OK) {
    printf("Couldn't align the depth and color streams\n%s\n", OpenNI::getExtendedError());
    return;
  } else {
    printf("Image Registration success!\n");
  }

  rc = depthStream.start();
  if (rc != STATUS_OK)
  {
    printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
    return;
  }

  rc = colorStream.start();
  if (rc != STATUS_OK)
  {
    printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
    return;
  }
}

void Kinect::setFrameListeners(
  VideoStream::NewFrameListener &depthListener,
  VideoStream::NewFrameListener &colorListener
) {
  depthStream.addNewFrameListener(&depthListener);
  colorStream.addNewFrameListener(&colorListener);
}

void Kinect::readFrames(VideoFrameRef& depthFrame, VideoFrameRef& colorFrame) {
  int changedIndex;
  
  Status rc = OpenNI::waitForAnyStream(allStreams, 2, &changedIndex);
  if (rc != STATUS_OK)
  {
    printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
    return;
  }

  switch (changedIndex) {
  case 0:
    printf("-- Reading Depth Frame\n");
    rc = depthStream.readFrame(&depthFrame);
    break;
  case 1:
    printf("-- Reading Color Frame\n");
    rc = colorStream.readFrame(&colorFrame);
    break;
  default:
    printf("Error in wait\n");
    rc = STATUS_ERROR;
    break;
  }

  if (rc != STATUS_OK)
  {
    printf("Read failed!\n%s\n", OpenNI::getExtendedError());
    return;
  }
}
