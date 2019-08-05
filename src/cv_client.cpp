#include "cv_client.hpp"

CVSocketClient::CVSocketClient(std::string full_address_string)
    : address_(full_address_string), connection_(address_) {}

CVSocketClient::~CVSocketClient(){
    connection_.close();
}