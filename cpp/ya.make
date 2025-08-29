LIBRARY()

SUBSCRIBER(g:bike)

PEERDIR(
    taxi/bike/iot/protocol/proto
    taxi/bike/iot/protocol/src
)

END()

RECURSE(
    example
    tests
)
