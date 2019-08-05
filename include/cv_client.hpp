#include <Poco/Net/StreamSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <opencv2/opencv.hpp>

#include <string>

class CVSocketClient : Poco::Runnable {
 public:
  enum ClientState { kClientIdle, kClientSending, kClientReceiving };
  CVSocketClient(std::string full_address_string);
  ~CVSocketClient();
  void StartSending(const int &width,const int &height);
  void StartReceiving();
  void Stop();
  void run();

 private:
  Poco::Net::SocketAddress address_;
  Poco::Net::StreamSocket connection_;
  Poco::Semaphore command_sema_;
  cv::Mat received_image_;
  int image_width_;
  int image_height_;
  ClientState state_;

  void StateMachine();
};