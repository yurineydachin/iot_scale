#include "command_validator.h"

#include "command_visitor.h"

namespace {

template <class CommandType>
bool IsValidCommand(const CommandType & /*command*/)
{
    return true;
}

// #ADD_NEW_COMMAND Добавь сюда специализацию валидатора для нужного типа
/*
template<>
bool IsValidCommand(const AnyParticularType& command) // i.e.
::bike::iot::proto::CmdIdentify
{
    return true;
}
*/

}  // namespace

namespace iot::backend::proto {

bool IsValidUntilMandatory(const Command &command)
{
    // Перечисляем команды, для которых в пакете обязательно наличие времени
    // жизни
    return command.has_payload() &&
           (command.payload().has_configure() ||
            command.payload().has_set_state() ||
            command.payload().has_set_params() ||
            command.payload().has_get_params() || command.payload().has_ping());
}

bool IsValid(const Command &command)
{
    if (command.chain_id().empty())
        return false;

    IOT_PROTO_TYPED_COMMAND_VISITOR(::IsValidCommand, bool) validator;
    return validator(command.payload());
}

}  // namespace iot::backend::proto
