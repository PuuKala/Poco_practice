#include "cv_client.hpp"
#include <iostream>

CVSocketClient::CVSocketClient(std::string full_address_string)
    : address_(full_address_string), command_sema_(1), state_(kClientIdle) {}

CVSocketClient::~CVSocketClient() {
  if (connection_.impl()->initialized()) {
    connection_.close();
  }
}

void CVSocketClient::StartSending(const int &width, const int &height) {
  connection_.connect(address_);
  image_width_ = width;
  image_height_ = height;
  connection_.sendBytes(&image_width_, sizeof(image_width_));
  connection_.sendBytes(&image_height_, sizeof(image_height_));
  std::cout << "Sent image size: " << image_width_ << "x" << image_height_
            << std::endl;
  state_ = kClientSending;
  command_sema_.set();
}

void CVSocketClient::StartReceiving() {
  connection_.connect(address_);
  connection_.receiveBytes(&image_width_, sizeof(image_width_));
  connection_.receiveBytes(&image_height_, sizeof(image_height_));
  std::cout << "Received image size: " << image_width_ << "x" << image_height_
            << std::endl;
  received_image_ =
      cv::Mat::zeros(cv::Size2i(image_width_, image_height_), CV_8UC3);
  state_ = kClientReceiving;
  command_sema_.set();
}

void CVSocketClient::Stop() {
  connection_.close();
  state_ = kClientIdle;
}

void CVSocketClient::StateMachine() {
  switch (state_) {
    case kClientIdle:
      command_sema_.wait();
      break;

    case kClientSending:
      break;

    case kClientReceiving:
      break;
  }
}

void CVSocketClient::run() {
  while (true) {
    StateMachine();
  }
}