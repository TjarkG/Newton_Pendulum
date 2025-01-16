//
// Created by Joel Neumann on 11.07.24.
//

#ifndef DREA_V2_CONFIGURATIONS_H
#define DREA_V2_CONFIGURATIONS_H

#include "MotorConfig.h"
#include "MultiKnobState.h"
#include "SenderConfig.h"

struct Configurations
{
	MultiKnobState multi_knob_state;
	SenderConfig sender_config = SenderConfig::getStandardConfig();
	MotorConfig motor_config;
};

#endif//DREA_V2_CONFIGURATIONS_H
