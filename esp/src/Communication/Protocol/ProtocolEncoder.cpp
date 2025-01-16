//
// Created by Joel Neumann on 02.07.24.
//

#include "ProtocolEncoder.h"
#include "Configuration/MotorConfig.h"
#include "crc16.h"

void ProtocolEncoder::appendChecksum()
{
	uint16_t crc = crc16(data_.data(), data_.size());
	data_.push_back(crc & 0xFF);
	data_.push_back((crc >> 8) & 0xFF);
}

void ProtocolEncoder::appendInt(int value)
{
	auto *int_ptr = (uint8_t *) &value;
	data_.insert(data_.end(), int_ptr, int_ptr + sizeof(int));
}

void ProtocolEncoder::appendFloat(float value)
{
	auto *float_ptr = (uint8_t *) &value;
	data_.insert(data_.end(), float_ptr, float_ptr + sizeof(float));
}

void ProtocolEncoder::appendMotorAngle(float angle)
{
	data_.push_back(FORMAT_MOTOR_ANGLE);
	appendFloat(angle);
}

void ProtocolEncoder::appendMotorAngleDelta(float angle_delta)
{
	data_.push_back(FORMAT_MOTOR_ANGLE_DELTA);
	appendFloat(angle_delta);
}

void ProtocolEncoder::appendMotorSnapPoint(int snap_point)
{
	data_.push_back(FORMAT_MOTOR_SNAP_POINT);
	appendInt(snap_point);
}

void ProtocolEncoder::appendButtonState(ButtonState button_state)
{
	data_.push_back(FORMAT_BUTTON_STATE);
	data_.push_back(button_state);
}

void ProtocolEncoder::appendButtonStateDelta(ButtonDeltaState button_delta_state)
{
	data_.push_back(FORMAT_BUTTON_STATE_DELTA);
	data_.push_back(button_delta_state);
}

void ProtocolEncoder::appendTouchCount(uint8_t touch_count)
{
	data_.push_back(FORMAT_TOUCH_COUNT);
	data_.push_back(touch_count);
}

void ProtocolEncoder::appendTouchCountDelta(uint8_t touch_count_delta)
{
	data_.push_back(FORMAT_TOUCH_COUNT_DELTA);
	data_.push_back(touch_count_delta);
}

void ProtocolEncoder::appendTouchPressure(int32_t touch_pressure)
{
	data_.push_back(FORMAT_TOUCH_PRESSURE);
	appendInt(touch_pressure);
}

void ProtocolEncoder::appendTouchCountAndTouches(uint8_t touch_count, const TouchFormat touch_format,
                                                 const TouchVector &touches, float motor_angle)
{
	data_.push_back(FORMAT_TOUCH_COUNT_AND_TOUCHES);
	data_.push_back(touch_count);
	data_.push_back(touch_format);

	switch (touch_format)
	{
		case POSITION_PRESSURE_CHANNELS:
			for (const Touch &touch: touches)
			{
				appendFloat(touch.getPosition());
				appendInt(touch.getPressure());
				data_.push_back(touch.getChannels());
			}
			break;
		case POSITION_PRESSURE:
			for (const Touch &touch: touches)
			{
				appendFloat(touch.getPosition());
				appendInt(touch.getChannels());
			}
			break;
		case RELATIVE_POSITION_PRESSURE_CHANNELS:
			for (const Touch &touch: touches)
			{
				appendFloat(touch.getRelativeCircularPosition(motor_angle));
				appendInt(touch.getPressure());
				data_.push_back(touch.getChannels());
			}
			break;
	}
}

void ProtocolEncoder::appendRequestMultiKnobState(const MultiKnobState::State &state)
{
	data_.push_back(REQUEST_ON_OFF);
	if (state == MultiKnobState::State::ON)
	{
		data_.push_back(COMMAND_ON);
	} else
	{
		data_.push_back(COMMAND_OFF);
	}
}

