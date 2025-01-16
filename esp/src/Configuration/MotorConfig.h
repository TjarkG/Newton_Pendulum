//
// Created by Joel Neumann on 11.07.24.
//

#ifndef DREA_V2_MOTOR_CONFIG_H
#define DREA_V2_MOTOR_CONFIG_H

#include "Communication/Protocol/Protocol.h"
#include <array>
#include <cstdint>

struct TouchConfig
{
	uint8_t strength;
	MotorConfiguration mode;
	int snaps;
	float lower_border;
	float upper_border;
};

struct TouchConfigUpdate
{
	TouchCount count;
	TouchConfig config;
};


class MotorConfig
{
public:
	MotorConfig();

	void setTouchConfig(size_t index, TouchConfig config);

	void setAllTouchConfigs(TouchConfig config);

	[[nodiscard]] TouchConfig getTouchConfig(size_t index) const;

private:
	std::array<TouchConfig, 7> touch_configs_{};
};

#endif//DREA_V2_MOTOR_CONFIG_H
