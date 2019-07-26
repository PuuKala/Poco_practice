#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketStream.h>
#include <iostream>

int main(int argc, char const *argv[]) {
  // Defining all the variables used in the connection
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::ServerSocket server(address);  // NOTE: The address is given here
  Poco::Net::StreamSocket connection;
  char buffer[1024];
  int bytes;

  std::cout << "Server ready for connection" << std::endl;

  // Accepting connections to the server
  connection = server.acceptConnection();

  std::cout << "Connection established" << std::endl;

  while (connection.impl()->initialized()) {
    // Receive bytes until buffer is full
    bytes = connection.receiveBytes(buffer, sizeof(buffer) - 1);

    // If bytes received in the message is 0, it's the closing of the client
    if (bytes == 0) {
      std::cout << "Received 0 byte message, interpreting that as exit "
                   "message, closing connection..."
                << std::endl;
      connection.close();
    } else {
      // Ending the buffer char with \0 to signal the end of char
      buffer[bytes] = '\0';
      std::cout << "Client sent: " << buffer << std::endl;
    }
  }
  return 0;
}
