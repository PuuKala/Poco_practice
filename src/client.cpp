#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <string>

int main(int argc, char const *argv[]) {
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection;

  // Defining the same buffer sizes as in server
  Poco::FIFOBuffer buffer(std::size_t(1024));
  Poco::Buffer<char> text(std::size_t(1024));

  // Greet message to the server
  text.assign("Hello from client!", 1024);

  // Running cin loop
  bool running = true;
  std::string input;

  connection.connect(address);

  buffer.write(text);
  connection.sendBytes(buffer);

  while (running) {
    std::cout << "Write something for the server. Write !q to quit."
              << std::endl;
    std::cin >> input;
    text.assign(input.c_str(), 1024);
    buffer.write(text);
    connection.sendBytes(buffer);
    if (input == "!q") {
      std::cout << "Exiting..." << std::endl;
      running = false;
    }
  }

  connection.close();

  return 0;
}
