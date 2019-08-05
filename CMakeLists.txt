cmake_minimum_required(VERSION 3.5)
project(Poco_practice)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message("Conan libs: ${CONAN_LIBS}")

find_package(OpenCV)
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif(NOT OpenCV_FOUND)

include_directories("include")

add_executable(poco_text_server_test src/text_server.cpp)
target_link_libraries(poco_text_server_test ${CONAN_LIBS})

add_executable(poco_text_client_test src/text_client.cpp)
target_link_libraries(poco_text_client_test ${CONAN_LIBS})

add_executable(poco_cv_server_test src/cv_server_receive.cpp)
target_link_libraries(poco_cv_server_test ${CONAN_LIBS} ${OpenCV_LIBRARIES})

add_executable(poco_cv_client_test src/cv_client_send.cpp)
target_link_libraries(poco_cv_client_test ${CONAN_LIBS} ${OpenCV_LIBRARIES})

add_executable(poco_cv_client_main_test src/cv_client_main.cpp src/cv_client.cpp)
target_link_libraries(poco_cv_client_main_test ${CONAN_LIBS} ${OpenCV_LIBRARIES})