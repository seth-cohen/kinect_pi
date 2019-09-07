#include "session.hpp"
#include "utility.hpp"
#include <OpenNI.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iomanip>

using namespace openni;
using namespace rapidjson;

PrintCallback::PrintCallback(const std::string& cbName, std::vector<Point> &pts)
  : name(cbName)
  , pts(pts){};

void PrintCallback::onNewFrame(VideoStream& stream) {
  std::cout << "Ready to reeeead frame" << std::endl;

  stream.readFrame(&frame);
  std::cout << "[" << std::setw(10) << frame.getTimestamp() << "]"
	    << std::setw(10) << name << " - "
	    << "sensor: " << frame.getSensorType() << std::endl;

  CoordinateConverter converter;
  int width = frame.getWidth();
  int height = frame.getHeight();

  switch (frame.getVideoMode().getPixelFormat()) {
  case PIXEL_FORMAT_DEPTH_1_MM:
  case PIXEL_FORMAT_DEPTH_100_UM: {
    DepthPixel *pDepth = (DepthPixel*)frame.getData();
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++pDepth) {
	Point &p = pts[y*width + x];
	converter.convertDepthToWorld(stream, x, y, *pDepth, &p.x, &p.y, &p.z);
      }
    }
    break;
  }
  case PIXEL_FORMAT_RGB888: {
    RGB888Pixel *pColor = (RGB888Pixel*)frame.getData();
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x, ++pColor) {
	Point &p = pts[y*width + x];
	p.r = (*pColor).r;
	p.g = (*pColor).g;
	p.b = (*pColor).b;
      }
    }
    break;
  }
  default:
    std::cout << "unkown pixel format";
  }
}


// Start the asynchronous operation
void Session::run() {
  kinect.initDevice();


  kinect.setFrameListeners(depthListener, colorListener);

  // Accept the websocket handshake
  ws_.async_accept(
    strand_.wrap(std::bind(
		   &Session::on_accept,
		   shared_from_this(),
		   std::placeholders::_1)));
}

void Session::on_accept(boost::system::error_code ec) {
  if(ec)
    return fail(ec, "accept");

  // Read a message
  do_read();
}

void Session::do_read() {
  // Read a message into our buffer
  ws_.async_read(
    buffer_,
    strand_.wrap(std::bind(
		   &Session::on_read,
		   shared_from_this(),
		   std::placeholders::_1,
		   std::placeholders::_2)));
}

void Session::on_read(
  boost::system::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  // This indicates that the Session was closed
  if(ec == websocket::error::closed)
    return;

  if(ec)
    fail(ec, "read");

  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartArray();                // Between StartArray()/EndArray(),
  for (const auto &i : points) {
    writer.StartObject();               // Between StartObject()/EndObject(), 
      
    writer.Key("x");                // output a key,
    writer.Double(i.x);             // follow by a value.

    writer.Key("y");
    writer.Double(i.y);

    writer.Key("z");
    writer.Double(i.z);

    writer.Key("r");
    writer.Uint(i.r);
      
    writer.Key("g");
    writer.Uint(i.g);

    writer.Key("b");
    writer.Uint(i.b);
  }
  writer.EndArray();
    

  buffer_.consume(buffer_.size());
  boost::beast::ostream(buffer_) << s.GetString() << std::endl;

  // write pixel values
  ws_.text(ws_.got_text());
  ws_.async_write(
    buffer_.data(),
    strand_.wrap(std::bind(
		   &Session::on_write,
		   shared_from_this(),
		   std::placeholders::_1,
		   std::placeholders::_2)));
}

void Session::on_write(
  boost::system::error_code ec,
  std::size_t bytes_transferred
) {
  boost::ignore_unused(bytes_transferred);

  if(ec)
    return fail(ec, "write");

  // Clear the buffer
  buffer_.consume(buffer_.size());

  // Do another read
  do_read();
}
