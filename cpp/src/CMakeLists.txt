cmake_minimum_required(VERSION 3.16)

enable_language(CXX)

add_library(proto_helpers_lib STATIC)

add_compile_definitions(JOTUNN)

target_sources(
    proto_helpers_lib
    PRIVATE
        "command_validator.cpp"
        "helpers.cpp"
        "command_result_validator.cpp"
        "request_validator.cpp"
        "response_validator.cpp"
        "telemetry_validator.cpp"
        "notification_validator.cpp"
        "packet_validator.cpp"
        "protocol_converter.cpp"
 )

target_link_libraries(
    proto_helpers_lib
    PUBLIC
        protobuf
        jsoncpp
        scooter_proto_backend_lib
    INTERFACE
        ${JTN_IO_LIBRARIES}
)

