#include "kinect.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <OpenNI.h>
#include <sstream>

using boost::asio::ip::tcp;
using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

VideoFrameRef colorFrame, depthFrame;

void sendDepthData(tcp::iostream& stream, Kinect& kinect) {
  kinect.readFrames(depthFrame, colorFrame);

  if (depthFrame.isValid()) {
    if (depthFrame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && depthFrame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
    {
	printf("Unexpected frame format\n");
	return;
    }

    std::cout << "We've got depth" << std::endl;
    DepthPixel* pDepth = (DepthPixel*)depthFrame.getData();

    int middleIndex = (depthFrame.getHeight()+1)*depthFrame.getWidth()/2;
    int frameHeight = depthFrame.getHeight();
    int frameWidth = depthFrame.getWidth();

    std::stringstream ss;
    ss << "{\"height\":" << frameHeight << ", \"width\":" << frameWidth << ", \"depthdata\":[";

    for (int i = 0; i < frameHeight; i++) {
      ss << "[";
      for (int j = 0; j < frameWidth; j++) {
	ss << pDepth[i * frameHeight + j] << ",";
      }
      ss << "],";
    } 
    ss << "]}" << std::endl;

    //stream << ss.str();
  }
  
  if (colorFrame.isValid()) {
    std::cout << "We've got color" << std::endl;
    RGB888Pixel* pColor = (RGB888Pixel*)colorFrame.getData();

    int middleIndex = (colorFrame.getHeight()+1)*colorFrame.getWidth()/2;
    int frameHeight = colorFrame.getHeight();
    int frameWidth = colorFrame.getWidth();

    std::stringstream ss;
    ss << "{\"height\":" << frameHeight << ", \"width\":" << frameWidth << ", \"colordata\":[";

    for (int i = 0; i < frameHeight; i++) {
      ss << "[";
      for (int j = 0; j < frameWidth; j++) {
	ss << "{\"r\":" << pColor[i * frameHeight + j].r
	   << ",\"g\":" << pColor[i * frameHeight + j].g
	   << ",\"b\":" << pColor[i * frameHeight + j].b
	   << "},";
      }
      ss << "],";
    } 
    ss << "]}" << std::endl;

    //stream << ss.str();
  }
}

int main()
{
  Kinect kinect;
  kinect.initDevice();

  try
  {
    boost::asio::io_service io_service;

    tcp::endpoint endpoint(tcp::v4(), 13);
    tcp::acceptor acceptor(io_service, endpoint);

    for (;;)
    {
      printf("looping\n");
      tcp::iostream stream;
      boost::system::error_code ec;
      acceptor.accept(*stream.rdbuf(), ec);
      if (!ec)
      {
	sendDepthData(stream, kinect);
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

