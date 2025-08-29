#pragma once

#include "helpers.h"

namespace iot::backend::proto {

/// Класс помошник для конвертации в объект пришедших сообщений
/// и для конвертации объектов для отправки:
/// json <-> object <-> binary
class ProtocolConverter {
  public:
    /*
     * JSON = std::string
     * BINARY = std::string
     */
    enum class TransportDataType { JSON, BINARY };

    explicit ProtocolConverter(TransportDataType type);

    [[nodiscard]] Packet      Deserialize(const std::string &buffer) const;
    [[nodiscard]] std::string Serialize(const Packet &packet) const;

    [[nodiscard]] TransportDataType GetType() const;

    /// Debug method to get human readable string for logging
    static std::string ToString(const Packet &packet);

  private:
    const TransportDataType type_;
};

}  // namespace iot::backend::proto
