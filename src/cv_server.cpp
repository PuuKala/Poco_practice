#include <Poco/Net/ServerSocket.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main(int argc, char const *argv[]) {
  // Defining all the variables used in the connection
  Poco::Net::SocketAddress address("127.0.0.1:10000");
  Poco::Net::ServerSocket server(address);  // NOTE: The address is given here
  Poco::Net::StreamSocket connection;
  cv::Mat img;
  std::vector<uchar> img_buffer;
//   std::vector<cv::Mat> img_reco;

  // Starting to listen for connections
  while (true) {
    // Accepting connections to the server
    connection = server.acceptConnection();

    while (connection.impl()->initialized()) {
      // Receiving data from the connection
      connection.receiveBytes(&img_buffer, 4096);

    //   img = cv::imdecode(img_buffer, cv::ImreadModes::IMREAD_ANYDEPTH);

    //   cv::imshow("Socket image", img);
    }
  }
  return 0;
}
