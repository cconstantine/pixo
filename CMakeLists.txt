
cmake_minimum_required(VERSION 3.0)
project(pixo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

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


###################### PIXLIB   ###############################
add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})


###################### PIXO     ################################
add_subdirectory(src)