void ProtocolEncoder::appendRequestSenderConfig(const SenderConfig &config)
{
	data_.push_back(REQUEST_SEND_FORMAT);
	uint8_t count_pos = data_.size();
	data_.push_back(0); // Placeholder for count
	uint8_t count = 0;
	if (config.motor_angle)
	{
		data_.push_back(FORMAT_MOTOR_ANGLE);
		count++;
	}
	if (config.motor_angle_delta)
	{
		data_.push_back(FORMAT_MOTOR_ANGLE_DELTA);
		count++;
	}
	if (config.motor_snap_point)
	{
		data_.push_back(FORMAT_MOTOR_SNAP_POINT);
		count++;
	}
	if (config.button_state)
	{
		data_.push_back(FORMAT_BUTTON_STATE);
		count++;
	}
	if (config.button_state_delta)
	{
		data_.push_back(FORMAT_BUTTON_STATE_DELTA);
		count++;
	}
	if (config.touch_count)
	{
		data_.push_back(FORMAT_TOUCH_COUNT);
		count++;
	}
	if (config.touch_count_delta)
	{
		data_.push_back(FORMAT_TOUCH_COUNT_DELTA);
		count++;
	}
	if (config.touch_pressure)
	{
		data_.push_back(FORMAT_TOUCH_PRESSURE);
		count++;
	}
	if (config.touch_count_and_touches)
	{
		data_.push_back(FORMAT_TOUCH_COUNT_AND_TOUCHES);
		count++;
		data_.push_back(config.touch_format);
		count++;
	}
	data_[count_pos] = count;
}

void ProtocolEncoder::appendRequestTarget(float target)
{
	data_.push_back(REQUEST_TARGET);
	appendFloat(target);
}

void ProtocolEncoder::appendMotorConfig(const MotorConfig &config, const TouchCount &count)
{
	TouchConfig touch_config = config.getTouchConfig(touchCountToIndex(count));

	data_.push_back(REQUEST_MOTOR_CONFIGURATION);

	data_.push_back(count);
	data_.push_back(touch_config.strength);

	data_.push_back(touch_config.mode);
	switch (touch_config.mode)
	{
		case CONFIGURATION_HIT_TARGET:
			break;
		case CONFIGURATION_SNAPS_OPEN:
			appendInt(touch_config.snaps);
			break;
		case CONFIGURATION_SNAPS_WITH_BORDER:
			appendInt(touch_config.snaps);
			appendFloat(touch_config.lower_border);
			appendFloat(touch_config.upper_border);
			break;
	}
}

std::vector<uint8_t> ProtocolEncoder::createPacket(const SenderConfig &config, const MultiKnobData &raw_data)
{
	data_.clear();

	if (config.motor_angle) appendMotorAngle(raw_data.motor_angle);
	if (config.motor_angle_delta) appendMotorAngleDelta(raw_data.motor_angle_delta);
	if (config.motor_snap_point) appendMotorSnapPoint(raw_data.motor_snap_point);
	if (config.button_state) appendButtonState(raw_data.button_state);
	if (config.button_state_delta) appendButtonStateDelta(raw_data.button_state_delta);
	if (config.touch_count) appendTouchCount(raw_data.touch_count);
	if (config.touch_count_delta) appendTouchCountDelta(raw_data.touch_count_delta);
	if (config.touch_pressure) appendTouchPressure(raw_data.touch_pressure);
	if (config.touch_count_and_touches)
	{
		appendTouchCountAndTouches(raw_data.touch_count, config.touch_format, raw_data.touches, raw_data.motor_angle);
	}

	appendChecksum();
	return data_;
}

std::vector<uint8_t> ProtocolEncoder::createRequestOnOffPacket(const MultiKnobState::State &state)
{
	data_.clear();
	data_.push_back(FORMAT_INFORMATION);
	appendRequestMultiKnobState(state);
	appendChecksum();
	return data_;
}

std::vector<uint8_t> ProtocolEncoder::createRequestSenderFormatPacket(const SenderConfig &config)
{
	data_.clear();
	data_.push_back(FORMAT_INFORMATION);
	appendRequestSenderConfig(config);
	appendChecksum();
	return data_;
}

std::vector<uint8_t> ProtocolEncoder::createRequestTargetPacket(float target)
{
	data_.clear();
	data_.push_back(FORMAT_INFORMATION);
	appendRequestTarget(target);
	appendChecksum();
	return data_;
}

std::vector<uint8_t> ProtocolEncoder::createMotorConfigPacket(const MotorConfig &config, const TouchCount &touch_count)
{
	data_.clear();
	data_.push_back(FORMAT_INFORMATION);
	appendMotorConfig(config, touch_count);
	appendChecksum();
	return data_;
}
