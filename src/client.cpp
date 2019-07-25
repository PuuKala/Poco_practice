#include <Poco/Net/StreamSocket.h>
#include <iostream>

int main(int argc, char const *argv[]) {
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection;

  // Defining the same buffer sizes as in server
  Poco::FIFOBuffer buffer(std::size_t(1024));
  Poco::Buffer<char> text(std::size_t(1024));

  text.assign("Hello from client!", 1024);

  buffer.write(text);

  connection.connect(address);

  connection.sendBytes(buffer);

  connection.close();
  
  return 0;
}
