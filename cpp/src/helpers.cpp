#include "helpers.h"

#include <chrono>

namespace iot::backend::proto::helpers {

using Packet = Packet;
using namespace iot::backend::proto;

/*
 * There are a few ways to determine a system time in seconds. All of them have
 * some drawbacks: 1) std::chrono::system_clock::now().time_since_epoch().
 * According to the standard it is not guaranteed that epoch is 01.01.1970.
 * However, starting from C++20, it should be so. Before C++20 it also was
 * typically true for POSIX systems.
 *
 * 2) std::time(). The result is also unclear. It returns std::time_t which
 * usually on POSIX systems is time in seconds since the Epoch. I've tested, and
 * it is true indeed for my Ubuntu 20.04.
 *
 * 3) gettimeofday() and then calculate tp.tv_sec + tp.tv_usec / 10000000. It
 * also works but looks too guilty.
 *
 * 4) Use chrono::utc_clock. It's a wrong approach that won't give a proper
 * result. This clock should return real amount of seconds since Epoch, but
 * currently all popular OSes use system time which does NOT take into account
 * leap seconds. (see
 * https://stackoverflow.com/questions/56618615/leap-seconds-and-stdchrono)
 * Moreover, this clock is unsupported by GCC now.
 *
 * Since we're working with C++20 it was decided to use the first approach.
 */
uint64_t GetCurrentUtcTimestamp()
{
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());
    return timestamp.count();
}

uint64_t GetCurrentUtcTimestampMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

Packet MakePacket(std::optional<std::chrono::seconds> valid_period_s)
{
    const auto current_timestamp = GetCurrentUtcTimestamp();

    Packet packet;
    packet.set_version(0x10000);
    packet.set_timestamp(current_timestamp);

    if (valid_period_s.has_value()) {
        const auto valid_until = packet.timestamp() + valid_period_s->count();
        packet.set_valid_until(valid_until);
    }

    return packet;
}

Packet MakeCommandResultPacket(
    std::string_view chain_id,
    std::optional<std::string_view> prev_chain_id,
    int32_t cmd_delivery_time_s,
    int32_t cmd_execution_time_ms)
{
    auto  packet = MakePacket();
    auto *result = packet.mutable_command_result();
    result->set_chain_id(chain_id.data());
    result->set_cmd_delivery_time(cmd_delivery_time_s);
    result->set_cmd_execution_time_ms(cmd_execution_time_ms);
    if (prev_chain_id) result->set_prev_chain_id(prev_chain_id->data());
    return packet;
}


Packet MakeCommandResultSuccess(
    std::string_view chain_id,
    std::optional<std::string_view> prev_chain_id,
    int32_t cmd_delivery_time_s,
    int32_t cmd_execution_time_ms)
{
    auto packet = MakeCommandResultPacket(chain_id, prev_chain_id, cmd_delivery_time_s, cmd_execution_time_ms);
    packet.mutable_command_result()->set_result(iot::backend::proto::RESULT_SUCCESS);
    return packet;
}

Packet MakeCommandResultError(std::string_view                chain_id,
                              iot::backend::proto::ResultCode resultCode,
                              iot::backend::proto::ErrorCode  errorCode,
                              std::string_view                message,
                              std::optional<std::string_view> prev_chain_id,
                              int32_t                         cmd_delivery_time_s,
                              int32_t                         cmd_execution_time_ms)
{
    auto packet = MakeCommandResultPacket(chain_id, prev_chain_id, cmd_delivery_time_s, cmd_execution_time_ms);
    auto *result = packet.mutable_command_result();
    result->set_result(resultCode);
    auto *description = result->mutable_error_description();
    description->set_status(errorCode);
    description->set_message(message.data());
    return packet;
}

}  // namespace iot::backend::proto::helpers
