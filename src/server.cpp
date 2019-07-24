#include <Poco/Net/ServerSocket.h>
#include <Poco/BasicEvent.h>
#include <iostream>

int main(int argc, char const *argv[])
{
    Poco::Net::SocketAddress address("127.0.0.1:10000");
    Poco::Net::ServerSocket server(address);
    Poco::Net::StreamSocket connection;
    Poco::FIFOBuffer buffer(std::size_t(1024));
    Poco::Buffer<char> text(std::size_t(1024));
    while (true){
        connection = server.acceptConnection();
        while (connection.impl()->initialized()){
            connection.receiveBytes(buffer);
            if (buffer.isReadable())
            {
                buffer.read(text);
                std::cout << text.begin() << std::endl;
            }
        }
    }
    // std::cout << "Fak yeeh!!1" << std::endl;
    return 0;
}
