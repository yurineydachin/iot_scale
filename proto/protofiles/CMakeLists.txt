cmake_minimum_required(VERSION 3.16)

enable_language(CXX)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(
    PROTO_SRCS 
    PROTO_HDRS 
        command_payload.proto
        command.proto
        command_result_payload.proto
        command_result.proto
        error.proto
        notification_payload.proto
        notification.proto
        packet.proto
        request_payload.proto
        request.proto
        response_payload.proto
        response.proto
        telemetry_payload.proto
        telemetry.proto
)

add_library(scooter_proto_backend_lib STATIC)

target_sources(
    scooter_proto_backend_lib 
    PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
 )

target_link_libraries(
    scooter_proto_backend_lib
    PUBLIC
        ${Protobuf_LIBRARIES}
)

target_include_directories( scooter_proto_backend_lib PUBLIC . )

