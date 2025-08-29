#pragma once

#include "packet.pb.h"

namespace iot::backend::proto {

bool IsValid(const Packet &packet);

}
