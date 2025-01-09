//
// Created by Joel Neumann on 11.07.24.
//

#ifndef DREA_V2_SENDER_CONFIG_H
#define DREA_V2_SENDER_CONFIG_H

#include "Communication/Protocol/Protocol.h"

struct SenderConfig {
    bool motor_angle;
    bool motor_angle_delta;
    bool motor_snap_point;
    bool button_state;
    bool button_state_delta;
    bool touch_count;
    bool touch_count_delta;
    bool touch_pressure;
    bool touch_count_and_touches;
    TouchFormat touch_format;

    SenderConfig();
    SenderConfig(bool, bool, bool, bool, bool, bool, bool, bool, bool, TouchFormat);

    static SenderConfig getStandardConfig();
};

#endif//DREA_V2_SENDER_CONFIG_H
