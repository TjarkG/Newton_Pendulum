//
// Created by Joel Neumann on 11.07.24.
//

#include "SenderConfig.h"


SenderConfig::SenderConfig(
	bool b0,
	bool b1,
	bool b2,
	bool b3,
	bool b4,
	bool b5,
	bool b6,
	bool b7,
	bool b8,
	TouchFormat format) : motor_angle(b0),
	                      motor_angle_delta(b1),
	                      motor_snap_point(b2),
	                      button_state(b3),
	                      button_state_delta(b4),
	                      touch_count(b5),
	                      touch_count_delta(b6),
	                      touch_pressure(b7),
	                      touch_count_and_touches(b8),
	                      touch_format(format) {}


SenderConfig::SenderConfig() : motor_angle(false),
                               motor_angle_delta(false),
                               motor_snap_point(false),
                               button_state(false),
                               button_state_delta(false),
                               touch_count(false),
                               touch_count_delta(false),
                               touch_pressure(false),
                               touch_count_and_touches(false),
                               touch_format(POSITION_PRESSURE_CHANNELS) {}

SenderConfig SenderConfig::getStandardConfig()
{
	return SenderConfig{true, false, true, true, false, false, false, false, true, POSITION_PRESSURE_CHANNELS};
}
