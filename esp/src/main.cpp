//
// Created by Joel Neumann on 02.01.24.
//


//#include "Configuration/MotorConfig.h"
//#include "Motor/MotorTask.h"
#include "SimpleFOC.h"
#include <Arduino.h>

#define PRESCALER 3
#define SENSOR_PIN 16
#define MOTOR_POLE_PAIRS 11
#define DRIVER_PINS 32, 33, 25, 26, 27, 14

///This can be adjusted to change the strength base for the MultiKnob. The bigger the number the stronger the strength.
#define STRENGTH_MULTIPLIER 0.3f

void moveWithForce(BLDCMotor * const motor, const float force)
{
	motor->PID_velocity.P = fabsf(force);
	//PID_velocity(force) calculates the move force based on the PID values
	motor->move(motor->PID_velocity(force));
}

float calculateForce(const float current_position, const float target_position, const uint8_t strength)
{
	const float position_difference = target_position - current_position;
	return position_difference * static_cast<float>(strength) * STRENGTH_MULTIPLIER;
}

void setup()
{
	Serial.begin(115200);

	auto sensor_ = MagneticSensorSPI(AS5048_SPI, SENSOR_PIN);
	auto *motor = new BLDCMotor(MOTOR_POLE_PAIRS);
	auto driver = BLDCDriver6PWM(DRIVER_PINS);

	sensor_.init();

	driver.voltage_power_supply = 5;
	driver.init();
	motor->linkDriver(&driver);
	motor->linkSensor(&sensor_);

	/*motor->foc_modulation = SpaceVectorPWM;
	//Angle and velocity control works not that good
	motor->controller = torque;
	motor->voltage_limit = 5;

	//Voltage for the calibration
	motor->voltage_sensor_align = 1;

	motor->PID_velocity.P = 100;
	motor->PID_velocity.I = 10;
	motor->PID_velocity.D = 10;
	motor->PID_velocity.output_ramp = 10000;*/
	motor->controller = velocity;

	motor->init();

	motor->initFOC();

	//motor->enable();
	sleep(4000);

	// ReSharper disable once CppDFAEndlessLoop
	while (true)
	{
		//moveWithForce(calculateForce(motor->shaft_angle, motor_data_.target, current_config.strength));
		motor->loopFOC();
		motor->move(2);
		/*sleep(1000);
		motor->move(0);
		sleep(1000);*/
	}
}


void loop() {}
