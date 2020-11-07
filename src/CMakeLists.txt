
cmake_minimum_required(VERSION 3.15)
project(pixo_app)

include_directories("${PIXPQ_INCLUDE}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_executable(pixo-creator pixo-creator.cpp storage.cpp )
target_compile_features(pixo-creator PRIVATE cxx_std_17)
target_link_libraries(pixo-creator pixlib libglew_static sqlite3 )
 
add_executable(pixo-patterns pixo-patterns.cpp storage.cpp )
target_compile_features(pixo-patterns PRIVATE cxx_std_17)
target_link_libraries(pixo-patterns pixlib libglew_static sqlite3 )

add_executable(pixo pixo.cpp storage.cpp)
target_compile_features(pixo PRIVATE cxx_std_17)

target_link_libraries(pixo pixlib sqlite3 nanogui ${NANOGUI_EXTRA_LIBS} libglew_static ${PIXPQ_LIBRARY} )
