cmake_minimum_required(VERSION 3.15)
project(pixrpc)


find_package(PkgConfig REQUIRED)
include(FindProtobuf)
find_package(Protobuf REQUIRED)

#
pkg_search_module(GRPC REQUIRED grpc)
pkg_search_module(GRPCPP REQUIRED grpc++)

# Proto file
find_program(protoc_cpp_plugin "grpc_cpp_plugin")

# Generated sources
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.grpc.pb.h")

set(PIXRPC_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}")

set(proto "pixrpc.proto")

add_custom_command(
      OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
      COMMAND protoc
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${CMAKE_CURRENT_SOURCE_DIR}"
        --plugin=protoc-gen-grpc="${protoc_cpp_plugin}"
        "${proto}"
      DEPENDS "${proto}")


include_directories("${PIXRPC_INCLUDE}")
include_directories(${PROTOBUF_INCLUDE_DIR})

add_library(pixrpc STATIC
  ${proto_srcs}
  ${grpc_srcs}
)


target_link_libraries(pixrpc grpc++ ${PROTOBUF_LIBRARY} )
set(PIXRPC_INCLUDE ${PIXRPC_INCLUDE} PARENT_SCOPE)
set(PIXRPC_LIBRARY pixrpc grpc++  ${PROTOBUF_LIBRARY} PARENT_SCOPE)

