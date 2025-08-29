#include <iostream>
#include "iot_scale/cpp/proto/packet.pb.h"
#include "iot_scale/cpp/src/packet_validator.h"
#include <google/protobuf/util/json_util.h>

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    try {
        iot::backend::proto::Packet cmd_set_params;
        cmd_set_params.set_version(0x10000);
        cmd_set_params.set_timestamp(1675927022);
        cmd_set_params.set_valid_until(1675927322); //Thursday, February 9, 2023 7:22:02 AM
        cmd_set_params.mutable_command()->set_chain_id("123-456");
        auto* cmd = cmd_set_params.mutable_command()->mutable_payload();
        auto& params = *cmd->mutable_set_params()->mutable_params();
        params["lock"] = "true";
        params["color"] = "yallow";

        TProtoStringType s;
        google::protobuf::util::MessageToJsonString(cmd_set_params, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(cmd_set_params) << std::endl;

        iot::backend::proto::Packet cmd_get_params;
        cmd_get_params.set_version(0x10000);
        cmd_get_params.set_timestamp(1675927022);
        cmd_get_params.set_valid_until(1675927322); //Thursday, February 9, 2023 7:22:02 AM
        cmd_get_params.mutable_command()->set_chain_id("456-789");
        auto& get_params = *cmd_get_params.mutable_command()->mutable_payload()->mutable_get_params()->mutable_param();
        get_params.Add("imei");
        get_params.Add("fw_sn");
        get_params.Add("bat_sn");

        google::protobuf::util::MessageToJsonString(cmd_get_params, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(cmd_get_params) << std::endl;

        iot::backend::proto::Packet cmd_set_state;
        cmd_set_state.set_version(0x10000);
        cmd_set_state.set_timestamp(1675927022);
        cmd_set_state.set_valid_until(1675927322); //Thursday, February 9, 2023 7:22:02 AM
        cmd_set_state.mutable_command()->set_chain_id("789-123");
        auto& state = *cmd_set_state.mutable_command()->mutable_payload()->mutable_set_state();
        state.set_state("locked");
        google::protobuf::util::MessageToJsonString(cmd_set_state, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(cmd_set_state) << std::endl;

        iot::backend::proto::Packet cmd_res;
        cmd_res.set_version(0x10000);
        cmd_res.set_timestamp(1675927022);
        auto st = cmd_res.mutable_command_result();
        st->set_chain_id("123-456");
        st->set_result(iot::backend::proto::RESULT_FAILED);
        st->mutable_error_description()->set_message("No bike found");
        st->mutable_error_description()->set_status(iot::backend::proto::STATUS_OTHER);
        google::protobuf::util::MessageToJsonString(cmd_res, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(cmd_res) << std::endl;

        cmd_res.set_version(0x10000);
        cmd_res.set_timestamp(1675927022);
        st = cmd_res.mutable_command_result();
        st->set_chain_id("123-456");
        st->set_result(iot::backend::proto::RESULT_SUCCESS);
        st->clear_error_description();
        google::protobuf::util::MessageToJsonString(cmd_res, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(cmd_res) << std::endl;

        iot::backend::proto::Packet tele;
        tele.set_version(0x10000);
        tele.set_timestamp(1675927022);
        auto& payload = *tele.mutable_telemetry()->mutable_payload();
        payload.set_battery_level(24);
        payload.mutable_location()->set_lat(55.743544);
        payload.mutable_location()->set_lon(37.475560);
        payload.set_speed_kmh(15.43);
        auto addon = payload.mutable_sensors();
        (*addon)["Tyre pressure"] = "2";
        (*addon)["Wheel count"] = "1";
        google::protobuf::util::MessageToJsonString(tele, &s);
        std::cout << s << std::endl;
        s.clear();
        std::cout << iot::backend::proto::IsValid(tele) << std::endl;


    }
    catch (std::exception e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
    return 0;
}

