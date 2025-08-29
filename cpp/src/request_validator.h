#pragma once

#include "request.pb.h"

namespace iot::backend::proto {

bool IsValid(const Request &request);
bool IsValidUntilMandatory(const Request &request);

}  // namespace iot::backend::proto
