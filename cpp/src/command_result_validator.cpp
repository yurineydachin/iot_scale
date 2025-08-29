#include "command_result_validator.h"

namespace iot::backend::proto {

bool IsValid(const CommandResult &result)
{
    // The chain_id field is mandatory.
    if (result.chain_id().empty())
        return false;

    // The result should be explicitly set.
    if (result.result() == RESULT_UNSPECIFIED)
        return false;

    if (result.result() == RESULT_FAILED && !result.has_error_description())
        return false;

    if (result.result() == RESULT_FAILED &&
        (!result.error_description().has_message() ||
         !result.error_description().has_status()))
        return false;

    return true;
}

}  // namespace iot::backend::proto
