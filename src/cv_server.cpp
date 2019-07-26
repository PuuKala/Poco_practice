#include <Poco/Net/ServerSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "cv_comm_definitions.hpp"

int main(int argc, char const *argv[]) {
  // Defining all the variables used in the connection
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::ServerSocket server(address);
  Poco::Net::StreamSocket connection;
  cv::Mat img;
  std::vector<uchar> img_buffer;
  std::size_t buffer_size;

  std::cout << "Server ready for connection" << std::endl;
  connection = server.acceptConnection();

  // Receive buffer size as std::size_t
  connection.receiveBytes(&buffer_size, sizeof(buffer_size) - 1);
  std::cout << "Received the image size" << std::endl;

  // A required middle piece for receiving the data from socket correctly
  uchar sockData[buffer_size];

  // Receiving data from the connection
  connection.receiveBytes(sockData, buffer_size);
  connection.close();
  std::cout << "Received the image and closed the connection" << std::endl;

  // Changing everything back to cv::Mat and showing it
  img_buffer.assign(sockData, sockData + buffer_size);
  img = cv::imdecode(img_buffer, cv::IMREAD_COLOR);
  cv::imshow("Socket image", img);
  cv::waitKey();
  return 0;
}
