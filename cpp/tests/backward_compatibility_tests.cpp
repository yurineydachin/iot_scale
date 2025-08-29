#include <library/cpp/testing/gtest/gtest.h>

#include "iot_scale/cpp/src/helpers.h"

class BackwardCompatibilityTestFixture
        : public ::testing::Test {};

TEST_F(BackwardCompatibilityTestFixture, BackwardCompatibilityTest) {
    using namespace iot::backend::proto;

    Packet packet;
    packet.Setversion(65537);
    packet.Settimestamp(1677599130);
    // optional field
    packet.Setvalid_until(1677599135);

    const auto packet_binary = helpers::ProtoPacketToBinary(packet);
    const auto packet_test = helpers::BinaryToProtoPacket<PacketTest>(packet_binary);
    ASSERT_EQ(packet.version(), packet_test.version());
    ASSERT_EQ(packet.timestamp(), packet_test.timestamp());
    const auto packet_json = helpers::ProtoPacketToJson(packet);
    EXPECT_THROW(
        helpers::JsonToProtoPacket<PacketTest>(packet_json),
        std::exception);

    const auto packet_test_binary = helpers::ProtoPacketToBinary(packet_test);
    const auto packet_from_test_binary = helpers::BinaryToProtoPacket<Packet>(packet_test_binary);
    ASSERT_EQ(packet_test.version(), packet_from_test_binary.version());
    ASSERT_EQ(packet_test.timestamp(), packet_from_test_binary.timestamp());
    const auto packet_test_json = helpers::ProtoPacketToJson(packet_test);
    const auto packet_from_test_json = helpers::JsonToProtoPacket<Packet>(packet_test_json);
    ASSERT_EQ(packet_test.version(), packet_from_test_json.version());
    ASSERT_EQ(packet_test.timestamp(), packet_from_test_json.timestamp());
}
