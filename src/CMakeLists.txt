
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

include_directories(../ext/mongoose/)

add_executable(pixo main.cpp ../ext/mongoose/mongoose.c)
target_link_libraries(pixo nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread pixlib )

add_custom_target(patterns ALL)
add_custom_command(TARGET patterns
                   POST_BUILD
                   COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/../patterns .
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   VERBATIM
                   )
