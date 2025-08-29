#pragma once

#include <stdexcept>

/*
Небольшой набор макромагии для организации обхода составных ProtoBuf объектов.

При помощи препроцессора собираем по месту следующую конструкцию:
class {
public:
    ReturnType operator()(const Subject& subject)
    {
        const auto visitor = getVisitor(subject)
        return visitor();
    }
protected:
    std::function<ReturnType(void)> getVisitor(const Subject& subject)
    {
        std::function<ReturnType(void)> result;
        // Для каждого составного блочка %Item% тут вставляем конструкцию вида
        if(subject.has%Item%)
            result = [](){ return %Visitor%(subject.get%Item%) };
        result;
    }
}

Примеры описывания допустимых составных частей смотри в packet_visitor.h
Пример использования смотри в packet_validator.cpp
*/
#define VISITOR_CASE(HasMethodName, GetMethodName, VisitorName)                \
    if (subject.HasMethodName()) {                                             \
        if (result)                                                            \
            throwMoreThanOneValue();                                           \
        result = [&subject]() {                                                \
            return VisitorName(subject.GetMethodName());                       \
        };                                                                     \
    }

#define IOT_PROTO_BEGIN_VISITOR_DECLARATION(ClassName, VisitorName,            \
                                            ReturnType)                        \
    static const class {                                                       \
      public:                                                                  \
        ReturnType operator()(const ClassName &subject) const                  \
        {                                                                      \
            const auto processor = getVisitor(subject);                        \
            return processor();                                                \
        }                                                                      \
                                                                               \
      private:                                                                 \
        void throwMoreThanOneValue() const                                     \
        {                                                                      \
            throw std::runtime_error("Composition '" #ClassName                \
                                     "' has more than one available value");   \
        }                                                                      \
                                                                               \
        void throwNoValue() const                                              \
        {                                                                      \
            throw std::runtime_error("Composition '" #ClassName                \
                                     "' has no available value");              \
        }                                                                      \
                                                                               \
        std::function<ReturnType(void)> getVisitor(                            \
            const ClassName &subject) const                                    \
        {                                                                      \
            std::function<ReturnType(void)> result;

#define IOT_PROTO_END_VISITOR_DECLARATION()                                    \
    if (!result)                                                               \
        throwNoValue();                                                        \
    return result;                                                             \
    }                                                                          \
    }
