cmake_minimum_required(VERSION 3.0)
project(pixo)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

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

###################### GLEW     ###############################
set(glew-cmake_BUILD_MULTI_CONTEXT       OFF CACHE BOOL " " FORCE)
set(glew-cmake_BUILD_SHARED              OFF CACHE BOOL " " FORCE)

add_subdirectory(ext/glew EXCLUDE_FROM_ALL)
include_directories(ext/glew/include lib/glew/src)

###################### PIXLIB   ###############################
include_directories(src)
add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})

###################### PIXPQ    ###############################
add_subdirectory("pixpq/")
include_directories("${PIXPQ_INCLUDE}")

###################### PIXO     ################################
add_subdirectory(src)

