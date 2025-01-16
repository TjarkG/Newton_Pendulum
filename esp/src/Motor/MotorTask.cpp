//
// Created by Joel Neumann on 21.12.23.
//

#include "MotorTask.h"

#include <Preferences.h>

MotorTask::MotorTask(int task_core, Configurations &configs)
	: Task("Motor", 10000, 1, task_core), configurations_(configs)
{
	motor_command_queue_ = xQueueCreate(10, sizeof(MotorCommand));
	beginTask();
}

void MotorTask::setup()
{
	sensor_.init();

	driver_.voltage_power_supply = 5;
	driver_.init();
	motor_->linkDriver(&driver_);
	motor_->linkSensor(&sensor_);

	motor_->foc_modulation = FOCModulationType::SpaceVectorPWM;
	//Angle and velocity control works not that good
	motor_->controller = MotionControlType::torque;
	motor_->voltage_limit = 5;

	//Voltage for the calibration
	motor_->voltage_sensor_align = 1;

	motor_->PID_velocity.P = 0;
	motor_->PID_velocity.I = 0;
	motor_->PID_velocity.D = 0;
	motor_->PID_velocity.output_ramp = 10000;

	//TODO: Move this elsewhere
	Preferences flash_storage;
	flash_storage.begin("motor", false);
	const char *namespace_zero_electric_angle = "zea";

	motor_->init();
	//Setting zero_electric_angle and sensor_direction before initFOC() skips the calibration
	if (flash_storage.isKey(namespace_zero_electric_angle))
	{
		motor_->zero_electric_angle = flash_storage.getFloat(namespace_zero_electric_angle);
		motor_->sensor_direction = Direction::CCW;
	}
	motor_->initFOC();
	if (!flash_storage.isKey(namespace_zero_electric_angle))
	{
		Serial.print("motor zero electric angle: ");
		Serial.println(motor_->zero_electric_angle);
		flash_storage.putFloat(namespace_zero_electric_angle, motor_->zero_electric_angle);
	}
}

float MotorTask::calculateForce(const float current_position, const float target_position, const uint8_t strength)
{
	const float position_difference = target_position - current_position;
	return position_difference * static_cast<float>(strength) * STRENGTH_MULTIPLIER;
}

float MotorTask::calculateOpenSnapTarget(const float current_angle, const int snaps)
{
	const float radians_per_snap_point = static_cast<float>(2 * M_PI) / static_cast<float>(snaps);
	const int closest_snap_point_index = static_cast<int>(round(current_angle / radians_per_snap_point));
	const float closest_snap_point_radians = static_cast<float>(closest_snap_point_index) * radians_per_snap_point;
	return closest_snap_point_radians;
}


float MotorTask::calculateBorderSnapTarget(const float current_angle, const int snaps, const float lower_border,
                                           const float upper_border)
{
	const float radians_per_snap_point = (upper_border - lower_border) / static_cast<float>(snaps - 1);
	int closest_snap_point_index = static_cast<int>(round((current_angle - lower_border) / radians_per_snap_point));

	if (closest_snap_point_index < 0)
	{
		closest_snap_point_index = 0;
	} else if (closest_snap_point_index >= snaps)
	{
		closest_snap_point_index = snaps - 1;
	}

	const float closest_snap_point_radians = lower_border + static_cast<float>(closest_snap_point_index) *
	                                         radians_per_snap_point;
	return closest_snap_point_radians;
}


void MotorTask::moveWithForce(const float force) const
{
	motor_->PID_velocity.P = fabsf(force);
	//PID_velocity(force) calculates the move force based on the PID values
	motor_->move(motor_->PID_velocity(force));
}

void MotorTask::updateAngle(const float angle)
{
	motor_data_.angle = angle;
}

void MotorTask::updateSnapPointOpen(const float target, const int snaps)
{
	motor_data_.current_snap_point = static_cast<int>(round(target / (2 * M_PI / snaps)));
}

//TODO: Check this
void MotorTask::updateSnapPointWithBorder(const float target, const int snaps, const float lower_border,
                                          const float upper_border)
{
	motor_data_.current_snap_point = static_cast<int>(round(target / (upper_border - lower_border) / static_cast<float>(snaps - 1)));
}


[[noreturn]] void MotorTask::run()
{
	setup();

	while (true)
	{
		//runs motor move function
		motor_->loopFOC();

		handleQueue();

		if (configurations_.multi_knob_state.getState() == MultiKnobState::State::ON)
		{
			if (!motor_active_)
			{
				motor_active_ = true;
				motor_->enable();
			}

			TouchConfig current_config = configurations_.motor_config.getTouchConfig(touch_count_);
			switch (current_config.mode)
			{
				case CONFIGURATION_HIT_TARGET:
					runHitTargetMode(current_config);
					break;
				case CONFIGURATION_SNAPS_OPEN:
					runSnapsOpenMode(current_config);
					break;
				case CONFIGURATION_SNAPS_WITH_BORDER:
					runSnapsWithBorderMode(current_config);
					break;
			}
		} else
		{
			if (motor_active_)
			{
				motor_active_ = false;
				motor_->disable();
			}
		}
	}
}


void MotorTask::runHitTargetMode(const TouchConfig &current_config)
{
	const float current_angle = motor_->shaft_angle;

	moveWithForce(calculateForce(current_angle, motor_data_.target, current_config.strength));

	updateAngle(current_angle);
}

