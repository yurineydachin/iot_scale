#include "request_validator.h"

#include "request_visitor.h"

namespace {

template <class RequestType>
bool IsValidRequest(const RequestType & /*request*/)
{
    return true;
}

// #ADD_NEW_REQUEST Добавь сюда специализацию валидатора для нужного типа
/*
template<>
bool IsValidRequest(const AnyParticularType& request) // i.e.
::iot::backend::proto::ReqUpgrade
{
    return true;
}
*/

}  // namespace

namespace iot::backend::proto {

bool IsValidUntilMandatory(const Request & /*request*/) { return false; }

bool IsValid(const Request &request)
{
    // The chain_id field is mandatory.
    if (request.chain_id().empty())
        return false;

    // IOT_PROTO_TYPED_REQUEST_VISITOR(::IsValidRequest, bool) validator;
    // return validator(request);
    return true;
}

}  // namespace iot::backend::proto
