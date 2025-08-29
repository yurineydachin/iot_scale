#include "helpers.h"

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <library/cpp/string_utils/base64/base64.h>

namespace iot::backend::proto::helpers {

std::string GenerateChainId()
{
    thread_local boost::uuids::random_generator uuid_gen;
    const boost::uuids::uuid                    id = uuid_gen();
    return to_string(id);
}

std::string ToBase64(std::string_view data)
{
    return Base64Encode(TStringBuf{data});
}

std::string FromBase64(std::string_view base64_data)
{
    return Base64DecodeUneven(TStringBuf{base64_data});
}

Packet MakeCommand(std::optional<std::chrono::seconds> valid_period_s,
                   std::string_view                    chain_id)
{
    Packet          packet = MakePacket(valid_period_s);
    proto::Command *cmd = packet.mutable_command();
    cmd->set_chain_id(chain_id.data());
    return packet;
}

}  // namespace iot::backend::proto::helpers
