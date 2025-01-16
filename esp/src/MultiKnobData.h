//
// Created by Joel Neumann on 12.07.24.
//

#ifndef DREA_V2_MULTI_KNOB_DATA_H
#define DREA_V2_MULTI_KNOB_DATA_H

#include <Touches/Touch.h>
#include <Arduino.h>

enum ButtonState : uint8_t
{
	NOT_PRESSED = 0x00,
	PRESSED = 0x01
};

enum ButtonDeltaState : uint8_t
{
	NO_CHANGE = 0x00,
	PRESSED_TO_NOT_PRESSED = 0x01,
	NOT_PRESSED_TO_PRESSED = 0x02
};

struct ButtonData
{
	ButtonState state;
	ButtonDeltaState delta;
};

struct SideTouchData
{
	uint8_t count;
	uint8_t count_delta;
	int32_t pressure;
	TouchVector touches;
};

struct MotorData
{
	float angle;
	float angle_delta;
	int current_snap_point;
	float target;
};

struct MultiKnobData
{
	float motor_angle;
	float motor_angle_delta;
	int motor_snap_point;
	ButtonState button_state;
	ButtonDeltaState button_state_delta;
	uint8_t touch_count;
	uint8_t touch_count_delta;
	int touch_pressure;
	TouchVector touches;
};

#endif//DREA_V2_MULTI_KNOB_DATA_H
