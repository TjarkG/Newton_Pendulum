//
// Created by Joel Neumann on 21.12.23.
//

#ifndef DREA_V2_BUTTON_HANDLER_H
#define DREA_V2_BUTTON_HANDLER_H

#define BUTTON_PIN 15

#include "task.h"
#include "Arduino.h"
#include "Configuration/SenderConfig.h"
#include "MultiKnobData.h"


class ButtonTask : public Task<ButtonTask>
{
	friend class Task<ButtonTask>;

private:
	const SemaphoreHandle_t done_semaphore_ = xSemaphoreCreateBinary();

	const SenderConfig &sender_config_;

	ButtonData button_data_{
		.state = NOT_PRESSED,
		.delta = NO_CHANGE
	};
	ButtonState last_delta_state_ = NOT_PRESSED;

	void updateDelta();

protected:
	[[noreturn]] void run();

public:
	ButtonTask(int task_core, SenderConfig &config);

	std::function<ButtonData()> getDataCallback() const;

	TaskHandles getHandles() const;
};

#endif// DREA_V2_BUTTON_HANDLER_H
