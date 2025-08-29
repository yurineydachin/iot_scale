#pragma once

#include "command_result.pb.h"

namespace iot::backend::proto {

bool IsValid(const CommandResult &status);
// Прямо сейчас для результата время жизни не является обязетнльным
// И в блиайшем будущем не будет, поэтому constexpr и inline
constexpr bool IsValidUntilMandatory(const CommandResult & /*result*/)
{
    return false;
};

}  // namespace iot::backend::proto
