//
// Created by Joel Neumann on 20.06.24.
//

#ifndef DREA_V2_SERIAL_COMMUNICATOR_H
#define DREA_V2_SERIAL_COMMUNICATOR_H


#include "Communication/Protocol/ProtocolDecoder.h"
#include "Communication/Protocol/ProtocolEncoder.h"
#include "Configuration/Configurations.h"
#include <PacketSerial.h>


class SerialCommunicator {
private:
    PacketSerial_<COBS> my_packet_serial_;
    ProtocolDecoder decoder_;
    ProtocolEncoder encoder_;

    std::function<MotorData()> get_motor_data_callback_;

    void packetHandler(const uint8_t *buffer, size_t);

public:
    static SerialCommunicator &getInstance();

    void setup(
            const std::function<void(const SenderConfig &)>& set_config_callback,
            const std::function<void(const RequestedData &)>& set_request_data_callback,
            const std::function<void(const MultiKnobState::State &)>& set_multi_knob_state_callback,
            const std::function<void(float)>& set_motor_target_callback,
            const std::function<MotorData()> &get_motor_data_callback,
            const std::function<void(MotorAction)> &perform_motor_action_callback,
            const std::function<void(TouchConfigUpdate&)> &update_motor_config_callback);
    void read();
    void send(const MultiKnobData &, const SenderConfig &);
    void sendRequestedData(const RequestedData &, const Configurations &);
};

#endif// DREA_V2_SERIAL_COMMUNICATOR_H
