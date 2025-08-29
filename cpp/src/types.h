#pragma once

#include <string>
#include <vector>

namespace iot::backend::proto {

// iot::backend::proto::Packet serialization into json type
using TJsonPacket = std::string;

// iot::backend::proto::Packet serialization into binary type
using TBinaryPacket = std::string;

#if defined(JOTUNN)
// In some versions of the protobuf library the type is already defined
using TProtoStringType = std::string;
#endif

}  // namespace iot::backend::proto
