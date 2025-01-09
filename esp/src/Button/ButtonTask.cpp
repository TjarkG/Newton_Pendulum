//
// Created by Joel Neumann on 21.12.23.
//

#include "ButtonTask.h"

ButtonTask::ButtonTask(int task_core, SenderConfig &config)
    : Task("Button", 10000, 1, task_core), sender_config_(config) {
    pinMode(BUTTON_PIN, INPUT);
    beginTask();
}

[[noreturn]] void ButtonTask::run() {
    while (true) {
        int button_reading = digitalRead(BUTTON_PIN);
        if (button_reading == 0) {
            button_data_.state = NOT_PRESSED;
        } else {
            button_data_.state = PRESSED;
        }
        if(sender_config_.button_state_delta){
            updateDelta();
        }

        xSemaphoreGive(done_semaphore_);
        vTaskSuspend(nullptr);
    }
}

void ButtonTask::updateDelta() {
    if (button_data_.state == last_delta_state_) {
        button_data_.delta = NO_CHANGE;
    } else if (button_data_.state == PRESSED) {
        button_data_.delta = NOT_PRESSED_TO_PRESSED;
    } else {
        button_data_.delta = PRESSED_TO_NOT_PRESSED;
    }
    last_delta_state_ = button_data_.state;
}

std::function<ButtonData()> ButtonTask::getDataCallback() const {
    return [this]{
        return button_data_;
    };
}

TaskHandles ButtonTask::getHandles() const {
    return TaskHandles{
            .task = getHandle(),
            .semaphore = done_semaphore_
    };
}
