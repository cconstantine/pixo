
cmake_minimum_required(VERSION 3.0)
project(pixlib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

###################### GLM     ###############################
include_directories( ../ext/glm/ )

###################### ASSIMP  ###############################
include_directories(../ext/assimp/include)

include_directories(include)

add_library(pixlib STATIC
 src/shader.cpp
 src/scene.cpp
 src/texture.cpp
 src/model.cpp
 src/mesh.cpp
 src/camera.cpp
 src/led_mesh.cpp
 src/led_cluster.cpp
 src/renderer.cpp
 src/fade_candy.cpp)


set(PIXLIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

