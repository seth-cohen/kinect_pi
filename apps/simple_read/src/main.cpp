#include "listener.hpp"

#include <string>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <functional>
#include <thread>

#include <boost/asio/signal_set.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>


int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 4)
    {
      std::cerr <<
	"Usage: simple_read <address> <port> <threads>\n" <<
	"Example:\n" <<
	"    simple_read 0.0.0.0 8080 1\n";
      return EXIT_FAILURE;
    }
    auto const address = boost::asio::ip::address::from_string(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<std::size_t>(1, std::atoi(argv[3]));

    boost::asio::io_service io_service;

    // Ability to handle the sig int for shutdown
    boost::asio::signal_set signals(io_service);
    signals.async_wait(std::bind(&boost::asio::io_service::stop, &io_service));

    // Create and launch the listening port
    std::make_shared<Listener>(io_service, tcp::endpoint{address, port})->run();

    // Run the service on the number of threads
    std::vector<std::thread> v;
    for (auto i = 0; i < threads; ++i) {
      v.emplace_back([&io_service]{io_service.run();});
    }
    io_service.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Graceful" << std::endl;
  return 0;
}
