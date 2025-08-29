#pragma once

#include "bike_iot_visitor.h"

#include "command.pb.h"

// Объявляем посетителя с возвращаемым значением
// #ADD_NEW_COMMAND Добавь ниже строчку по следующем образцу
// VISITOR_CASE(%проверка_наличия_пакета%, %извлечение_пакета%, VisitorName);
#define IOT_PROTO_TYPED_COMMAND_VISITOR(VisitorName, ResultType)               \
    IOT_PROTO_BEGIN_VISITOR_DECLARATION(iot::backend::proto::CommandPayload,   \
                                        VisitorName, ResultType)               \
    VISITOR_CASE(has_configure, configure, VisitorName);                       \
    VISITOR_CASE(has_set_state, set_state, VisitorName);                       \
    VISITOR_CASE(has_set_params, set_params, VisitorName);                     \
    VISITOR_CASE(has_get_params, get_params, VisitorName);                     \
    VISITOR_CASE(has_ping, ping, VisitorName);                                 \
    IOT_PROTO_END_VISITOR_DECLARATION()

// Объявляем посетителя без возвращаемого значения
#define IOT_PROTO_COMMAND_VISITOR(VisitorName)                                 \
    IOT_PROTO_TYPED_COMMAND_VISITOR(VisitorName, void)
