//
// Created by Joel Neumann on 02.07.24.
//

#include "ProtocolDecoder.h"
#include "crc16.h"

float bytesToFloat(const uint8_t* data) {
    float value;
    memcpy(&value, data, sizeof(float));
    return value;
}

int bytesToInt(const uint8_t* data) {
    int value;
    memcpy(&value, data, sizeof(int));
    return value;
}

void ProtocolDecoder::handleCommands(const uint8_t *buffer, size_t length) {
    size_t index = 0;
    while (index < length) {
        uint8_t command_length = buffer[index++];
        if (index + command_length > length) {
            break;
        }

        uint8_t command = buffer[index++];
        const uint8_t *command_data = &buffer[index];
        index += command_length - 1;

        switch (command) {
            case COMMAND_ON_OFF:
                handleCommandOnOff(command_data, command_length - 1);
                break;
            case COMMAND_SENDER_FORMAT:
                handleCommandSendFormat(command_data, command_length - 1);
                break;
            case COMMAND_SET_TARGET:
                handleCommandSetTarget(command_data, command_length - 1);
                break;
            case COMMAND_MOTOR_CONFIGURATION:
                handleCommandMotorConfig(command_data, command_length - 1);
                break;
            case COMMAND_MOTOR_ACTION:
                handleCommandMotorAction(command_data, command_length - 1);
                break;
            case COMMAND_REQUEST_DATA:
                handleRequestData(command_data, command_length - 1);
                break;
            default:
                break;
        }
    }
}

void ProtocolDecoder::handleCommandOnOff(const uint8_t *data, size_t length) {
    if (length != 1) {
        return;
    }
    bool state = (OnOffCommand) data[0] == COMMAND_ON;
    if (state) {
        set_multi_knob_callback_(MultiKnobState::State::ON);
    } else {
        set_multi_knob_callback_(MultiKnobState::State::OFF);
    }
}

void ProtocolDecoder::handleCommandSendFormat(const uint8_t *data, size_t length) {
    if (update_sender_config_callback_) {
        SenderConfig new_config;
        for (int i = 0; i < length; ++i) {
            switch (data[i]) {
                case FORMAT_MOTOR_ANGLE:
                    new_config.motor_angle = true;
                    break;
                case FORMAT_MOTOR_ANGLE_DELTA:
                    new_config.motor_angle_delta = true;
                    break;
                case FORMAT_MOTOR_SNAP_POINT:
                    new_config.motor_snap_point = true;
                    break;
                case FORMAT_BUTTON_STATE:
                    new_config.button_state = true;
                    break;
                case FORMAT_BUTTON_STATE_DELTA:
                    new_config.button_state_delta = true;
                    break;
                case FORMAT_TOUCH_COUNT:
                    new_config.touch_count = true;
                    break;
                case FORMAT_TOUCH_COUNT_DELTA:
                    new_config.touch_count_delta = true;
                    break;
                case FORMAT_TOUCH_PRESSURE:
                    new_config.touch_pressure = true;
                    break;
                case FORMAT_TOUCH_COUNT_AND_TOUCHES:
                    new_config.touch_count_and_touches = true;
                    switch (data[++i]) {
                        case POSITION_PRESSURE_CHANNELS:
                            new_config.touch_format = POSITION_PRESSURE_CHANNELS;
                            break;
                        case POSITION_PRESSURE:
                            new_config.touch_format = POSITION_PRESSURE;
                            break;
                        case RELATIVE_POSITION_PRESSURE_CHANNELS:
                            new_config.touch_format = RELATIVE_POSITION_PRESSURE_CHANNELS;
                            break;
                        default:
                            return;
                    }
                    break;
                default:
                    return;
            }
        }
        update_sender_config_callback_(new_config);
    }
}

void ProtocolDecoder::handleCommandSetTarget(const uint8_t *data, size_t length) {
    if (!set_motor_target_) {
        return;
    }
    if (length != 4) {
        return;
    }

    set_motor_target_(bytesToFloat(data));
}

void ProtocolDecoder::handleCommandMotorConfig(const uint8_t *data, size_t length) {
    if (!update_motor_config_callback_) {
        return;
    }
    if (length < 3) {
        return;
    }

    TouchConfigUpdate new_config{
            .count = (TouchCount) data[0],
            .config = {
                    .strength = data[1],
                    .mode = (MotorConfiguration) data[2]
            }
    };

    switch (new_config.config.mode) {
        case CONFIGURATION_HIT_TARGET:
            break;
        case CONFIGURATION_SNAPS_OPEN:
            if (length < 7) {
                return;
            }
            new_config.config.snaps = bytesToInt(&data[3]);
            break;
        case CONFIGURATION_SNAPS_WITH_BORDER:
            if (length < 15) {
                return;
            }
            new_config.config.snaps = bytesToInt(&data[3]);
            new_config.config.lower_border = bytesToFloat(&data[7]);
            new_config.config.upper_border = bytesToFloat(&data[11]);
            break;
    }

    update_motor_config_callback_(new_config);

}

void ProtocolDecoder::handleCommandMotorAction(const uint8_t *data, size_t length) {
    if (!perform_motor_action_callback_) {
        return;
    }
    if (length != 1) {
        return;
    }

    perform_motor_action_callback_((MotorAction) data[0]);
}

void ProtocolDecoder::handleRequestData(const uint8_t *data, size_t length) {
    if (!request_data_callback_) {
        return;
    }
    if (length < 1) {
        return;
    }

    RequestedData requested_data;
    switch (data[0]) {
        case REQUEST_ON_OFF:
        case REQUEST_SEND_FORMAT:
        case REQUEST_TARGET:
            requested_data.data = static_cast<RequestData>(data[0]);
            break;
        case REQUEST_MOTOR_CONFIGURATION:
            requested_data.data = REQUEST_MOTOR_CONFIGURATION;
            if (length != 2) {
                return;
            }
            if (data[1] >= ALL && data[1] <= GREATER_THAN_FIVE) {
                requested_data.touch_count = static_cast<TouchCount>(data[1]);
            } else {
                return;
            }
            break;
        default:
            return;
    }

    request_data_callback_(requested_data);
}

bool ProtocolDecoder::hasMinPacketSize(size_t length) {
    if (length < 3) {
        return false;
    }
    return true;
}

bool ProtocolDecoder::hasCorrectChecksum(const uint8_t *buffer, size_t length) {
    uint16_t received_crc = buffer[length - 2] | (buffer[length - 1] << 8);
    uint16_t calculated_crc = crc16(buffer, length - 2);

    if (received_crc != calculated_crc) {
        return false;
    }
    return true;
}

void ProtocolDecoder::decode(const uint8_t *buffer, size_t length) {
    if (hasMinPacketSize(length) && hasCorrectChecksum(buffer, length)) {
        handleCommands(buffer, length - 2);
    }
}

void ProtocolDecoder::setCallbacks(
        const std::function<void(const SenderConfig &)> &update_sender_config_callback,
        const std::function<void(const RequestedData &)> &request_data_callback,
        const std::function<void(const MultiKnobState::State &)> &set_multi_knob_callback,
        const std::function<void(float)> &set_motor_target,
        const std::function<void(MotorAction)> &perform_motor_action_callback,
        const std::function<void(TouchConfigUpdate&)> &update_motor_config_callback) {

    update_sender_config_callback_ = update_sender_config_callback;
    request_data_callback_ = request_data_callback;
    set_multi_knob_callback_ = set_multi_knob_callback;
    set_motor_target_ = set_motor_target;
    perform_motor_action_callback_ = perform_motor_action_callback;
    update_motor_config_callback_ = update_motor_config_callback;

}
