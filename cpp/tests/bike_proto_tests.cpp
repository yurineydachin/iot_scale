#include "iot_scale/cpp/proto/packet.pb.h"
#include "iot_scale/cpp/src/packet_validator.h"

#include <google/protobuf/util/message_differencer.h>

#include <library/cpp/testing/gtest/gtest.h>

#include <util/generic/string.h>
#include <util/stream/str.h>

class CmdBuilder {
private:
    CmdBuilder() {}

public:
    static CmdBuilder Create() { return CmdBuilder(); }

    CmdBuilder& AddDefaultPacketHeader()
    {
        AddTimestamp(1675917321);
        AddVersion(1,1);
        AddChainId("123");
        AddValidUntil(1675917321 + 1);
        return *this;
    }

    CmdBuilder& AddVersion(unsigned major, unsigned minor)
    {
        uint32_t ver = (major << 16) | (minor & 0xFFFF);
        packet_.set_version(ver);
        return *this;
    }

    CmdBuilder& AddTimestamp(uint64_t ts)
    {
        packet_.set_timestamp(ts);
        return *this;
    }

    CmdBuilder& AddValidUntil(uint64_t ts)
    {
        packet_.set_valid_until(ts);
        return *this;
    }

    CmdBuilder& AddChainId(const char* chain_id)
    {
        packet_.mutable_command()->set_chain_id(chain_id);
        return *this;
    }

    CmdBuilder& AddCmdConfigure()
    {
        packet_.mutable_command()->mutable_payload()->mutable_configure();
        return *this;
    }

    CmdBuilder& AddCmdSetState()
    {
        packet_.mutable_command()->mutable_payload()->mutable_set_state();
        return *this;
    }

    CmdBuilder& AddCmdSetParams()
    {
        packet_.mutable_command()->mutable_payload()->mutable_set_params();
        return *this;
    }

    CmdBuilder& AddCmdGetParams()
    {
        packet_.mutable_command()->mutable_payload()->mutable_set_params();
        return *this;
    }

    CmdBuilder& AddCmdPing()
    {
        packet_.mutable_command()->mutable_payload()->mutable_ping();
        return *this;
    }

    iot::backend::proto::Packet get()
    {
        return packet_;
    }

private:
    iot::backend::proto::Packet packet_;
};

class TelemetryBuilder {
private:
    TelemetryBuilder() {}

public:
    static TelemetryBuilder Create() { return TelemetryBuilder(); }

    TelemetryBuilder& AddVersion(unsigned major, unsigned minor)
    {
        uint32_t ver = (major << 16) | (minor & 0xFFFF);
        packet_.set_version(ver);
        return *this;
    }

    TelemetryBuilder& AddTimestamp(uint64_t ts)
    {
        packet_.set_timestamp(ts);
        return *this;
    }

    TelemetryBuilder& AddBatteryLevel(uint32_t level)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_battery_level(level);
        return *this;
    }

    TelemetryBuilder& AddSpeed(float speed)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_speed_kmh(speed);
        return *this;
    }

    TelemetryBuilder& AddLocation(float lat, float lon)
    {
        packet_.mutable_telemetry()->mutable_payload()->mutable_location()->set_lat(lat);
        packet_.mutable_telemetry()->mutable_payload()->mutable_location()->set_lon(lon);
        return *this;
    }

    TelemetryBuilder& AddVoltage(unsigned v)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_voltage(v);
        return *this;
    }

    TelemetryBuilder& AddGsmLevel(unsigned gsm_level)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_gsm_signal_level(gsm_level);
        return *this;
    }

    TelemetryBuilder& AddChargeStatus(bool status)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_charging(status);
        return *this;
    }

    TelemetryBuilder& AddLocked(bool locked)
    {
        packet_.mutable_telemetry()->mutable_payload()->set_locked(locked);
        return *this;
    }

    TelemetryBuilder& AddSensor(const char* key, const char* value)
    {
        (*packet_.mutable_telemetry()->mutable_payload()->mutable_sensors())[key] = value;
        return *this;
    }

    iot::backend::proto::Packet get()
    {
        return packet_;
    }

private:
    iot::backend::proto::Packet packet_;
};

class CommandResultBuilder {
private:
    CommandResultBuilder() {}

public:
    static CommandResultBuilder Create() { return CommandResultBuilder(); }

    CommandResultBuilder& AddDefaultPacketHeader()
    {
        AddTimestamp(1675917321);
        AddVersion(1,1);
        AddChainId("123");
        return *this;
    }

    CommandResultBuilder& AddVersion(unsigned major, unsigned minor)
    {
        uint32_t ver = (major << 16) | (minor & 0xFFFF);
        packet_.set_version(ver);
        return *this;
    }

    CommandResultBuilder& AddTimestamp(uint64_t ts)
    {
        packet_.set_timestamp(ts);
        return *this;
    }

