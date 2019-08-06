cmake_minimum_required(VERSION 3.5)
project(Poco_practice)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message("Conan libs: ${CONAN_LIBS}")

# NOTE: OpenCV could be fetched through conan too. OpenCV can be found in conan-community,
# instead of conan-center, so it might need some more configuration for searching from
# conan-community specifically.
find_package(OpenCV)
if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found!")
endif(NOT OpenCV_FOUND)

include_directories("include")

add_executable(poco_text_server_test src/text_server.cpp)
target_link_libraries(poco_text_server_test ${CONAN_LIBS})

add_executable(poco_text_client_test src/text_client.cpp)
target_link_libraries(poco_text_client_test ${CONAN_LIBS})

add_executable(poco_cv_client_main_test src/cv_client_main.cpp src/cv_client.cpp)
target_link_libraries(poco_cv_client_main_test ${CONAN_LIBS} ${OpenCV_LIBRARIES})


add_executable(poco_cv_server_main_test src/cv_server_main.cpp src/cv_server.cpp)
target_link_libraries(poco_cv_server_main_test ${CONAN_LIBS} ${OpenCV_LIBRARIES})