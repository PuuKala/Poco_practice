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
  std::size_t buffer_size;
  std::size_t rec_bytes;

  // The variables for the image
  cv::Mat img;
  int image_ptr = 0;

  // Wait for connection and receive the buffer size from the client
  std::cout << "Server ready for connection" << std::endl;
  connection = server.acceptConnection();
  connection.receiveBytes(&buffer_size, sizeof(buffer_size));
  std::cout << "Received buffer size information. Buffer size: " << buffer_size
            << std::endl;
  uchar sockData[buffer_size];

  // Reserving memory for 3 channel 2D image
  img = cv::Mat::zeros(cv::Size(VIDEO_WIDTH, VIDEO_HEIGHT), CV_8UC3);

  while (connection.impl()->initialized()) {
    // Receiving data for a complete image from the connection
    for (std::size_t i = 0; i < buffer_size; i += rec_bytes) {
      rec_bytes = connection.receiveBytes(sockData + i, buffer_size - i);
    }

    // Reconstructing the raw cv::Mat from socketData
    image_ptr = 0;
    for (std::size_t i = 0; i < VIDEO_HEIGHT; i++) {
      for (std::size_t j = 0; j < VIDEO_WIDTH; j++) {
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
