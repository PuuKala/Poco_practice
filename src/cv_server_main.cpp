#include <Poco/Delegate.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "cv_server.hpp"

void eventReceive(cv::Mat &image) {
  cv::imshow("Socket image", image);
  cv::waitKey(1);
}

int main(int argc, char const *argv[]) {
  // Handle input from command line arguments
  bool sending;
  if (argc != 2) {
    std::cout
        << "Incorrect amount of arguments given." << std::endl
        << "For receiving from the localhost hosted server, give argument r"
        << std::endl
        << "For sending to localhost hosted server, give argument s"
        << std::endl;
    return 0;
  } else if (strcmp(argv[1], "s") == 0) {
    std::cout << "Sending video from camera..." << std::endl;
    sending = true;
  } else if (strcmp(argv[1], "r") == 0) {
    std::cout << "Receiving video from server..." << std::endl;
    sending = false;
  }

  // Initialize CVSocketServer
  std::string address = "127.0.0.1:10000";
  CVSocketServer server(address);

  cv::Mat image;

  Poco::Thread server_thread;

  if (sending) {
    // Initialize OpenCV for capturing
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
      std::cout << "Could not open camera! Exiting..." << std::endl;
      return 0;
    }

    cap >> image;
    server.StartSending(image);

    // This start can be called before or after StartSending/StartReceiving
    server_thread.start(server);

    std::cout << "Going to camera loop..." << std::endl;
    while (true) {
      cap >> image;
      server.SendNewImage(image);
      cv::waitKey(33);
    }
    std::cout << "Exiting camera loop..." << std::endl;
  }

  if (!sending) {
    server.StartReceiving(CV_8UC3);
    server_thread.start(server);
    server.ReceivedImage += Poco::delegate(&eventReceive);

    // Nothing left to do in the main thread anymore. The server calls for a
    // single thread with function eventReceive() by itself through
    // notifyAsync() with the event. We'll just wait for the server thread to
    // crash or exit.
    server_thread.join();
  }
  return 0;
}