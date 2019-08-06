#include <Poco/Delegate.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "cv_server.hpp"

void eventReceive(void *sender, cv::Mat &image) {
  cv::imshow("Socket image", image);
  int key = cv::waitKey(20);
  if (key == 27) {
    std::cout << "Esc pressed, stopping..." << std::endl;
    CVSocketServer *server = static_cast<CVSocketServer *>(sender);
    server->Stop();
  }
}

void eventState(void *sender, CVSocketServer::ServerState &state) {
  if (state == CVSocketServer::kServerIdle) {
    std::cout << "State changed to Idle, calling exit function..." << std::endl;
    CVSocketServer *server = static_cast<CVSocketServer *>(sender);
    server->Exit();
  }
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

    server_thread.start(server);

    cap >> image;
    server.StartSending(image);

    std::cout << "Going to camera loop..." << std::endl;
    std::cout << "NOTE: Although I've tried to read the connection closing, "
                 "the sending side doesn't quite pick it up."
              << std::endl
              << "The only way to exit is Ctrl-C for now." << std::endl;
    while (true) {
      cap >> image;
      server.SendNewImage(image);
      cv::waitKey(33);
    }
    std::cout << "Exiting camera loop..." << std::endl;
  }

  if (!sending) {
    server_thread.start(server);
    server.StartReceiving(CV_8UC3);
    server.ReceivedImage += Poco::delegate(&eventReceive);
    server.StateChanged += Poco::delegate(&eventState);
    std::cout << "Press esc while the window is active to stop" << std::endl;

    // Nothing left to do in the main thread anymore. The server calls for a
    // single thread with function eventReceive() by itself through
    // notifyAsync() with the event. We'll just wait for the server thread to
    // crash or exit.
    server_thread.join();
  }
  return 0;
}