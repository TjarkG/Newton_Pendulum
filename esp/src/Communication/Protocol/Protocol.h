//
// Created by Joel Neumann on 02.07.24.
//

#ifndef DREA_V2_PROTOCOL_H
#define DREA_V2_PROTOCOL_H

#include <Arduino.h>

enum SenderCommand : uint8_t {
    COMMAND_ON_OFF = 0x01,
    COMMAND_SENDER_FORMAT = 0x02,
    COMMAND_SET_TARGET = 0x03,
    COMMAND_MOTOR_CONFIGURATION = 0x04,
    COMMAND_MOTOR_ACTION = 0x05,
    COMMAND_REQUEST_DATA = 0x06
};

enum OnOffCommand : uint8_t {
    COMMAND_OFF = 0x00,
    COMMAND_ON = 0x01
};

enum SenderFormat : uint8_t {
    FORMAT_INFORMATION = 0x00,
    FORMAT_MOTOR_ANGLE = 0x01,
    FORMAT_MOTOR_ANGLE_DELTA = 0x02,
    FORMAT_MOTOR_SNAP_POINT = 0x03,
    FORMAT_BUTTON_STATE = 0x04,
    FORMAT_BUTTON_STATE_DELTA = 0x05,
    FORMAT_TOUCH_COUNT = 0x06,
    FORMAT_TOUCH_COUNT_DELTA = 0x07,
    FORMAT_TOUCH_PRESSURE = 0x08,
    FORMAT_TOUCH_COUNT_AND_TOUCHES = 0x09
};

enum TouchFormat : uint8_t {
    POSITION_PRESSURE_CHANNELS = 0x01,
    POSITION_PRESSURE = 0x02,
    RELATIVE_POSITION_PRESSURE_CHANNELS = 0x03
};

enum MotorAction : uint8_t {
    ACTION_HAPTIC_FEEDBACK = 0x01,
    ACTION_HIT_TARGET = 0x02
};

enum MotorConfiguration : uint8_t {
    CONFIGURATION_HIT_TARGET = 0x01,
    CONFIGURATION_SNAPS_OPEN = 0x02,
    CONFIGURATION_SNAPS_WITH_BORDER = 0x03
};

enum RequestData : uint8_t {
    REQUEST_ON_OFF = 0x01,
    REQUEST_SEND_FORMAT = 0x02,
    REQUEST_TARGET = 0x03,
    REQUEST_MOTOR_CONFIGURATION = 0x04
};

enum TouchCount : uint8_t {
    ALL = 0x01,
    ZERO = 0x02,
    ONE = 0x03,
    TWO = 0x04,
    THREE = 0x05,
    FOUR = 0x06,
    FIVE = 0x07,
    GREATER_THAN_FIVE = 0x08
};

size_t touchCountToIndex(TouchCount count);


#endif//DREA_V2_PROTOCOL_H
