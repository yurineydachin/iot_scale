#include "iot_scale/cpp/src/helpers.h"
#include "iot_scale/cpp/src/packet_validator.h"

#include <library/cpp/testing/gtest/gtest.h>

#include <regex>

TEST(BikeIotProto_Helpers, GenerateChainId) {
    std::regex reIdFormat("[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}");
    auto chainId = iot::backend::proto::helpers::GenerateChainId();
    EXPECT_TRUE(std::regex_match(chainId, reIdFormat))
        << "Chain ID "<< chainId << " does not match pattern";
    for(int i=0; i<3; ++i)
    {
        auto id = iot::backend::proto::helpers::GenerateChainId();
        EXPECT_TRUE(std::regex_match(chainId, reIdFormat))
            << "Chain ID "<< id << " does not match pattern";
        EXPECT_NE(chainId, id);
        chainId = std::move(id);
    }
}

TEST(BikeIotProto_Helpers, MakePacket) {
    using namespace std::literals::chrono_literals;
    const auto packet = iot::backend::proto::helpers::MakePacket(5s);
    EXPECT_GE(iot::backend::proto::helpers::GetCurrentUtcTimestamp(), packet.timestamp());
    EXPECT_EQ(packet.valid_until()-packet.timestamp(), ::NProtoBuf::uint64(5));
}

class ConvertProtoObjectIntoStrFixture : public testing::TestWithParam<iot::backend::proto::Packet> {
public:
    static iot::backend::proto::Packet MakeValidCommand() {
        auto packet = iot::backend::proto::helpers::MakeCommand(
            std::chrono::seconds(5), iot::backend::proto::helpers::GenerateChainId());
        packet.mutable_command()->mutable_payload()->mutable_configure();
        return packet;
    }

    static iot::backend::proto::Packet MakeValidCommandResult(
            iot::backend::proto::ResultCode resultCode,
            iot::backend::proto::ErrorCode errorCode = iot::backend::proto::ErrorCode::STATUS_OK) {
        auto packet = iot::backend::proto::helpers::MakePacket();
        auto* command_result = packet.mutable_command_result();
        command_result->Setchain_id(iot::backend::proto::helpers::GenerateChainId());
        command_result->set_result(resultCode);
        if (resultCode != iot::backend::proto::ResultCode::RESULT_SUCCESS) {
            auto* err_description = command_result->mutable_error_description();
            err_description->set_status(errorCode);
            err_description->set_message("Some error message");
        }
        return packet;
    }

    static iot::backend::proto::Packet MakeInvalidCommandResult() {
        auto packet = iot::backend::proto::helpers::MakePacket();
        auto* command_result = packet.mutable_command_result();
        command_result->Setchain_id(iot::backend::proto::helpers::GenerateChainId());
        command_result->set_result(iot::backend::proto::ResultCode::RESULT_UNSPECIFIED);
        return packet;
    }

protected:
    static void Validate(const iot::backend::proto::Packet& packet, const iot::backend::proto::Packet& reconverted_packet) {
        EXPECT_EQ(packet.timestamp(), reconverted_packet.timestamp());
        EXPECT_EQ(packet.version(), reconverted_packet.version());

        if (packet.has_command()) {
            EXPECT_EQ(packet.command().chain_id(), reconverted_packet.command().chain_id());
        }

        if (packet.has_command_result()) {
            EXPECT_EQ(packet.command_result().result(), reconverted_packet.command_result().result());
            EXPECT_EQ(packet.command_result().has_error_description(), reconverted_packet.command_result().has_error_description());

            if (packet.command_result().has_error_description()) {
                EXPECT_EQ(packet.command_result().error_description().status(), reconverted_packet.command_result().error_description().status());
                EXPECT_EQ(packet.command_result().error_description().message(), reconverted_packet.command_result().error_description().message());
            }
        }
    }
};

TEST_P(ConvertProtoObjectIntoStrFixture, CompareJsonReconverted) {
    using namespace iot::backend::proto;

    Packet packet = GetParam();

    const std::string json_packet = helpers::ProtoPacketToJson(packet);
    const Packet reconverted_packet = helpers::JsonToProtoPacket<Packet>(json_packet);

    Validate(packet, reconverted_packet);
}

TEST_P(ConvertProtoObjectIntoStrFixture, CompareBinaryReconverted) {
    using namespace iot::backend::proto;

    Packet packet = GetParam();

    const std::string binary_packet = helpers::ProtoPacketToBinary(packet);
    const Packet reconverted_packet = helpers::BinaryToProtoPacket<Packet>(binary_packet);

    Validate(packet, reconverted_packet);
}

INSTANTIATE_TEST_SUITE_P(
        Convert, ConvertProtoObjectIntoStrFixture,
        testing::Values(ConvertProtoObjectIntoStrFixture::MakeValidCommand(),
                        ConvertProtoObjectIntoStrFixture::MakeInvalidCommandResult(),
                        ConvertProtoObjectIntoStrFixture::MakeValidCommandResult(iot::backend::proto::ResultCode::RESULT_SUCCESS),
                        ConvertProtoObjectIntoStrFixture::MakeValidCommandResult(iot::backend::proto::ResultCode::RESULT_FAILED, iot::backend::proto::ErrorCode::STATUS_OTHER)));
