//
// Created by Joel Neumann on 02.07.24.
//

#ifndef DREA_V2_PROTOCOL_ENCODER_H
#define DREA_V2_PROTOCOL_ENCODER_H

#include "Configuration/Configurations.h"
#include "MultiKnobData.h"
#include "Protocol.h"
#include <Touches/Touch.h>
#include <Arduino.h>
#include <vector>


class ProtocolEncoder {
private:
    std::vector<uint8_t> data_;

    void appendChecksum();
    void appendInt(int value);
    void appendFloat(float value);
    void appendMotorAngle(float angle);
    void appendMotorAngleDelta(float angle_delta);
    void appendMotorSnapPoint(int snap_point);
    void appendButtonState(ButtonState);
    void appendButtonStateDelta(ButtonDeltaState);
    void appendTouchCount(uint8_t touch_count);
    void appendTouchCountDelta(uint8_t touch_count_delta);
    void appendTouchPressure(int32_t touch_pressure);
    void appendTouchCountAndTouches(uint8_t touch_count, TouchFormat, const TouchVector &touches, float motor_angle);
    void appendRequestMultiKnobState(const MultiKnobState::State &);
    void appendRequestSenderConfig(const SenderConfig &);
    void appendRequestTarget(float target);
    void appendMotorConfig(const MotorConfig &, const TouchCount &count);

public:
    std::vector<uint8_t> createPacket(const SenderConfig &, const MultiKnobData &);
    std::vector<uint8_t> createRequestOnOffPacket(const MultiKnobState::State &);
    std::vector<uint8_t> createRequestSenderFormatPacket(const SenderConfig &);
    std::vector<uint8_t> createRequestTargetPacket(float target);
    std::vector<uint8_t> createMotorConfigPacket(const MotorConfig &, const TouchCount &);
};

#endif// DREA_V2_PROTOCOL_ENCODER_H
