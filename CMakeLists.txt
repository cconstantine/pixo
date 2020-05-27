
cmake_minimum_required(VERSION 3.0)
project(pixo)

set(CMAKE_C_COMPILER /usr/bin/gcc-8 CACHE BOOL " " FORCE)
set(CMAKE_CXX_COMPILER /usr/bin/g++-8 CACHE BOOL " " FORCE)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if (NOT SKIP_TRACKER)
###################### DLIB     ###############################
include(./ext/dlib/dlib/cmake)
endif(NOT SKIP_TRACKER)

###################### NANOGUI ###############################
# Disable building extras we won't need (pure C++ project)
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

# Disable warnings because assimp and nanogui have too many
add_definitions(-w)

add_subdirectory(ext/nanogui EXCLUDE_FROM_ALL)
include_directories(ext/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})


###################### GLM     ###############################
include_directories( ext/glm/ )

if (NOT SKIP_TRACKER)
###################### librealsense ###########################
set(BUILD_GRAPHICAL_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
set(BUILD_WITH_CUDA ON CACHE BOOL " " FORCE)
set(BUILD_WITH_TM2  OFF CACHE BOOL " " FORCE)
add_subdirectory(ext/librealsense EXCLUDE_FROM_ALL)
include_directories(ext/librealsense/include)
endif(NOT SKIP_TRACKER)


###################### GLEW     ###############################
# glew
set(glew-cmake_BUILD_MULTI_CONTEXT       OFF CACHE BOOL " " FORCE)
set(glew-cmake_BUILD_SHARED              OFF CACHE BOOL " " FORCE)

add_subdirectory(ext/glew EXCLUDE_FROM_ALL)
include_directories(ext/glew/include lib/glew/src)


###################### PIXLIB   ###############################
include_directories(src)
add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})

if (NOT SKIP_TRACKER)
####################### PIXSENSE ##############################
add_subdirectory(pixsense)
include_directories(${PIXSENSE_INCLUDE})
endif(NOT SKIP_TRACKER)

###################### PIXLIB   ###############################
add_subdirectory(pixrpc)

###################### PIXO     ################################
add_subdirectory(src)

