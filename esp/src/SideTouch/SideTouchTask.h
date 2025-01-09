//
// Created by Joel Neumann on 22.12.23.
//

#ifndef DREA_V2_TOUCH_HANDLER_H
#define DREA_V2_TOUCH_HANDLER_H

#include "Configuration/SenderConfig.h"
#include "MultiKnobData.h"
#include "task.h"
#include <TouchSensor.h>
#include <Arduino.h>

class SideTouchTask : public Task<SideTouchTask> {
    friend class Task<SideTouchTask>;

private:
    const SemaphoreHandle_t done_semaphore_ = xSemaphoreCreateBinary();

    const SenderConfig &sender_config_;

    TouchSensor touch_ring_sensor_;
    SideTouchData side_touch_data_{
            .count = 0,
            .count_delta = 0,
            .pressure = 0,
            .touches = {}};
    uint8_t last_touch_count_ = 0;

    const std::function<void(uint8_t)> update_touch_count_for_motor_callback_;

    void updateTouchCountDelta();
    void updateTouchPressure();

protected:
    [[noreturn]] void run();

public:
    SideTouchTask(int task_core, const SenderConfig &sender_config, int prescaler, const std::function<void(uint8_t)> &update_touch_count_for_motor_callback);

    std::function<SideTouchData()> getDataCallback() const;
    TaskHandles getHandles() const;
};

#endif// DREA_V2_TOUCH_HANDLER_H
