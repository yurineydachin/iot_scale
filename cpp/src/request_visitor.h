#pragma once

#include "bike_iot_visitor.h"

#include "request.pb.h"

// Объявляем посетителя с возвращаемым значением
// #ADD_NEW_REQUEST Добавь ниже строчку по следующем образцу
// VISITOR_CASE(%проверка_наличия_пакета%, %извлечение_пакета%, VisitorName);
#define IOT_PROTO_TYPED_REQUEST_VISITOR(VisitorName, ResultType)               \
    IOT_PROTO_BEGIN_VISITOR_DECLARATION(iot::backend::proto::Request,          \
                                        VisitorName, ResultType)               \
    IOT_PROTO_END_VISITOR_DECLARATION()

// Объявляем посетителя без возвращаемого значения
#define IOT_PROTO_REQUEST_VISITOR(VisitorName)                                 \
    IOT_PROTO_TYPED_REQUEST_VISITOR(VisitorName, void)
