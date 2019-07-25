#include <Poco/BasicEvent.h>
#include <Poco/Net/ServerSocket.h>
#include <iostream>

int main(int argc, char const *argv[]) {
  // Defining all the variables used in the connection
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::ServerSocket server(address);  // NOTE: The address is given here
  Poco::Net::StreamSocket connection;

  // Buffer size: 1024. Just some I checked online.
  Poco::FIFOBuffer buffer(std::size_t(1024));
  // The same size for the text, because... dunno.
  Poco::Buffer<char> text(std::size_t(1024));

  Poco::Timespan timeout_time(1, 0);

  // Starting to listen for connections
  while (true) {
    // Accepting connections to the server
    connection = server.acceptConnection();

    while (connection.impl()->initialized()) {
      // Receiving data from the connection
      connection.receiveBytes(buffer);

      if (buffer.isReadable()) {
        // If there's something in the buffer -> read the buffer
        buffer.read(text);
        std::cout << "MESSAGE FROM CLIENT: " << text.begin() << std::endl;

        if (!strcmp(text.begin(), "!q")) {
          // If the message is the exit message in client, close connection and
          // go to acceptConnection loop
          connection.close();
        }
      }
    }
  }
  return 0;
}
