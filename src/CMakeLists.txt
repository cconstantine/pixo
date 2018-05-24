
cmake_minimum_required(VERSION 3.0)
project(pixo_app)

include_directories(../ext/mongoose/)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)


# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

include_directories(include)

add_executable(pixo-creator pixo-creator.cpp)
target_link_libraries(pixo-creator pixlib)

add_executable(pixo-patterns pixo-patterns.cpp)
target_link_libraries(pixo-patterns pixlib)

add_executable(pixo pixo.cpp )
target_link_libraries(pixo pthread pixlib ${GTK3_LIBRARIES} epoxy)
