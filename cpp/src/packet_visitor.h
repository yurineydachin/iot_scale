#pragma once

#include "bike_iot_visitor.h"

#include "packet.pb.h"

// Набор доступных составных для объектов типа iot::backend::proto::Packet
// Пример использования смотри в packet_validator.cpp

// Объявляем посетителя с возвращаемым значением
// #ADD_NEW_PACKET Добавь ниже строчку по следующем образцу
// VISITOR_CASE(%проверка_наличия_пакета%, %извлечение_пакета%, VisitorName);
#define IOT_PROTO_TYPED_PACKET_VISITOR(VisitorName, ResultType)                \
    IOT_PROTO_BEGIN_VISITOR_DECLARATION(iot::backend::proto::Packet,           \
                                        VisitorName, ResultType)               \
    VISITOR_CASE(has_command, command, VisitorName);                           \
    VISITOR_CASE(has_command_result, command_result, VisitorName);             \
    VISITOR_CASE(has_request, request, VisitorName);                           \
    VISITOR_CASE(has_response, response, VisitorName);                         \
    VISITOR_CASE(has_telemetry, telemetry, VisitorName);                       \
    VISITOR_CASE(has_notification, notification, VisitorName);                 \
    IOT_PROTO_END_VISITOR_DECLARATION()

// Объявляем посетителя без возвращаемого значения
#define IOT_PROTO_PACKET_VISITOR(VisitorName)                                  \
    IOT_PROTO_TYPED_PACKET_VISITOR(VisitorName, void)
