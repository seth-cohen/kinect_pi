#include <memory>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>

// Accepts incoming connections and launches the sessions
class Listener : public std::enable_shared_from_this<Listener>
{
  tcp::acceptor acceptor_;
  tcp::socket socket_;

public:
  Listener(
    boost::asio::io_service& ios,
    tcp::endpoint endpoint);

  // Start accepting incoming connections
  void run();

private:
  void do_accept();
  void on_accept(boost::system::error_code ec);
};
