#include "session.hpp"
#include "utility.hpp"
#include <OpenNI.h>

#include <iomanip>

using namespace openni;

class PrintCallback : public VideoStream::NewFrameListener {
public:
  PrintCallback(const std::string& cbName, void *midValue)
    :midValue(midValue)
    ,name(cbName){};

  void onNewFrame(VideoStream& stream) {
    std::cout << "Ready to reeeead frame" << std::endl;

    stream.readFrame(&frame);
    std::cout << "[" << std::setw(10) << frame.getTimestamp() << "]"
	      << std::setw(10) << name << " - "
	      << "sensor: " << frame.getSensorType() << std::endl;


    int middleIndex = (frame.getHeight() + 1) * frame.getWidth() / 2;

    switch (frame.getVideoMode().getPixelFormat()) {
    case PIXEL_FORMAT_DEPTH_1_MM:
    case PIXEL_FORMAT_DEPTH_100_UM:
      *(DepthPixel*)midValue = ((DepthPixel*)frame.getData())[middleIndex];
      break;
    case PIXEL_FORMAT_RGB888:
      *(RGB888Pixel*)midValue = ((RGB888Pixel*)frame.getData())[middleIndex];
      break;
    default:
      std::cout << "unkown pixel format";
    }
  }

protected:
  void *midValue;
  VideoFrameRef frame;
  std::string name;
};

  

// Start the asynchronous operation
void Session::run() {
  kinect.initDevice();


  PrintCallback *depthPrinter = new PrintCallback("depth", &depth);
  PrintCallback *colorPrinter = new PrintCallback("color", &pixel);
  kinect.setFrameListeners(*depthPrinter, *colorPrinter);

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

  buffer_.consume(buffer_.size());
  boost::beast::ostream(buffer_) << "Depth: " << depth << std::endl
				 << "R: " << (unsigned int)pixel.r
				 << ", G: " << (unsigned int)pixel.g
				 << ", B: " << (unsigned int)pixel.b << std::endl;

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
