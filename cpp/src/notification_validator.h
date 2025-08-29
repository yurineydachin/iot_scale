#pragma once

#include "notification.pb.h"

namespace iot::backend::proto {

bool IsValid(const Notification &notification);
// Прямо сейчас для ответов время жизни не является обязетнльным
// И в блиайшем будущем не будет, поэтому constexpr и inline
constexpr bool IsValidUntilMandatory(const Notification & /*notification*/)
{
    return false;
};

}  // namespace iot::backend::proto
