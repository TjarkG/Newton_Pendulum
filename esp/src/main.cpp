//
// Created by Joel Neumann on 02.01.24.
//


#include "Button/ButtonTask.h"
#include "Communication/CommunicationTask.h"
#include "Configuration/Configurations.h"
#include "Motor/MotorTask.h"
#include "SideTouch/SideTouchTask.h"
#include <Arduino.h>


#define PRESCALER 3

void setup()
{
	Serial.begin(115200);

	Configurations configurations;

	auto motor_task = MotorTask(1, configurations);

	auto button_task = ButtonTask(0, configurations.sender_config);
	auto side_touch_task = SideTouchTask(0, configurations.sender_config, PRESCALER,
	                                              motor_task.getUpdateTouchCountCallback());

	auto communication_task = CommunicationTask(
		0,
		button_task.getHandles(),
		side_touch_task.getHandles(),
		configurations,
		motor_task.getUpdateAngleDeltaCallback(),
		button_task.getDataCallback(),
		side_touch_task.getDataCallback(),
		motor_task.getDataCallback(),
		motor_task.getSetTargetCallback(),
		motor_task.getPerformMotorActionCallback(),
		motor_task.getUpdateMotorConfigCallback()
	);


	//Delete loop task
	vTaskDelete(nullptr);
}


void loop() {}
