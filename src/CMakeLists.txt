
cmake_minimum_required(VERSION 3.15)
project(pixo_app)

include_directories("${PIXPQ_INCLUDE}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(pixo-creator pixo-creator.cpp )
target_compile_features(pixo-creator PRIVATE cxx_std_17)
target_link_libraries(pixo-creator ${PIXPQ_LIBRARY} )
 
add_executable(pixo-patterns pixo-patterns.cpp )
target_compile_features(pixo-patterns PRIVATE cxx_std_17)
target_link_libraries(pixo-patterns ${PIXPQ_LIBRARY})

add_executable(pixo pixo.cpp )
target_compile_features(pixo PRIVATE cxx_std_17)

target_link_libraries(pixo pixlib nanogui ${NANOGUI_EXTRA_LIBS} libglew_static ${PIXPQ_LIBRARY} )
