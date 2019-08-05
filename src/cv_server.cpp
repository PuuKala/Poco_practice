#include "cv_server.hpp"
#include <iostream>

CVSocketServer::CVSocketServer(std::string &full_address_string)
    : address_(full_address_string), server_(address_), state_(kServerIdle) {}

CVSocketServer::~CVSocketServer() {
  if (connection_.impl()->initialized()) {
    connection_.close();
  }
  if (server_.impl()->initialized())
  {
    server_.close();
  }  
}

void CVSocketServer::StartSending(cv::Mat &image) {
  connection_ = server_.acceptConnection();
  connection_.sendBytes(&image.cols, sizeof(image.cols));
  connection_.sendBytes(&image.rows, sizeof(image.rows));
  std::cout << "Sent image size: " << image.cols << "x" << image.rows
            << std::endl;
  buffer_size_ = image.total() * image.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  std::cout << "Starting to send..." << std::endl;
  state_ = kServerSending;
}

void CVSocketServer::StartReceiving(int cv_mat_type) {
  connection_ = server_.acceptConnection();
  connection_.receiveBytes(&image_cols_, sizeof(image_cols_));
  connection_.receiveBytes(&image_rows_, sizeof(image_rows_));
  std::cout << "Received image size: " << image_cols_ << "x" << image_rows_
            << std::endl;
  image_ = cv::Mat::zeros(cv::Size2i(image_cols_, image_rows_), cv_mat_type);
  buffer_size_ = image_.total() * image_.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  sock_data_ = new uchar[buffer_size_];
  std::cout << "Starting to receive..." << std::endl;
  state_ = kServerReceiving;
}

void CVSocketServer::Stop() {
  mutex_.lock();
  std::cout << "Stopping..." << std::endl;
  connection_.close();
  state_ = kServerIdle;
  mutex_.unlock();
}

void CVSocketServer::SendNewImage(cv::Mat &image) {
  if (mutex_.tryLock()) {
    // Copy the image to private image_ in order to be sure the private image_
    // isn't indirectly modified elsewhere without mutex_
    image.copyTo(image_);
    new_image_ = true;
    mutex_.unlock();
  }
}

void CVSocketServer::SendMat() {
  if (new_image_) {
    mutex_.lock();
    image_.reshape(0, 1);
    connection_.sendBytes(image_.data, buffer_size_);
    new_image_ = false;
    mutex_.unlock();
  }
}

void CVSocketServer::ReceiveMat() {
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
    ReceivedImage.notifyAsync(this, image_);
  }
}

void CVSocketServer::StateMachine() {
  switch (state_) {
    case kServerIdle:
      std::cout << "Waiting for start function call..." << std::endl;
      break;

    case kServerSending:
      SendMat();
      break;

    case kServerReceiving:
      ReceiveMat();
      break;
  }
}

void CVSocketServer::run() {
  while (true) {
    StateMachine();
  }
}