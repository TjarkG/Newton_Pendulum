//
// Created by Joel Neumann on 02.01.24.
//


#include "Configuration/Configurations.h"
#include "Motor/MotorTask.h"
#include <Arduino.h>


#define PRESCALER 3

void setup()
{
	Serial.begin(115200);

	Configurations configurations;

	auto motor_task = MotorTask(1, configurations);

	//Delete loop task
	vTaskDelete(nullptr);
}


void loop() {}
