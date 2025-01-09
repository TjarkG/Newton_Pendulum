//
// Created by Joel Neumann on 22.12.23.
//

#include "SideTouchTask.h"

SideTouchTask::SideTouchTask(int task_core, const SenderConfig &sender_config, int prescaler, const std::function<void(uint8_t)> &update_touch_count_for_motor_callback)
    : Task("SideTouch", 10000, 1, task_core), sender_config_(sender_config), update_touch_count_for_motor_callback_(update_touch_count_for_motor_callback) {
    touch_ring_sensor_.setup(
            {
                    {
                            .prescaler = prescaler,
                            .address = 0x48
                    },
                    {
                            .prescaler = prescaler,
                            .address = 0x49
                    },
            }, true);
    beginTask();
}


[[noreturn]] void SideTouchTask::run() {
    while (true){
        touch_ring_sensor_.readData();
        side_touch_data_.touches = touch_ring_sensor_.getTouches();
        side_touch_data_.count = side_touch_data_.touches.size();

        if(sender_config_.touch_pressure){
            updateTouchPressure();
        }
        if (sender_config_.touch_count_delta){
            updateTouchCountDelta();
        }

        update_touch_count_for_motor_callback_(side_touch_data_.count);

        xSemaphoreGive(done_semaphore_);
        vTaskSuspend(nullptr);
    }
}

void SideTouchTask::updateTouchCountDelta() {
    side_touch_data_.count_delta = side_touch_data_.touches.size() - last_touch_count_;
    last_touch_count_ = side_touch_data_.touches.size();
}

void SideTouchTask::updateTouchPressure() {
    int total_pressure = 0;
    for (const Touch &touch : side_touch_data_.touches) {
        total_pressure += touch.getPressure();
    }
    side_touch_data_.pressure = total_pressure;
}

TaskHandles SideTouchTask::getHandles() const {
    return TaskHandles{
            .task = getHandle(),
            .semaphore = done_semaphore_
    };
}

std::function<SideTouchData()> SideTouchTask::getDataCallback() const {
    return [this]{
        return side_touch_data_;
    };
}
