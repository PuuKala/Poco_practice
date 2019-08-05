#include <Poco/Net/ServerSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Mutex.h>
#include <opencv2/opencv.hpp>

class socket_srvr : public Poco::Runnable {
 private:
  Poco::Mutex mutex_;
  Poco::Net::ServerSocket server_;
  Poco::Net::StreamSocket connection_;
  cv::Mat img_;
  std::vector<uchar> img_buffer_;
  std::size_t buffer_size_;

 public:
  socket_srvr(/* args */);
  ~socket_srvr();
  void run();  // Implementation of Runnable-class
};

socket_srvr::socket_srvr(/* args */) {}

socket_srvr::~socket_srvr() {}
