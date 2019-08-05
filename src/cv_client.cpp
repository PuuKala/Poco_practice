#include "cv_client.hpp"
#include <iostream>

CVSocketClient::CVSocketClient(std::string &full_address_string)
    : address_(full_address_string),
      state_(kClientIdle),
      notify_result_(ReceivedImage.notifyAsync(this, image_)) {}

CVSocketClient::~CVSocketClient() {
  if (connection_.impl()->initialized()) {
    connection_.close();
  }
}

void CVSocketClient::StartSending(cv::Mat &image) {
  connection_.connect(address_);
  connection_.sendBytes(&image.cols, sizeof(image.cols));
  connection_.sendBytes(&image.rows, sizeof(image.rows));
  std::cout << "Sent image size: " << image.cols << "x" << image.rows
            << std::endl;
  buffer_size_ = image.total() * image.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  std::cout << "Starting to send..." << std::endl;
  state_ = kClientSending;
}

void CVSocketClient::StartReceiving(int cv_mat_type) {
  connection_.connect(address_);
  connection_.receiveBytes(&image_cols_, sizeof(image_cols_));
  connection_.receiveBytes(&image_rows_, sizeof(image_rows_));
  std::cout << "Received image size: " << image_cols_ << "x" << image_rows_
            << std::endl;
  image_ = cv::Mat::zeros(cv::Size2i(image_cols_, image_rows_), cv_mat_type);
  buffer_size_ = image_.total() * image_.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  sock_data_ = new uchar[buffer_size_];
  std::cout << "Starting to receive..." << std::endl;
  state_ = kClientReceiving;
}

void CVSocketClient::Stop() {
  mutex_.lock();
  std::cout << "Stopping..." << std::endl;
  connection_.close();
  state_ = kClientIdle;
  mutex_.unlock();
}

void CVSocketClient::SendNewImage(cv::Mat &image) {
  if (mutex_.tryLock()) {
    // Copy the image to private image_ in order to be sure the private image_
    // isn't indirectly modified elsewhere without mutex_
    image.copyTo(image_);
    new_image_ = true;
    mutex_.unlock();
  }
}

void CVSocketClient::SendMat() {
  if (new_image_) {
    mutex_.lock();
    image_.reshape(0, 1);
    connection_.sendBytes(image_.data, buffer_size_);
    new_image_ = false;
    mutex_.unlock();
  }
}

void CVSocketClient::ReceiveMat() {
  if (!mutex_.tryLock()) {
    std::cout << "Could not lock mutex for receiving data!" << std::endl;
    return;
  }

  received_bytes_ = 0;
  for (std::size_t i = 0; i < buffer_size_; i += received_bytes_) {
    received_bytes_ =
        connection_.receiveBytes(sock_data_ + i, buffer_size_ - i);
  }

  mutex_.unlock();

  if (image_.type() == CV_8UC3) {
    image_ptr_ = 0;
    for (std::size_t i = 0; i < image_rows_; i++) {
      for (std::size_t j = 0; j < image_cols_; j++) {
        image_.at<cv::Vec3b>(i, j) =
            cv::Vec3b(sock_data_[image_ptr_], sock_data_[image_ptr_ + 1],
                      sock_data_[image_ptr_ + 2]);
        image_ptr_ += 3;
      }
    }
    // Copy in order to be safe from modifications to the Mat properties
    notify_result_.wait();
    notify_result_ = ReceivedImage.notifyAsync(this, image_);
  }
}

void CVSocketClient::StateMachine() {
  switch (state_) {
    case kClientIdle:
      std::cout << "Waiting for start function call..." << std::endl;
      break;

    case kClientSending:
      SendMat();
      break;

    case kClientReceiving:
      ReceiveMat();
      break;
  }
}

void CVSocketClient::run() {
  while (true) {
    StateMachine();
  }
}