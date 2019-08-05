#pragma once
#ifndef CV_CLIENT_H
#define CV_CLIENT_H

#include <Poco/BasicEvent.h>
#include <Poco/Mutex.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <opencv2/opencv.hpp>

#include <string>

class CVSocketClient : public Poco::Runnable {
 public:
  enum ClientState { kClientIdle, kClientSending, kClientReceiving };
  CVSocketClient(std::string &full_address_string);
  ~CVSocketClient();
  void StartSending(cv::Mat &image);
  void StartReceiving(int cv_mat_type);
  void Stop();
  void SendNewImage(cv::Mat &image);  // Can be done with events easily too
  void run();

  Poco::BasicEvent<cv::Mat> ReceivedImage;

 private:
  Poco::Net::SocketAddress address_;
  Poco::Net::StreamSocket connection_;
  Poco::Mutex mutex_;
  Poco::ActiveResult<cv::Mat> notify_result_;
  cv::Mat image_;
  cv::Mat received_image_;
  int image_cols_;
  int image_rows_;
  std::size_t buffer_size_;
  int received_bytes_;
  int image_ptr_;
  uchar *sock_data_;
  bool new_image_ = false;
  ClientState state_;

  void SendMat();
  void ReceiveMat();
  void StateMachine();
};
#endif  // CV_CLIENT_H