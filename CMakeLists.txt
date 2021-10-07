cmake_minimum_required(VERSION 3.16.3)
project(frameworkd)

add_definitions("-std=c++17")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(lockedStorage src/classes/locked-storage/locked-storage.cpp src/classes/locked-storage/locked-storage.hpp)
target_link_libraries(lockedStorage ${CONAN_LIBS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
