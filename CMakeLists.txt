cmake_minimum_required(VERSION 3.10)
project(ESetSpeedtest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

add_executable(code code.cpp)
