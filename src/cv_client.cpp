/**
 * @file cv_client.cpp
 * @brief The definitions of CVSocketClient class
 *
 * @author Sami Karkinen
 */
#include "cv_client.hpp"
#include <iostream>

/**
 * @brief Non-functional constructor, initializes address_, state_ and sema_
 *
 * @param full_address_string The full address in the form of IP:PORT, for
 * example 127.0.0.1:5000
 */
CVSocketClient::CVSocketClient(std::string &full_address_string)
    : address_(full_address_string), state_(kClientIdle), sema_(1) {}

/**
 * @brief Destructor, closes the possibly open connection
 */
CVSocketClient::~CVSocketClient() {
  if (connection_.impl()->initialized()) {
    connection_.close();
  }
}

/**
 * @brief Take the size information of the image, send it to the server and
 * change state to sending
 */
void CVSocketClient::StartSending(cv::Mat &image) {
  connection_.connect(address_);
  connection_.sendBytes(&image.cols, sizeof(image.cols));
  connection_.sendBytes(&image.rows, sizeof(image.rows));
  std::cout << "Sent image size: " << image.cols << "x" << image.rows
            << std::endl;
  int mat_type = image.type();
  connection_.sendBytes(&mat_type, sizeof(mat_type));
  std::cout << "Sent image type: ";
  switch (mat_type) {
    case CV_8UC3:
      std::cout << "CV_8UC3" << std::endl;
      break;

    case CV_16UC3:
      std::cout << "CV_16UC3" << std::endl;
      break;

    default:
      std::cout << "UNCOMMON TYPE" << std::endl;
      break;
  }
  buffer_size_ = image.total() * image.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  std::cout << "Starting to send..." << std::endl;
  state_ = kClientSending;
  StateChanged.notify(this, state_);
  sema_.set();
}

/**
 * @brief Receive the size information of the image and initialize a cv::Mat
 * with them according to the cv_mat_type
 *
 * @param cv_mat_type The type of the cv::Mat to receive. See OpenCV
 * documentation for more information on that.
 */
void CVSocketClient::StartReceiving() {
  connection_.connect(address_);
  connection_.receiveBytes(&image_cols_, sizeof(image_cols_));
  connection_.receiveBytes(&image_rows_, sizeof(image_rows_));
  std::cout << "Received image size: " << image_cols_ << "x" << image_rows_
            << std::endl;
  int mat_type;
  connection_.receiveBytes(&mat_type, sizeof(mat_type));
  image_ = cv::Mat::zeros(cv::Size2i(image_cols_, image_rows_), mat_type);
  buffer_size_ = image_.total() * image_.elemSize();
  std::cout << "Calculated buffer size: " << buffer_size_ << std::endl;
  sock_data_ = new uchar[buffer_size_];
  std::cout << "Starting to receive..." << std::endl;
  state_ = kClientReceiving;
  StateChanged.notify(this, state_);
  sema_.set();
}

/**
 * @brief Stop the sending/receiving, close the connection and go to idle state
 */
void CVSocketClient::Stop() {
  mutex_.lock();
  std::cout << "Stopping..." << std::endl;
  connection_.close();
  state_ = kClientIdle;
  StateChanged.notify(this, state_);
  mutex_.unlock();
}

/**
 * @brief Exit the run loop
 */
void CVSocketClient::Exit() {
  std::cout << "Exiting..." << std::endl;
  running_ = false;
  if (state_ == kClientIdle) {
    sema_.set();
  }
}

/**
 * @brief Set a new image to be sent, if the send mutex is unlocked
 *
 * @param image The image to be sent
 */
void CVSocketClient::SendNewImage(cv::Mat &image) {
  if (mutex_.tryLock()) {
    // Copy the image to private image_ in order to be sure the private image_
    // isn't indirectly modified elsewhere without mutex_
    image.copyTo(image_);
    new_image_ = true;
    mutex_.unlock();
  }
}

/**
 * @brief Send the image set with SendNewImage() function
 */
void CVSocketClient::SendMat() {
  if (new_image_) {
    mutex_.lock();
    if (!CheckConnection()) {
      connection_.close();
      state_ = kClientIdle;
      StateChanged.notify(this, state_);
      return;
    }
    connection_.sendBytes(image_.data, buffer_size_);
    new_image_ = false;
    mutex_.unlock();
  }
}

/**
 * @brief Check whether there's errors in the connection
 * @details This doesn't actually work for some reason... Even if the connection
 * is closed from the other side, poll() doesn't seem to detect that reliably
 */
bool CVSocketClient::CheckConnection() {
  if (connection_.poll(Poco::Timespan(0, 100),
                       Poco::Net::Socket::SELECT_ERROR)) {
    std::cout << "Disconnected from server" << std::endl;
    return false;
  }
  return true;
}

/**
 * @brief Receive an image from the server, assign it to cv::Mat and send it
 * onwards through an event
 */
void CVSocketClient::ReceiveMat() {
  if (!mutex_.tryLock()) {
    std::cout << "Could not lock mutex for receiving data!" << std::endl;
    return;
  }

  received_bytes_ = 0;
  for (std::size_t i = 0; i < buffer_size_; i += received_bytes_) {
    received_bytes_ =
        connection_.receiveBytes(sock_data_ + i, buffer_size_ - i);
    if (received_bytes_ == 0) {
      std::cout
          << "0 bytes received, interpreting that as disconnected from server"
          << std::endl;
      connection_.close();
      state_ = kClientIdle;
      StateChanged.notify(this, state_);
      return;
    }
  }

  mutex_.unlock();

  // Assign sock_data_ to cv::Mat of the
  image_ =
      cv::Mat(cv::Size2i(image_cols_, image_rows_), image_.type(), sock_data_);

  // WARNING: This notification doesn't check if the previous notification is
  // done. I had trouble initializing Poco::ActiveResult as private variable
  // and thus, there is no waiting for previous notification to end at the
  // moment.
  ReceivedImage.notifyAsync(this, image_);
}

/**
 * @brief Act according to the state of this class implementation
 */
void CVSocketClient::StateMachine() {
  switch (state_) {
    case kClientIdle:  // Not used if start called before run
      std::cout << "Waiting for start function call..." << std::endl;
      sema_.wait();
      break;

    case kClientSending:
      SendMat();
      break;

    case kClientReceiving:
      ReceiveMat();
      break;
  }
}

/**
 * @brief Poco::Runnable implementation. Runs the StateMachine() in a loop and
 * stops when Exit() is called.
 */
void CVSocketClient::run() {
  while (running_) {
    StateMachine();
  }
}