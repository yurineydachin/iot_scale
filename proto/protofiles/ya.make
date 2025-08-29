SUBSCRIBER(g:bike)

PROTO_LIBRARY()

PROTO_NAMESPACE(taxi/bike/iot/protocol/proto)
PY_NAMESPACE(.)

SRCS(
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

EXCLUDE_TAGS(GO_PROTO JAVA_PROTO)

END()

