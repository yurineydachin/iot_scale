#pragma once

#include "packet.pb.h"
#include <google/protobuf/util/json_util.h>

#include "types.h"

#include <optional>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace iot::backend::proto::helpers {

/// Generate unique chain identity
/// Right now it is implemented as UUID generation
std::string GenerateChainId();

/// Returns current timestamp in protocol specific format (UTC, seconds since
/// epoch)
uint64_t GetCurrentUtcTimestamp();
uint64_t GetCurrentUtcTimestampMs();

/// Create protocol packet with default member values (times, versions and so)
iot::backend::proto::Packet MakePacket(
    std::optional<std::chrono::seconds> valid_period_s = std::nullopt);

/// Make packet with Command structure inside. Fill chain filed with proper
/// value
iot::backend::proto::Packet MakeCommand(
    std::optional<std::chrono::seconds> valid_period_s,
    std::string_view                    chain_id);

/// Make packet with success status inside
iot::backend::proto::Packet MakeCommandResultSuccess(
    std::string_view chain_id,
    std::optional<std::string_view> prev_chain_id,
    int32_t cmd_delivery_time_s,
    int32_t cmd_execution_time_ms);

/// Make packet with error status inside
iot::backend::proto::Packet MakeCommandResultError(
    std::string_view                chain_id,
    iot::backend::proto::ResultCode resultCode,
    iot::backend::proto::ErrorCode  errorCode,
    std::string_view                message,
    std::optional<std::string_view> prev_chain_id,
    int32_t                         cmd_delivery_time_s,
    int32_t                         cmd_execution_time_ms);

/// Convert C++ ProtoPacket into std::string json
template <class PacketT>
TJsonPacket ProtoPacketToJson(const PacketT &packet)
{
    TProtoStringType json_message;

    const auto status =
        google::protobuf::util::MessageToJsonString(packet, &json_message);

    if (!status.ok()) {
        throw std::invalid_argument(
            "google::protobuf::util::MessageToJsonString failed: " +
            status.ToString());
    }

    return json_message;
}

/// Convert C++ ProtoPacket into std::string (size() safe)
template <class PacketT>
TBinaryPacket ProtoPacketToBinary(const PacketT &packet)
{
    const auto packet_size = packet.ByteSizeLong();

    TBinaryPacket data;
    data.resize(packet_size);

    const auto res =
        packet.SerializeToArray(data.data(), static_cast<int>(packet_size));

    if (!res) {
        throw std::invalid_argument(
            "google::protobuf::Message.SerializeToArray failed");
    }

    return data;
}

/// Convert std::string json into C++ ProtoPacket
template <class PacketT>
PacketT JsonToProtoPacket(const TJsonPacket &json_string)
{
    PacketT proto_packet;

    auto l1 = json_string.size();
    auto l2 = strlen(json_string.c_str());

    const auto status = google::protobuf::util::JsonStringToMessage(
        TProtoStringType(json_string), &proto_packet);

    if (!status.ok() || (l1 != l2)) {
        throw std::invalid_argument(
            "google::protobuf::util::JsonStringToMessage failed: '{}'" +
            status.ToString());
    }

    return proto_packet;
}

/// Convert std::string with binary data into C++ ProtoPacket
template <class PacketT>
PacketT BinaryToProtoPacket(const TBinaryPacket &binary_string)
{
    PacketT proto_packet;

    if (!proto_packet.ParseFromString(TProtoStringType(binary_string))) {
        throw std::runtime_error("Wrong packet format");
    }

    return proto_packet;
}

/// Encode a string to Base64
std::string ToBase64(std::string_view data);

/// Decode a string from Base64
std::string FromBase64(std::string_view base64_data);

}  // namespace iot::backend::proto::helpers
