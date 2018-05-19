
cmake_minimum_required(VERSION 3.0)
project(pixlib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")

#add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

include_directories( ext/glm )

include_directories(include)

add_library(pixlib STATIC
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
 src/pattern_renderer.cpp
 src/led_renderer.cpp
 src/timer.cpp
 src/fade_candy.cpp
 src/pattern.cpp
 src/app.cpp
 src/data_model.cpp
 )

 target_link_libraries(pixlib sqlite3)


set(PIXLIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm PARENT_SCOPE)

