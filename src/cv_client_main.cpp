#include <Poco/Delegate.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include "cv_client.hpp"

void eventReceive(void *sender, cv::Mat &image) {
  cv::imshow("Socket image", image);
  int key = cv::waitKey(20);
  if (key == 27) {
    std::cout << "Esc pressed, stopping..." << std::endl;
    CVSocketClient *client = static_cast<CVSocketClient *>(sender);
    client->Stop();
  }
}

void eventState(void *sender, CVSocketClient::ClientState &state) {
  if (state == CVSocketClient::kClientIdle) {
    std::cout << "State changed to Idle, calling exit function..." << std::endl;
    CVSocketClient *client = static_cast<CVSocketClient *>(sender);
    client->Exit();
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

  // Initialize CVSocketClient
  std::string address = "127.0.0.1:10000";
  CVSocketClient client(address);

  cv::Mat image;

  Poco::Thread client_thread;

  if (sending) {
    // Initialize OpenCV for capturing
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
      std::cout << "Could not open camera! Exiting..." << std::endl;
      return 0;
    }

    // The thread needs to be started before calling the Start...() functions,
    // because of semaphore usage
    client_thread.start(client);

    cap >> image;
    client.StartSending(image);

    // This start can be called before or after StartSending/StartReceiving

    std::cout << "Going to camera loop..." << std::endl;
    std::cout << "NOTE: Although I've tried to read the connection closing, "
                 "the sending side doesn't quite pick it up."
              << std::endl
              << "The only way to exit is Ctrl-C for now." << std::endl;
    while (true) {
      cap >> image;
      client.SendNewImage(image);
      cv::waitKey(33);
    }
    std::cout << "Exiting camera loop..." << std::endl;
  }

  if (!sending) {
    // The thread needs to be started before calling the Start...() functions,
    // because of semaphore usage
    client_thread.start(client);
    client.StartReceiving();
    client.ReceivedImage += Poco::delegate(&eventReceive);
    client.StateChanged += Poco::delegate(&eventState);

    // Nothing left to do in the main thread anymore. The client calls for a
    // single thread with function eventReceive() by itself through
    // notifyAsync() with the event. We'll just wait for the client thread to
    // crash or exit.
    client_thread.join();
  }
  return 0;
}