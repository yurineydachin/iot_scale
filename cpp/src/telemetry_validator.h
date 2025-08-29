#pragma once

#include "telemetry.pb.h"

namespace iot::backend::proto {

bool IsValid(const Telemetry &telemetry);
// Прямо сейчас для телеметрии время жизни не является обязетнльным
// И в блиайшем будущем не будет, поэтому constexpr и inline
constexpr bool IsValidUntilMandatory(const Telemetry & /*telemetry*/)
{
    return false;
};
}  // namespace iot::backend::proto
