#include <Poco/Net/StreamSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

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

  // Take a picture for checking image sizes
  cap.read(img);
  std::cout << "cv::Mat size in bytes:" << std::endl
            << img.total() * img.elemSize() << std::endl;

  // Encode image and check its size
  cv::imencode(".webp", img, img_buffer);

  std::cout << "Encoded image size:" << std::endl
            << img_buffer.size() << std::endl;

  std::cout << "Sending camera stream to socket server" << std::endl;

  // Connect to the server and send a greet message
  connection.connect(address);

  while (cap.read(img)) {
    // Encode the image into the buffer and send it
    cv::imencode(".webp", img, img_buffer);
    connection.sendBytes(img_buffer.data(), img_buffer.size());
  }

  connection.close();

  return 0;
}
