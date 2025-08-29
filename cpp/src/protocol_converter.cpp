#include "protocol_converter.h"
#include "packet.pb.h"

namespace iot::backend::proto {

ProtocolConverter::ProtocolConverter(TransportDataType type) : type_(type) {}

Packet ProtocolConverter::Deserialize(const std::string &buffer) const
{
    if (type_ == TransportDataType::BINARY) {
        return helpers::BinaryToProtoPacket<Packet>(
            helpers::FromBase64(buffer));
    }

    return helpers::JsonToProtoPacket<Packet>(buffer);
}

std::string ProtocolConverter::Serialize(const Packet &packet) const
{
    if (type_ == TransportDataType::BINARY) {
        return helpers::ToBase64(helpers::ProtoPacketToBinary(packet));
    }

    return helpers::ProtoPacketToJson(packet);
}

ProtocolConverter::TransportDataType ProtocolConverter::GetType() const
{
    return type_;
}

std::string ProtocolConverter::ToString(const proto::Packet &packet)
{
    // todo bench this, maybe critical for telemetry processing
    return helpers::ProtoPacketToJson(packet);
}

}  // namespace iot::backend::proto
