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

  // Take a picture, resize and encode it
  cap.read(img);
  cv::resize(img, img, cv::Size(VIDEO_WIDTH, VIDEO_HEIGHT));
  cv::imencode(".webp", img, img_buffer);

  // Connect to the server and send the image size info
  std::cout << "Sending image size info to serever" << std::endl;
  std::size_t buffer_size = img_buffer.size();
  connection.connect(address);
  connection.sendBytes(&buffer_size, sizeof(buffer_size));

  // Send the image to server
  // NOTE: buffer_size == img_buffer.size()
  std::cout << "Sending image to server" << std::endl;
  connection.sendBytes(img_buffer.data(), buffer_size);

  std::cout << "Image sent, closing..." << std::endl;
  connection.close();

  return 0;
}
