
cmake_minimum_required(VERSION 3.15)
project(pixo_app)

include_directories("${PIXRPC_INCLUDE}")

set (CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

add_definitions(-std=c++14)

add_executable(pixo-creator pixo-creator.cpp storage.cpp )
target_compile_features(pixo-creator PRIVATE cxx_std_14)
target_link_libraries(pixo-creator pixlib libglew_static sqlite3 )
 

add_executable(pixo-patterns pixo-patterns.cpp storage.cpp )
target_compile_features(pixo-patterns PRIVATE cxx_std_14)
target_link_libraries(pixo-patterns pixlib libglew_static sqlite3 )

add_executable(pixo-tracker pixo-tracker.cpp storage.cpp )
target_compile_features(pixo-tracker PRIVATE cxx_std_14)
target_link_libraries(pixo-tracker pixlib libglew_static sqlite3 )

add_executable(pixo pixo.cpp storage.cpp)
target_compile_features(pixo PRIVATE cxx_std_14)

target_link_libraries(pixo pixlib sqlite3 nanogui ${NANOGUI_EXTRA_LIBS} libglew_static ${PIXRPC_LIBRARY})

if (PIXO_BUILD_TRACKER)
add_executable(face_detection face_detection.cpp)
target_link_libraries(face_detection pixsense)

add_executable(tracking_service tracking_service.cpp)
target_link_libraries(tracking_service pixsense ${PIXRPC_LIBRARY})
endif(PIXO_BUILD_TRACKER)