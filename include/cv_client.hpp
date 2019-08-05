#include <Poco/Net/StreamSocket.h>
#include <Poco/Runnable.h>
#include <Poco/Semaphore.h>
#include <opencv2/opencv.hpp>

#include <string>

class CVSocketClient : Poco::Runnable {
 private:
  Poco::Net::SocketAddress address_;
  Poco::Net::StreamSocket connection_;
  Poco::Semaphore command_sema_;

 public:
  CVSocketClient(std::string full_address_string);
  ~CVSocketClient();
  void run();
};