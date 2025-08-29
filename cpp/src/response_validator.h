#pragma once

#include "response.pb.h"

namespace iot::backend::proto {

bool IsValid(const Response &response);
// Прямо сейчас для ответов время жизни не является обязетнльным
// И в блиайшем будущем не будет, поэтому constexpr и inline
constexpr bool IsValidUntilMandatory(const Response & /*response*/)
{
    return false;
};

}  // namespace iot::backend::proto
