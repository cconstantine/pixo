
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(pixo-creator pixo-creator.cpp storage.cpp )
target_link_libraries(pixo-creator pixlib sqlite3)

add_executable(pixo-patterns pixo-patterns.cpp storage.cpp )
target_link_libraries(pixo-patterns pixlib sqlite3)

add_executable(pixo pixo.cpp storage.cpp )
target_link_libraries(pixo sqlite3 nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread pixlib)
