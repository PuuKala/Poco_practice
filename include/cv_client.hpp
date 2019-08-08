#pragma once
#ifndef CV_CLIENT_HPP
#define CV_CLIENT_HPP

#include <Poco/BasicEvent.h>
#include <Poco/Mutex.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <opencv2/opencv.hpp>

#include <string>

class CVSocketClient : public Poco::Runnable {
 public:
  // Declaration of the states of this class
  enum ClientState { kClientIdle, kClientSending, kClientReceiving };

  // Public functions, more info in source file
  CVSocketClient(std::string &full_address_string);
  ~CVSocketClient();
  void StartSending(cv::Mat &image);
  void StartReceiving();
  void Stop();
  void Exit();
  void SendNewImage(cv::Mat &image);  // Can be done with events easily too
  void run();

  // Declaration of the event for getting the received image out from this class
  Poco::BasicEvent<cv::Mat> ReceivedImage;
  Poco::BasicEvent<ClientState> StateChanged;

 private:
  // Poco socket client side declarations
  Poco::Net::SocketAddress address_;
  Poco::Net::StreamSocket connection_;

  // Threading declarations
  Poco::Mutex mutex_;
  Poco::Semaphore sema_;

  // Declarations for variables containing the data sending/receiving
  cv::Mat image_;
  int image_cols_;
  int image_rows_;
  std::size_t buffer_size_;
  int received_bytes_;
  uchar *sock_data_;

  // Declaration of state and whether there's new image available
  bool new_image_ = false;
  bool running_ = true;
  ClientState state_;

  // Private functions, more info in source file
  void SendMat();
  bool CheckConnection();
  void ReceiveMat();
  void StateMachine();
};
#endif  // CV_CLIENT_HPP