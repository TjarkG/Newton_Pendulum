//
// Created by Joel Neumann on 20.06.24.
//

#include "SerialCommunicator.h"

SerialCommunicator &SerialCommunicator::getInstance()
{
	static SerialCommunicator instance;
	return instance;
}

void SerialCommunicator::setup(
	const std::function<void(const SenderConfig &)> &set_config_callback,
	const std::function<void(const RequestedData &)> &set_request_data_callback,
	const std::function<void(const MultiKnobState::State &)> &set_multi_knob_state_callback,
	const std::function<void(float)> &set_motor_target_callback,
	const std::function<MotorData()> &get_motor_target_callback,
	const std::function<void(MotorAction)> &perform_motor_action_callback,
	const std::function<void(TouchConfigUpdate &)> &update_motor_config_callback
)
{
	//Serial.begin(115200);

	get_motor_data_callback_ = get_motor_target_callback;
	decoder_.setCallbacks(
		set_config_callback,
		set_request_data_callback,
		set_multi_knob_state_callback,
		set_motor_target_callback,
		perform_motor_action_callback,
		update_motor_config_callback);

	my_packet_serial_.setStream(&Serial);
	my_packet_serial_.setPacketHandler([](const uint8_t *buffer, size_t size) {
		SerialCommunicator::getInstance().packetHandler(buffer, size);
	});
}

void SerialCommunicator::packetHandler(const uint8_t *buffer, size_t size)
{
	decoder_.decode(buffer, size);
}

void SerialCommunicator::read()
{
	my_packet_serial_.update();
}

void SerialCommunicator::send(const MultiKnobData &data, const SenderConfig &sender_config)
{
	std::vector<uint8_t> packet = encoder_.createPacket(sender_config, data);
	/*//Debugging
	for (unsigned char i : packet) {
	    Serial.print(i);
	    Serial.print(" ");
	}
	Serial.println();*/
	my_packet_serial_.send(packet.data(), packet.size());
}

void SerialCommunicator::sendRequestedData(const RequestedData &requested_data, const Configurations &configs)
{
	std::vector<uint8_t> packet;
	switch (requested_data.data)
	{
		case REQUEST_ON_OFF:
			packet = encoder_.createRequestOnOffPacket(configs.multi_knob_state.getState());
			break;
		case REQUEST_SEND_FORMAT:
			packet = encoder_.createRequestSenderFormatPacket(configs.sender_config);
			break;
		case REQUEST_TARGET:
			packet = encoder_.createRequestTargetPacket(get_motor_data_callback_().target);
			break;
		case REQUEST_MOTOR_CONFIGURATION:
			packet = encoder_.createMotorConfigPacket(configs.motor_config, requested_data.touch_count);
			break;
	}

	my_packet_serial_.send(packet.data(), packet.size());
}
