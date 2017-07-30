
cmake_minimum_required(VERSION 3.0)
project(pixo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui")
  message(FATAL_ERROR "The NanoGUI dependency is missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

###################### NANOGUI ###############################
# Disable building extras we won't need (pure C++ project)
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

# Disable warnings because assimp and nanogui have too many
add_definitions(-w)

add_subdirectory(ext/nanogui)
include_directories(ext/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

###################### GLM     ###############################
include_directories( ext/glm/ )

###################### librealsense ##########################
# add_subdirectory(ext/librealsense)
# include_directories( ext/librealsense/include )

###################### GLEW     ###############################
# glew
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(ext/glew)
include_directories(ext/glew/include lib/glew/src)

include_directories(
	pixelpusher/
	src/
	)

###################### PIXLIB   ###############################
add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})

add_executable(pixo src/main.cpp )
target_link_libraries(pixo  nanogui ${NANOGUI_EXTRA_LIBS} glfw libglew_static pthread pixlib ) #realsense


