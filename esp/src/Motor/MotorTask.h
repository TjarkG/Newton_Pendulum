//
// Created by Joel Neumann on 21.12.23.
//

#ifndef DREA_V2_MOTOR_CONTROLLER_H
#define DREA_V2_MOTOR_CONTROLLER_H

#define SENSOR_PIN 16
#define MOTOR_POLE_PAIRS 11
#define DRIVER_PINS 32, 33, 25, 26, 27, 14

///This can be adjusted to change the strength base for the MultiKnob. The bigger the number the stronger the strength.
#define STRENGTH_MULTIPLIER 0.3f

#include "Configuration/Configurations.h"
#include "MultiKnobData.h"
#include "SimpleFOC.h"
#include "task.h"

class MotorTask final : public Task<MotorTask>
{
	friend class Task; // Allow base Task to invoke protected run()

	enum class MotorCommandType
	{
		SET_TARGET,
		PERFORM_ACTION,
		UPDATE_CONFIG
	};

	struct MotorCommand
	{
		MotorCommandType type;

		union CommandData
		{
			float new_target;
			MotorAction action;
			TouchConfigUpdate new_config;
		};

		CommandData data;
	};


	Configurations &configurations_;
	bool motor_active_ = true;

	MagneticSensorSPI sensor_ = MagneticSensorSPI(AS5048_SPI, SENSOR_PIN);
	BLDCMotor *motor_ = new BLDCMotor(MOTOR_POLE_PAIRS);
	BLDCDriver6PWM driver_ = BLDCDriver6PWM(DRIVER_PINS);

	QueueHandle_t motor_command_queue_;

	MotorData motor_data_{
		.angle = 0,
		.angle_delta = 0,
		.current_snap_point = 0,
		.target = 0
	};
	float last_angle_ = 0;

	uint8_t touch_count_ = 0;

	void setup();

	void hapticFeedback(float strength, uint8_t direction_length) const;

	static float calculateForce(float current_position, float target_position, uint8_t strength);

	static float calculateOpenSnapTarget(float current_angle, int snaps);

	static float calculateBorderSnapTarget(float current_angle, int snaps, float lower_border, float upper_border);

	void moveWithForce(float force) const;

	void updateAngle(float angle);

	void updateSnapPointOpen(float target, int snaps);

	void updateSnapPointWithBorder(float target, int snaps, float lower_border, float upper_border);

	void handleQueue();

	void hitTargetAction();

	void updateConfig(const TouchConfigUpdate &) const;

	void performAction(MotorAction action);

	///Each mode function should update the Data
	void runHitTargetMode(const TouchConfig &);

	void runSnapsOpenMode(const TouchConfig &);

	void runSnapsWithBorderMode(const TouchConfig &);

protected:
	[[noreturn]] void run();

public:
	MotorTask(int task_core, Configurations &configurations);

	[[nodiscard]] std::function<MotorData()> getDataCallback() const;

	std::function<void()> getUpdateAngleDeltaCallback();

	[[nodiscard]] std::function<void(float)> getSetTargetCallback() const;

	[[nodiscard]] std::function<void(MotorAction)> getPerformMotorActionCallback() const;

	[[nodiscard]] std::function<void(TouchConfigUpdate &)> getUpdateMotorConfigCallback() const;

	std::function<void(uint8_t)> getUpdateTouchCountCallback();
};

#endif// DREA_V2_MOTOR_CONTROLLER_H
