#include "kinect.hpp"
#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <OpenNI.h>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

// Echoes back all received WebSocket messages
class Session : public std::enable_shared_from_this<Session> {
  websocket::stream<tcp::socket> ws_;
  boost::asio::io_service::strand strand_;
  boost::beast::multi_buffer buffer_;

  openni::DepthPixel depth;
  openni::RGB888Pixel pixel;
  Kinect kinect;

public:
  // Take ownership of the socket
  explicit Session(tcp::socket socket)
    : ws_(std::move(socket))
    , strand_(ws_.get_io_service())
    , depth{99}
    , pixel{9,9,9}
  {
    std::cout << "Depth: " << depth << std::endl
	      << "R: " << (unsigned int)pixel.r
	      << ", G: " << (unsigned int)pixel.g
	      << ", B: " << (unsigned int)pixel.b << std::endl;
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
