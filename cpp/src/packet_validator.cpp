#include "packet_validator.h"

// #ADD_NEW_PACKET
// Валидаторы возможных типов пакетов. Добавь в список новые по необходимости
#include "request_validator.h"
#include "response_validator.h"
#include "command_validator.h"
#include "command_result_validator.h"
#include "telemetry_validator.h"
#include "notification_validator.h"

#include "packet_visitor.h"

namespace {

// Велики появились в 2023 году, так что время отправки команды
// никогда не может быть раньше 1 января 2023.
static const uint64_t kMinimalTimestamp =
    1672574400;  // Sunday, January 1, 2023 12:00:00 PM

bool ValidatePayloadDependent(const iot::backend::proto::Packet &packet)
{
    // Check, whether lifetime field is mandatory for the packet
    IOT_PROTO_TYPED_PACKET_VISITOR(IsValidUntilMandatory, bool)
    lifetime_validator;
    if (lifetime_validator(packet) && !packet.has_valid_until()) {
        return false;
    }

    // Validate all possible composition items
    // i.e. if(packet.has_XXXX) return IsValid(packet); for all possible cases
    IOT_PROTO_TYPED_PACKET_VISITOR(IsValid, bool) validator;
    return validator(packet);
}
}  // namespace

namespace iot::backend::proto {

bool IsValid(const Packet &packet)
{
    // Protocol version can't be zero or 0xFFFFFFFF;
    const auto proto_version = packet.version();
    const auto major = (proto_version >> 16) & 0xFFFF;
    const auto minor = proto_version & 0xFFFF;
    if ((major == 0 && minor == 0) || major == 0xFFFF || minor == 0xFFFF) {
        return false;
    }

    // Timestamp can't be less than beginning of 2023.
    const auto timestamp = packet.timestamp();
    if (timestamp < kMinimalTimestamp)  // Sunday, January 1, 2023 12:00:00 PM
    {
        return false;
    }

    if (packet.has_valid_until() && packet.valid_until() < packet.timestamp()) {
        return false;
    }

    try {
        return ValidatePayloadDependent(packet);
    } catch (...) {
        // Unsupported packet type - we can't process it.
        return false;
    }
}

}  // namespace iot::backend::proto
