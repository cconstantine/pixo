
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(pixo-creator pixo-creator.cpp)
target_link_libraries(pixo-creator pixlib)

add_executable(pixo-patterns pixo-patterns.cpp)
target_link_libraries(pixo-patterns pixlib)

add_executable(pixo pixo.cpp )
target_link_libraries(pixo nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread pixlib realsense2)
