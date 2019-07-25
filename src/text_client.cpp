#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <string>

int main(int argc, char const *argv[]) {
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection;
  std::string input;

  // Defining the same buffer sizes as in server
  Poco::FIFOBuffer buffer(std::size_t(1024));

  // Connect to the server and send a greet message
  connection.connect(address);
  buffer.write("Hello from client!", 1024);
  connection.sendBytes(buffer);

  while (true) {
    std::cout << "Write something for the server. Write !q to quit."
              << std::endl;
    std::getline(std::cin, input);

    // Write the string into the buffer and send it
    // Text size defined the same as the buffer because... dunno what that
    // actually is
    buffer.write(input.c_str(), 1024);
    connection.sendBytes(buffer);

    // If the input is !q, quit
    if (input == "!q") {
      std::cout << "Exiting..." << std::endl;
      break;
    }
  }

  connection.close();

  return 0;
}
