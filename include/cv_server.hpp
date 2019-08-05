#pragma once
#ifndef CV_SERVER_HPP
#define CV_SERVER_HPP

#include <Poco/BasicEvent.h>
#include <Poco/Mutex.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <opencv2/opencv.hpp>

#include <string>

class CVSocketServer : public Poco::Runnable {
 public:
  // Declaration of the states of this class
  enum ServerState { kServerIdle, kServerSending, kServerReceiving };

  // Public functions, more info in source file
  CVSocketServer(std::string &full_address_string);
  ~CVSocketServer();
  void StartSending(cv::Mat &image);
  void StartReceiving(int cv_mat_type);
  void Stop();
  void SendNewImage(cv::Mat &image);  // Can be done with events easily too
  void run();

  // Declaration of the event for getting the received image out from this class
  Poco::BasicEvent<cv::Mat> ReceivedImage;

 private:
  // Poco socket client side declarations
  Poco::Net::SocketAddress address_;
  Poco::Net::StreamSocket connection_;
  Poco::Net::ServerSocket server_;

  // Threading declarations
  Poco::Mutex mutex_;

  // Declarations for variables containing the data sending/receiving
  cv::Mat image_;
  int image_cols_;
  int image_rows_;
  std::size_t buffer_size_;
  int received_bytes_;
  int image_ptr_;
  uchar *sock_data_;

  // Declaration of state and whether there's new image available
  bool new_image_ = false;
  ServerState state_;

  // Private functions, more info in source file
  void SendMat();
  void ReceiveMat();
  void StateMachine();
};
#endif  // CV_SERVER_HPP