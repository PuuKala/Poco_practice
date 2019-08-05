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
  int image_cols;
  int image_rows;
  std::size_t buffer_size;
  std::size_t rec_bytes;

  // The variables for the image
  cv::Mat img;
  int image_ptr = 0;

  // Wait for connection and receive the buffer size from the client
  std::cout << "Server ready for connection" << std::endl;
  connection = server.acceptConnection();
  connection.receiveBytes(&image_cols, sizeof(image_cols));
  connection.receiveBytes(&image_rows, sizeof(image_rows));
  img = cv::Mat::zeros(cv::Size(image_cols, image_rows), CV_8UC3);
  buffer_size = img.total() * img.elemSize();
  std::cout << "Received buffer size information. Buffer size: " << buffer_size
            << std::endl;
  uchar *sockData = new uchar[buffer_size];

  while (connection.impl()->initialized()) {
    // Receiving data for a complete image from the connection
    std::cout << "Receiving " << buffer_size << " bytes..." << std::endl;
    for (std::size_t i = 0; i < buffer_size; i += rec_bytes) {
      rec_bytes = connection.receiveBytes(sockData + i, buffer_size - i);
      std::cout << rec_bytes << " bytes received, " << buffer_size - i << " bytes left" << std::endl;
    }

    // Reconstructing the raw cv::Mat from socketData
    image_ptr = 0;
    for (std::size_t i = 0; i < image_rows; i++) {
      for (std::size_t j = 0; j < image_cols; j++) {
        img.at<cv::Vec3b>(i, j) =
            cv::Vec3b(sockData[image_ptr], sockData[image_ptr + 1],
                      sockData[image_ptr + 2]);
        image_ptr += 3;
      }
    }

    // Show image
    if (!img.empty()) {
      cv::imshow("Socket image", img);
      cv::waitKey(1);  // Required for imshow
    }
  }
  connection.close();
  return 0;
}