    CommandResultBuilder& AddChainId(const char* id)
    {
        packet_.mutable_command_result()->set_chain_id(id);
        return *this;
    }

    CommandResultBuilder& AddResult(iot::backend::proto::ResultCode res)
    {
        packet_.mutable_command_result()->set_result(res);
        return *this;
    }

    CommandResultBuilder& AddErrMessage(const char* mes)
    {
        packet_.mutable_command_result()->mutable_error_description()->set_message(mes);
        return *this;
    }

    CommandResultBuilder& AddErrCode(iot::backend::proto::ErrorCode code)
    {
        packet_.mutable_command_result()->mutable_error_description()->set_status(code);
        return *this;
    }

    iot::backend::proto::Packet get()
    {
        return packet_;
    }

private:
    iot::backend::proto::Packet packet_;
};

TEST(BikeIotProto_Command, valid_configure) {
    auto packet = CmdBuilder::Create()
            .AddDefaultPacketHeader()
            .AddCmdConfigure()
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
    packet.clear_valid_until();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, valid_set_params) {
    auto packet = CmdBuilder::Create()
            .AddDefaultPacketHeader()
            .AddCmdSetParams()
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
    packet.clear_valid_until();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, valid_set_state) {
    auto packet = CmdBuilder::Create()
            .AddDefaultPacketHeader()
            .AddCmdSetState()
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
    packet.clear_valid_until();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, valid_get_params) {
    auto packet = CmdBuilder::Create()
            .AddDefaultPacketHeader()
            .AddCmdGetParams()
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
    packet.clear_valid_until();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, valid_ping) {
    auto packet = CmdBuilder::Create()
            .AddDefaultPacketHeader()
            .AddCmdPing()
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
    packet.clear_valid_until();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, time_invalid) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917)
            .AddVersion(1,1)
            .AddChainId("123")
            .AddValidUntil(1675917321 + 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, time_unset) {
    auto packet = CmdBuilder::Create()
            .AddVersion(1,1)
            .AddChainId("123")
            .AddValidUntil(1675917321 + 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, version_zero) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(0, 0)
            .AddChainId("123")
            .AddValidUntil(1675917321 + 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, version_unset) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917321)
            .AddChainId("123")
            .AddValidUntil(1675917321 + 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, chain_id_unset) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddValidUntil(1675917321 + 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, deadline_behind) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddChainId("123")
            .AddValidUntil(1675917321 - 1)
            .AddCmdConfigure()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Command, command_unset) {
    auto packet = CmdBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddChainId("123")
            .AddValidUntil(1675917321 + 1)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, empty) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, really_empty) {
    auto packet = TelemetryBuilder::Create()
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, battery_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddBatteryLevel(10)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, battery_out_of_range) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddBatteryLevel(1000)
            .get();

    //We don't want to add any verificaion on data values. Verifier check only presence of some fields
    //except timestamps.
    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, speed_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddSpeed(1000)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, location_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddLocation(53.234, 32.334545)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, voltage_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddVoltage(12)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, gsm_signal_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddGsmLevel(17)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, charge_status_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddChargeStatus(true)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, locked_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddLocked(true)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}
TEST(BikeIotProto_Telemetry, custom_values_only) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddSensor("1", "2")
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_Telemetry, custom_values_empty) {
    auto packet = TelemetryBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .get();

    //Create an empty map
    packet.mutable_telemetry()->mutable_payload()->mutable_sensors();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, valid_full) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_SUCCESS)
            .AddErrMessage("Ok")
            .AddErrCode(iot::backend::proto::STATUS_OK)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, valid_minimal) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_SUCCESS)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, fail_wo_descr) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_FAILED)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, fail_wo_descr_code) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_FAILED)
            .AddErrMessage("Error")
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, fail_wo_descr_message) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_FAILED)
            .AddErrCode(iot::backend::proto::STATUS_OTHER)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, fail_valid) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_FAILED)
            .AddErrMessage("Error")
            .AddErrCode(iot::backend::proto::STATUS_OTHER)
            .get();

    ASSERT_TRUE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, fail_with_details) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddResult(iot::backend::proto::RESULT_FAILED)
            .get();

    packet.mutable_command_result()->mutable_payload()->mutable_get_params();
    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, chain_id_empty) {
    auto packet = CommandResultBuilder::Create()
            .AddDefaultPacketHeader()
            .AddChainId("")
            .AddResult(iot::backend::proto::RESULT_SUCCESS)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, chain_id_absent) {
    auto packet = CommandResultBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddResult(iot::backend::proto::RESULT_SUCCESS)
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}

TEST(BikeIotProto_CommandResult, result_unset) {
    auto packet = CommandResultBuilder::Create()
            .AddTimestamp(1675917321)
            .AddVersion(1,1)
            .AddChainId("123")
            .get();

    ASSERT_FALSE(iot::backend::proto::IsValid(packet));
}
