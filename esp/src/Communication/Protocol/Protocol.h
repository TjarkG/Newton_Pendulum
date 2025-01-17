//
// Created by Joel Neumann on 02.07.24.
//

#ifndef DREA_V2_PROTOCOL_H
#define DREA_V2_PROTOCOL_H

#include <Arduino.h>

enum MotorAction : uint8_t
{
	ACTION_HAPTIC_FEEDBACK = 0x01,
	ACTION_HIT_TARGET = 0x02
};

enum MotorConfiguration : uint8_t
{
	CONFIGURATION_HIT_TARGET = 0x01,
	CONFIGURATION_SNAPS_OPEN = 0x02,
	CONFIGURATION_SNAPS_WITH_BORDER = 0x03
};

enum TouchCount : uint8_t
{
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
