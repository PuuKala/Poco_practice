#include <Poco/Net/ServerSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "cv_comm_definitions.hpp"

int main(int argc, char const *argv[]) {
  // Defining all the variables used in the connection
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::ServerSocket server(address);  // NOTE: The address is given here
  Poco::Net::StreamSocket connection;
  cv::Mat img;
  std::vector<uchar> img_buffer;
  std::size_t buffer_size;

  connection = server.acceptConnection();

  // Receive buffer size as std::size_t
  connection.receiveBytes(&buffer_size, sizeof(buffer_size));
  std::cout << "Received image size:" << buffer_size << std::endl;

  // Receiving data from the connection
  connection.receiveBytes(&img_buffer, buffer_size);
  connection.close();
  std::cout << "Image actual size:" << img_buffer.size() << std::endl;
  img = cv::imdecode(img_buffer, cv::IMREAD_COLOR);
  cv::imshow("Socket image", img);
  cv::waitKey();
  return 0;
}
