cmake_minimum_required(VERSION 3.16.3)
project(frameworkd)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

