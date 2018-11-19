
cmake_minimum_required(VERSION 3.0)
project(pixlib)

find_package( OpenCV REQUIRED )
INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS})

add_definitions(-std=c++14 -std=gnu++14)

###################### GLM     ###############################
include_directories( ../ext/glm/ )
include_directories(include)

add_library(pixlib SHARED
 src/shader.cpp
 src/scene.cpp
 src/texture.cpp
 src/cube.cpp
 src/dot.cpp
 src/mesh.cpp
 src/camera.cpp
 src/led_mesh.cpp
 src/led_cluster.cpp
 src/renderer.cpp
 src/led_renderer.cpp
 src/timer.cpp
 src/fade_candy.cpp
 src/pattern.cpp
 src/app.cpp
 src/data_model.cpp
 src/face_finder.cpp
 )

target_link_libraries(pixlib dlib::dlib pthread realsense2 ${OpenCV_LIBS} )


set(PIXLIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

