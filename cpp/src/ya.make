SUBSCRIBER(g:bike)

LIBRARY()

SRCS(
    command_validator.cpp
    helpers.cpp
    helpers_backend.cpp
    command_result_validator.cpp
    request_validator.cpp
    response_validator.cpp
    telemetry_validator.cpp
    notification_validator.cpp
    packet_validator.cpp
    protocol_converter.cpp
)



PEERDIR(
    taxi/bike/iot/protocol/proto
    contrib/restricted/boost/uuid
    library/cpp/string_utils/base64
)

END()
