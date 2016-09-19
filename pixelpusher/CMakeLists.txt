# Set the name and the supported language of the project
project(pixel_pusher)

# Set the minimum version of cmake required to build this project
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11" )

include_directories(
  .
)


add_executable(pixel_pusher_sender pixel_pusher_sender.cpp)

target_link_libraries(pixel_pusher_sender pthread)