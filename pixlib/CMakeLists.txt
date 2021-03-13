
cmake_minimum_required(VERSION 3.0)
project(pixlib)

add_definitions(-fPIC -std=c++14 -std=gnu++14)

###################### GLM     ###############################
include_directories( ../ext/glm/ )
include_directories(include)

add_library(pixlib STATIC
 src/shader.cpp
 src/scene.cpp
 src/texture.cpp
 src/drawable.cpp
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
 )

target_link_libraries(pixlib pthread )


set(PIXLIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

