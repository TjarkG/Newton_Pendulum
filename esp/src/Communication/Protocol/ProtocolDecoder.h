//
// Created by Joel Neumann on 02.07.24.
//

#ifndef DREA_V2_PROTOCOL_DECODER_H
#define DREA_V2_PROTOCOL_DECODER_H

#include "Configuration/Configurations.h"
#include "Protocol.h"
#include <Arduino.h>

typedef struct {
    RequestData data;
    TouchCount touch_count;
} RequestedData;

class ProtocolDecoder {
private:
    std::function<void(const SenderConfig &)> update_sender_config_callback_;
    std::function<void(const RequestedData &)> request_data_callback_;
    std::function<void(const MultiKnobState::State &)> set_multi_knob_callback_;
    std::function<void(float)> set_motor_target_;
    std::function<void(MotorAction)> perform_motor_action_callback_;
    std::function<void(TouchConfigUpdate&)> update_motor_config_callback_;

    void handleCommands(const uint8_t *buffer, size_t length);
    void handleCommandOnOff(const uint8_t *data, size_t length);
    void handleCommandSendFormat(const uint8_t *data, size_t length);
    void handleCommandSetTarget(const uint8_t *data, size_t length);
    void handleCommandMotorConfig(const uint8_t *data, size_t length);
    void handleCommandMotorAction(const uint8_t *data, size_t length);
    void handleRequestData(const uint8_t *data, size_t length);
    static bool hasMinPacketSize(size_t length);
    static bool hasCorrectChecksum(const uint8_t *buffer, size_t length);

public:
    void decode(const uint8_t *buffer, size_t length);
    void setCallbacks(
            const std::function<void(const SenderConfig &)> &,
            const std::function<void(const RequestedData &)> &,
            const std::function<void(const MultiKnobState::State &)> &,
            const std::function<void(float)> &,
            const std::function<void(MotorAction)> &perform_motor_action_callback,
            const std::function<void(TouchConfigUpdate&)> &update_motor_config_callback);
};

#endif// DREA_V2_PROTOCOL_DECODER_H
