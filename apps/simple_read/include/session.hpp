#include "kinect.hpp"
#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <OpenNI.h>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>
using namespace openni;

struct Point {
  float x, y, z;
  uint8_t r, g, b;
};

class PrintCallback : public VideoStream::NewFrameListener {
public:
  PrintCallback(const std::string& cbName, std::vector<Point> &pts);
  void onNewFrame(VideoStream& stream);

protected:
  VideoFrameRef frame;
  std::string name;
  std::vector<Point> &pts;
};

// Echoes back all received WebSocket messages
class Session : public std::enable_shared_from_this<Session> {
  websocket::stream<tcp::socket> ws_;
  boost::asio::io_service::strand strand_;
  boost::beast::multi_buffer buffer_;

  Kinect kinect;
  std::vector<Point> points;
  PrintCallback depthListener;
  PrintCallback colorListener;

public:
  // Take ownership of the socket
  explicit Session(tcp::socket socket)
    : ws_(std::move(socket))
    , strand_(ws_.get_io_service())
    , points{640 * 480, {0.0, 0.0, 0.0, 0, 0, 0}}
    , depthListener("depth", points)
    , colorListener("color", points)
  {
  }

  // Start the asynchronous operation
  void run();

protected:

  void on_accept(boost::system::error_code ec);
  void do_read();
  void on_read(
    boost::system::error_code ec,
    std::size_t bytes_transferred
  );
  void on_write(
    boost::system::error_code ec,
    std::size_t bytes_transferred
  );
};
