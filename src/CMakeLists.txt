
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

message(${CMAKE_BINARY_DIR})
include_directories(../ext/mongoose/)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(pixo main.cpp ../ext/mongoose/mongoose.c)
target_link_libraries(pixo nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread pixlib )
