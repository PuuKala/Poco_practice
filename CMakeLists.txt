cmake_minimum_required(VERSION 3.5)
project(Poco_practice)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(poco_net_test src/main.cpp)
target_link_libraries(poco_net_test ${CONAN_LIBS})
