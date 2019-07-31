#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <string>

int main(int argc, char const *argv[]) {
  // Defining and connecting to server
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection(address);
  std::string input;
  bool polling;

  while (true) {
    std::cout << "Send to server: ";
    std::getline(std::cin, input);

    // Write the string into the buffer and send it
    connection.sendBytes(input.data(), input.size());

    if (connection.poll(Poco::Timespan(0, 100), Poco::Net::Socket::SELECT_ERROR)) {
      std::cout << "Disconnected, exiting..." << std::endl;
      break;
    }
  }

  connection.close();

  return 0;
}