void MotorTask::runSnapsOpenMode(const TouchConfig &current_config)
{
	const float current_angle = motor_->shaft_angle;
	if (current_config.snaps != 0)
	{
		const float target = calculateOpenSnapTarget(current_angle, current_config.snaps);
		moveWithForce(calculateForce(current_angle, target, current_config.strength));

		updateSnapPointOpen(target, current_config.snaps);
	} else
	{
		motor_->move(0);
	}
	updateAngle(current_angle);
}

void MotorTask::runSnapsWithBorderMode(const TouchConfig &current_config)
{
	const float current_angle = motor_->shaft_angle;

	if (current_angle < current_config.lower_border)
	{
		moveWithForce(calculateForce(current_angle, current_config.lower_border, 255));
	} else if (current_angle > current_config.upper_border)
	{
		moveWithForce(calculateForce(current_angle, current_config.upper_border, 255));
	} else
	{
		if (current_config.snaps > 0)
		{
			const float target = calculateBorderSnapTarget(current_angle, current_config.snaps,
			                                               current_config.lower_border,
			                                               current_config.upper_border);
			moveWithForce(calculateForce(current_angle, target, current_config.strength));

			updateSnapPointWithBorder(target, current_config.snaps, current_config.lower_border,
			                          current_config.upper_border);
		} else
		{
			motor_->move(0);
		}
	}
	updateAngle(current_angle);
}

void MotorTask::handleQueue()
{
	MotorCommand command{};
	while (xQueueReceive(motor_command_queue_, &command, 0) == pdTRUE)
	{
		switch (command.type)
		{
			case MotorCommandType::SET_TARGET:
				motor_data_.target = command.data.new_target;
				break;
			case MotorCommandType::PERFORM_ACTION:
				performAction(command.data.action);
				break;
			case MotorCommandType::UPDATE_CONFIG:
				updateConfig(command.data.new_config);
				break;
		}
	}
}


std::function<void(TouchConfigUpdate &)> MotorTask::getUpdateMotorConfigCallback() const
{
	return [this](const TouchConfigUpdate &new_config) {
		const MotorCommand command = {
			.type = MotorCommandType::UPDATE_CONFIG,
			.data = {
				.new_config = new_config,
			}
		};
		xQueueSend(motor_command_queue_, &command, portMAX_DELAY);
	};
}


void MotorTask::updateConfig(const TouchConfigUpdate &new_config) const
{
	if (new_config.count == ALL)
	{
		configurations_.motor_config.setAllTouchConfigs(new_config.config);
	} else
	{
		configurations_.motor_config.setTouchConfig(touchCountToIndex(new_config.count), new_config.config);
	}
}


std::function<void(MotorAction)> MotorTask::getPerformMotorActionCallback() const
{
	return [this](const MotorAction action) {
		const MotorCommand command = {
			.type = MotorCommandType::PERFORM_ACTION,
			.data = {
				.action = action,
			}
		};
		xQueueSend(motor_command_queue_, &command, portMAX_DELAY);
	};
}

void MotorTask::performAction(const MotorAction action)
{
	if (motor_active_)
	{
		switch (action)
		{
			case ACTION_HAPTIC_FEEDBACK:
				hapticFeedback(3, 5);
				break;
			case ACTION_HIT_TARGET:
				hitTargetAction();
				break;
		}
	}
}

void MotorTask::hapticFeedback(const float strength, const uint8_t direction_length) const
{
	motor_->PID_velocity.P = fabsf(strength);
	motor_->move(strength);
	for (uint8_t i = 0; i < direction_length; i++)
	{
		motor_->loopFOC();
		delay(1);
	}
	motor_->move(-strength);
	for (uint8_t i = 0; i < direction_length; i++)
	{
		motor_->loopFOC();
		delay(1);
	}
	motor_->PID_velocity.P = 0;
	motor_->move(0);
	vTaskDelay(1);
}

void MotorTask::hitTargetAction()
{
	uint8_t hits = 0;
	while (true)
	{
		motor_->loopFOC();

		const float current_angle = motor_->shaft_angle;
		moveWithForce(calculateForce(current_angle, motor_data_.target, 255));

		updateAngle(current_angle);

		//Has to be nearby the target for a given time to avoid overshoots
		const float dif = motor_data_.target - current_angle;
		if (dif <= M_PI / 180 && dif >= -M_PI / 180)
		{
			hits++;
		} else
		{
			hits = 0;
		}
		if (hits == 75) return;
	}
}

std::function<void()> MotorTask::getUpdateAngleDeltaCallback()
{
	return [this] {
		motor_data_.angle_delta = motor_data_.angle - last_angle_;
		last_angle_ = motor_data_.angle;
	};
}

std::function<MotorData()> MotorTask::getDataCallback() const
{
	return [this] {
		return motor_data_;
	};
}

std::function<void(float)> MotorTask::getSetTargetCallback() const
{
	return [this](const float new_target) {
		const MotorCommand command = {
			.type = MotorCommandType::SET_TARGET,
			.data = {
				.new_target = new_target,
			}
		};
		xQueueSend(motor_command_queue_, &command, portMAX_DELAY);
	};
}

std::function<void(uint8_t)> MotorTask::getUpdateTouchCountCallback()
{
	return [this](const uint8_t new_touch_count) {
		touch_count_ = new_touch_count;
	};
}
