
cmake_minimum_required(VERSION 3.0)
project(pixo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

###################### GLFW     ################################
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/glfw EXCLUDE_FROM_ALL)


###################### NANOGUI ###############################
# Disable building extras we won't need (pure C++ project)
#set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
#set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
#set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

# Disable warnings because assimp and nanogui have too many
#add_definitions(-w)

#add_subdirectory(ext/nanogui EXCLUDE_FROM_ALL)
#include_directories(ext/nanogui/include)
#include_directories(${NANOGUI_EXTRA_INCS})
#add_definitions(${NANOGUI_EXTRA_DEFS})


###################### GLM     ###############################
include_directories( ext/glm/ )

###################### GLEW     ###############################
# glew
set(glew-cmake_BUILD_MULTI_CONTEXT       OFF CACHE BOOL " " FORCE)
set(glew-cmake_BUILD_SHARED              OFF CACHE BOOL " " FORCE)

add_subdirectory(ext/glew EXCLUDE_FROM_ALL)
include_directories(ext/glew/include lib/glew/src)

###################### PIXLIB   ###############################
add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})

set(PIXLIB_INCLUDE
  ${PIXO_INCLUDE}
  ${NANOGUI_EXTRA_INCS} 
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/glew/include
  ${PIXLIB_INCLUDE}
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/glm/
 PARENT_SCOPE) 


###################### PIXO     ################################
add_subdirectory(src)