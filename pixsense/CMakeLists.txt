
cmake_minimum_required(VERSION 3.0)
project(pixsense)

find_package( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS})


###################### librealsense ###########################
set(BUILD_GRAPHICAL_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
set(BUILD_WITH_CUDA ON CACHE BOOL " " FORCE)
set(BUILD_WITH_TM2  OFF CACHE BOOL " " FORCE)
add_subdirectory(ext/librealsense EXCLUDE_FROM_ALL)
include_directories(ext/librealsense/include)
add_definitions(-fPIC -std=c++14 -std=gnu++14)

###################### DLIB     ###############################
include(./ext/dlib/dlib/cmake)

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

add_executable(trainer src/trainer.cpp)
target_link_libraries(trainer dlib )
