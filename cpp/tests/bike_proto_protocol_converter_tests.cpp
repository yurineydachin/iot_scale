#include <library/cpp/testing/gtest/gtest.h>

#include "iot_scale/cpp/src/protocol_converter.h"

class ProtocolConverterPiplineFixture
        : public ::testing::TestWithParam<std::string> {};

TEST_P(ProtocolConverterPiplineFixture, ProtocolConverterPipline) {
    const std::string json_payload = GetParam();

    using DataType = iot::backend::proto::ProtocolConverter::TransportDataType;

    iot::backend::proto::ProtocolConverter json_converter(DataType::JSON);
    iot::backend::proto::ProtocolConverter binary_converter(DataType::BINARY);

    /// json -> object -> binary -> object -> json
    const auto packet1 = json_converter.Deserialize(json_payload);
    const std::string binary = binary_converter.Serialize(packet1);
    const auto packet2 = binary_converter.Deserialize(binary);
    const std::string reconverted_json = json_converter.Serialize(packet2);
    EXPECT_EQ(json_payload, reconverted_json);
}

INSTANTIATE_TEST_SUITE_P(
        Convert, ProtocolConverterPiplineFixture,
        testing::Values(
        R"({"version":65536,"timestamp":"1677599130","validUntil":"1677599135","command":{"chainId":"de2466e4066b494c86438e4197cc70be","payload":{"configure":{}}}})",
        R"({"version":65536,"timestamp":"1677844914","commandResult":{"chainId":"856ccfc0-8c02-4f6b-a6f6-376b4871f246","result":"RESULT_SUCCESS","payload":{"configure":{}}}})",
        R"({"version":65536,"timestamp":"1677844914","commandResult":{"chainId":"856ccfc0-8c02-4f6b-a6f6-376b4871f246","result":"RESULT_FAILED"}})",
        R"({"version":16777216,"timestamp":"1707425629","telemetry":{"payload":{"voltage":373,"sensors":{"imei":"869492042841493"}}}})",
        "{\"version\":16777216,\"timestamp\":\"1707427135\",\"telemetry\":{\"payload\":{\"voltage\":413,\"sensors\":{\"imei\":\"869492042841493\"}}}}"));
