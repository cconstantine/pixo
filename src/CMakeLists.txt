
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

add_executable(pixo main.cpp )
target_link_libraries(pixo glfw libglew_static pthread pixlib pistache)

add_custom_target(patterns ALL)
add_custom_command(TARGET patterns
                   POST_BUILD
                   COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/../patterns .
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   VERBATIM
                   )
