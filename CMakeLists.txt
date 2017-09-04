
cmake_minimum_required(VERSION 3.0)
project(pixo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui")
  message(FATAL_ERROR "The NanoGUI dependency is missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

###################### CROW    ###############################
find_package( Boost 1.52 COMPONENTS system thread REQUIRED )
include_directories(ext/crow/include)

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

###################### GLEW     ###############################
# glew
set(glew-cmake_BUILD_MULTI_CONTEXT       OFF CACHE BOOL " " FORCE)
set(glew-cmake_BUILD_SHARED              OFF CACHE BOOL " " FORCE)

add_subdirectory(ext/glew)
include_directories(ext/glew/include lib/glew/src)

###################### PIXLIB   ###############################
include_directories(
  src/
  )

add_subdirectory(pixlib)
include_directories(${PIXLIB_INCLUDE})


###################### PIXO     ################################
add_executable(pixo src/main.cpp )
target_link_libraries(pixo nanogui ${NANOGUI_EXTRA_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} glfw libglew_static pthread pixlib )

add_custom_target(patterns ALL)
add_custom_command(TARGET patterns
                   POST_BUILD
                   COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/patterns .
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   VERBATIM
                   )

add_custom_target(shaders ALL)
add_custom_command(TARGET shaders
                   POST_BUILD
                   COMMAND cp -a ${CMAKE_CURRENT_SOURCE_DIR}/shaders .
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   VERBATIM
                   )