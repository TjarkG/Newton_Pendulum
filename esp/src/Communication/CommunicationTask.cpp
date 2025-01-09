//
// Created by Joel Neumann on 10.07.24.
//

#include "CommunicationTask.h"

CommunicationTask::CommunicationTask(
        int task_core,
        const TaskHandles button_handles,
        const TaskHandles side_touch_handles,
        Configurations &configurations,
        const std::function<void()> &update_motor_angle_delta_callback,
        const std::function<ButtonData()> &get_button_data_callback,
        const std::function<SideTouchData()> &get_side_touch_data_callback,
        const std::function<MotorData()> &get_motor_data_callback,
        const std::function<void(float)> &set_motor_target_callback,
        const std::function<void(MotorAction )> &perform_motor_action_callback,
        const std::function<void(TouchConfigUpdate &)> &update_motor_config_callback)
    : Task("Communication", 10000, 1, task_core),
      button_handles_(button_handles),
      side_touch_handles_(side_touch_handles),
      configurations_(configurations),
      update_motor_angle_delta_callback_(update_motor_angle_delta_callback),
      get_button_data_callback_(get_button_data_callback),
      get_side_touch_data_callback_(get_side_touch_data_callback),
      get_motor_data_callback_(get_motor_data_callback)

{
    communicator_.setup(
            getSetConfigCallback(),
            getRequestDataCallback(),
            getSetMultiKnobStateCallback(),
            set_motor_target_callback,
            get_motor_data_callback,
            perform_motor_action_callback,
            update_motor_config_callback);

    communication_action_queue_ = xQueueCreate(10, sizeof(CommunicationAction));

    beginTask();
}

[[noreturn]] void CommunicationTask::run() {
    while (true) {
        xSemaphoreTake(button_handles_.semaphore, portMAX_DELAY);
        xSemaphoreTake(side_touch_handles_.semaphore, portMAX_DELAY);

        //Has to be done from the same core for syncing.
        update_motor_angle_delta_callback_();

        MultiKnobData data = collectData();

        vTaskResume(button_handles_.task);
        vTaskResume(side_touch_handles_.task);

        ///Handle Actions -> Write -> Read
        handleActions();
        if (configurations_.multi_knob_state.getState() == MultiKnobState::ON) {
            communicator_.send(data, configurations_.sender_config);
        }
        communicator_.read();
    }
}

MultiKnobData CommunicationTask::collectData() const {
    ButtonData button_data = get_button_data_callback_();
    SideTouchData side_touch_data = get_side_touch_data_callback_();
    MotorData motor_data = get_motor_data_callback_();

    return {
            .motor_angle = motor_data.angle,
            .motor_angle_delta = motor_data.angle_delta,
            .motor_snap_point = motor_data.current_snap_point,
            .button_state = button_data.state,
            .button_state_delta = button_data.delta,
            .touch_count = side_touch_data.count,
            .touch_count_delta = side_touch_data.count_delta,
            .touch_pressure = side_touch_data.pressure,
            .touches = side_touch_data.touches};
}


std::function<void(const SenderConfig &)> CommunicationTask::getSetConfigCallback() const {
    return [this](const SenderConfig &new_sender_config) {
        CommunicationAction action = {
                .type = CommunicationActionType::SET_SENDER_CONFIGURATION,
                .data = {
                        .sender_config = new_sender_config,
                }};
        xQueueSend(communication_action_queue_, &action, portMAX_DELAY);
    };
}

std::function<void(const RequestedData &)> CommunicationTask::getRequestDataCallback() const {
    return [this](const RequestedData &requested_data) {
        CommunicationAction action = {
                .type = CommunicationActionType::REQUEST_DATA,
                .data = {
                        .requested_data = requested_data,
                }};
        xQueueSend(communication_action_queue_, &action, portMAX_DELAY);
    };
}

std::function<void(const MultiKnobState::State &)> CommunicationTask::getSetMultiKnobStateCallback() const {
    return [this](const MultiKnobState::State &new_state) {
        configurations_.multi_knob_state.setState(new_state);
    };
}

void CommunicationTask::handleActions() {
    CommunicationAction action{};
    while (xQueueReceive(communication_action_queue_, &action, 0) == pdTRUE) {
        switch (action.type) {
            case CommunicationActionType::SET_SENDER_CONFIGURATION:
                configurations_.sender_config = action.data.sender_config;
                break;
            case CommunicationActionType::REQUEST_DATA:
                communicator_.sendRequestedData(action.data.requested_data, configurations_);
                break;
        }
    }
}