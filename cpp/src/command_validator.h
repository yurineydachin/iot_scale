#pragma once

#include "command.pb.h"

namespace iot::backend::proto {

bool IsValid(const Command &command);
bool IsValidUntilMandatory(const Command &command);

}  // namespace iot::backend::proto
