#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "cv_comm_definitions.hpp"

int main(int argc, char const *argv[]) {
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection;
  cv::Mat img;
  std::vector<uchar> img_buffer;

  // Open camera stream
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cout << "Could not open camera stream, exiting..." << std::endl;
    return 0;
  }

  // Take a picture and resize it
  cap.read(img);
  cv::resize(img, img, cv::Size(VIDEO_WIDTH, VIDEO_HEIGHT));

  // Encode it and show its size
  cv::imencode(".webp", img, img_buffer);
  std::size_t buffer_size = img_buffer.size();

  connection.connect(address);
  connection.sendBytes(&buffer_size, sizeof(buffer_size));

  std::cout << "Encoded image size:" << img_buffer.size() << std::endl;
  std::cout << "Sending image to socket server" << std::endl;

  // Connect to the server and send the image
  connection.sendBytes(img_buffer.data(), buffer_size);
  connection.close();

  return 0;
}
