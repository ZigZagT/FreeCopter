cmake_minimum_required(VERSION 3.4)
project(FreeCopter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/include/FreeCopter.h)

add_executable(FreeCopter ${SOURCE_FILES})