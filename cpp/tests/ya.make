GTEST()

SUBSCRIBER(g:bike)

SRCS(
    bike_proto_tests.cpp
    bike_proto_helpers_tests.cpp
    bike_proto_protocol_converter_tests.cpp
    backward_compatibility_tests.cpp
)

PEERDIR(
    taxi/bike/iot/protocol/proto
    taxi/bike/iot/protocol/src
)

END()
