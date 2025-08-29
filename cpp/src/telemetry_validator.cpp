#include "telemetry_validator.h"

namespace iot::backend::proto {

bool IsValid(const Telemetry &telemetry)
{
    return (telemetry.payload().has_battery_level() ||
            telemetry.payload().has_speed_kmh() ||
            telemetry.payload().has_location() ||
            telemetry.payload().has_voltage() ||
            telemetry.payload().has_gsm_signal_level() ||
            telemetry.payload().has_charging() ||
            telemetry.payload().has_locked() ||
            !telemetry.payload().sensors().empty());
}

}  // namespace iot::backend::proto
