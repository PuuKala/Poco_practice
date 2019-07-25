cmake_minimum_required(VERSION 3.5)
project(Poco_practice)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message("Conan libs: ${CONAN_LIBS}")

add_executable(poco_text_server_test src/text_server.cpp)
target_link_libraries(poco_text_server_test ${CONAN_LIBS})

add_executable(poco_text_client_test src/text_client.cpp)
target_link_libraries(poco_text_client_test ${CONAN_LIBS})
