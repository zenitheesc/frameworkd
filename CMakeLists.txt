cmake_minimum_required(VERSION 3.16.3)
project(daemons-framework)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(daemonTest tests/mainTest.cpp)
target_link_libraries(daemonTest ${CONAN_LIBS})