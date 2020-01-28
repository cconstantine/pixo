
cmake_minimum_required(VERSION 3.0)
project(pixsense)

find_package( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS})

add_definitions(-fPIC -std=c++14 -std=gnu++14)

###################### GLM     ###############################
include_directories( ../ext/glm/ )
include_directories(include)

add_library(pixsense STATIC
 src/face_finder.cpp
 ../pixlib/src/timer.cpp
 )

target_link_libraries(pixsense dlib::dlib realsense2 ${OpenCV_LIBS} )

file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


set(PIXSENSE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

