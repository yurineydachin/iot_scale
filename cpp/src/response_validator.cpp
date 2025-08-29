#include "response_validator.h"

namespace iot::backend::proto {

bool IsValid(const Response &response)
{
    // The chain_id field is mandatory.
    if (response.chain_id().empty())
        return false;

    // The result should be explicitly set.
    if (response.result() == RESULT_UNSPECIFIED)
        return false;

    if (response.result() == RESULT_FAILED && !response.has_error_description())
        return false;

    return true;
}

}  // namespace iot::backend::proto
