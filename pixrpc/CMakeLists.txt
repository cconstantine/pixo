cmake_minimum_required(VERSION 3.15)
project(pixrpc)


###################### GRPC ##################################
add_subdirectory(./ext/grpc ${CMAKE_CURRENT_BINARY_DIR}/grpc EXCLUDE_FROM_ALL)

set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)


# Generated sources
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pixrpc.grpc.pb.h")

set(PIXRPC_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}")

set(proto "pixrpc.proto")

add_custom_command(
      OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
      COMMAND "${_PROTOBUF_PROTOC}"
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${CMAKE_CURRENT_SOURCE_DIR}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto}"
      DEPENDS "${_PROTOBUF_PROTOC}" "${_GRPC_CPP_PLUGIN_EXECUTABLE}" "${proto}")


include_directories("${PIXRPC_INCLUDE}")
include_directories(${PROTOBUF_INCLUDE_DIR})

add_library(pixrpc STATIC
  ${proto_srcs}
  ${grpc_srcs}
)

target_link_libraries(pixrpc grpc++ ${PROTOBUF_LIBRARY} )
set(PIXRPC_INCLUDE ${PIXRPC_INCLUDE} PARENT_SCOPE)
set(PIXRPC_LIBRARY pixrpc grpc++  ${PROTOBUF_LIBRARY} PARENT_SCOPE)

