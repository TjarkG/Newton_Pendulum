//
// Created by Joel Neumann on 10.07.24.
//

#ifndef DREA_V2_COMMUNICATION_TASK_H
#define DREA_V2_COMMUNICATION_TASK_H

#include "Communication/Serial/SerialCommunicator.h"
#include "Configuration/Configurations.h"
#include "task.h"
#include <Arduino.h>

enum class CommunicationActionType {
    SET_SENDER_CONFIGURATION,
    REQUEST_DATA
};

struct CommunicationAction {
    CommunicationActionType type;
    union ActionData {
        SenderConfig sender_config;
        RequestedData requested_data;
    };
    ActionData data;
};

//TODO: Feature: Add Rate of sending Packages
class CommunicationTask : public Task<CommunicationTask> {
    friend class Task<CommunicationTask>;

private:
    SerialCommunicator &communicator_ = SerialCommunicator::getInstance();
    QueueHandle_t communication_action_queue_;

    Configurations &configurations_;

    const TaskHandles button_handles_;
    const TaskHandles side_touch_handles_;

    const std::function<void()> update_motor_angle_delta_callback_;
    const std::function<ButtonData()> get_button_data_callback_;
    const std::function<SideTouchData()> get_side_touch_data_callback_;
    const std::function<MotorData()> get_motor_data_callback_;

    MultiKnobData collectData() const;
    std::function<void(const SenderConfig&)> getSetConfigCallback() const;
    std::function<void(const RequestedData &)> getRequestDataCallback() const;
    std::function<void(const MultiKnobState::State &)> getSetMultiKnobStateCallback() const;
    void handleActions();

protected:
    [[noreturn]] void run();

public:
    CommunicationTask(
            int task_core,
            TaskHandles button_handles,
            TaskHandles side_touch_handles,
            Configurations &,
            const std::function<void()> &update_motor_angle_delta_callback,
            const std::function<ButtonData()> &get_button_data_callback,
            const std::function<SideTouchData()> &get_side_touch_data_callback,
            const std::function<MotorData()> &get_motor_data_callback,
            const std::function<void(float)> &set_motor_target_callback,
            const std::function<void(MotorAction)> &perform_motor_action_callback,
            const std::function<void(TouchConfigUpdate&)> &update_motor_config_callback);
};


#endif//DREA_V2_COMMUNICATION_TASK_H
