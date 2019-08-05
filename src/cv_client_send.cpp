#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "cv_comm_definitions.hpp"

int main(int argc, char const *argv[]) {
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::StreamSocket connection;
  cv::Mat img;
  std::vector<uchar> img_buffer;
  std::size_t buffer_size;

  // Open connection to server
  connection.connect(address);

  // Open camera stream
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    std::cout << "Could not open camera stream, exiting..." << std::endl;
    return 0;
  }

  // Reading a single image to know the buffer size needed and sending the
  // buffer size info to server
  cap.read(img);
  cv::resize(img, img, cv::Size(VIDEO_WIDTH, VIDEO_HEIGHT));
  buffer_size = img.total() * img.elemSize();
  std::cout << "Sending buffer size: " << buffer_size << std::endl;
  connection.sendBytes(&img.cols, sizeof(img.cols));
  connection.sendBytes(&img.rows, sizeof(img.rows));

  while (connection.impl()->initialized()) {
    // Take a picture and resize it
    cap.read(img);
    cv::resize(img, img, cv::Size(VIDEO_WIDTH, VIDEO_HEIGHT));

    // Reshape the image to essentially an array and send it
    img.reshape(0, 1);
    connection.sendBytes(img.data, buffer_size);

    // Sleep for 33ms
    cv::waitKey(33);
  }
  connection.close();
  return 0;
}
