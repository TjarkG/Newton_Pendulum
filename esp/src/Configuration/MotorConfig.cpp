//
// Created by Joel Neumann on 11.07.24.
//

#include "MotorConfig.h"


MotorConfig::MotorConfig() {
    for (auto &config: touch_configs_) {
        config = {50, CONFIGURATION_SNAPS_OPEN, 44, 0.0f, 0.0f};
    }
}

void MotorConfig::setAllTouchConfigs(TouchConfig config) {
    for (size_t i = 0; i < touch_configs_.size(); ++i) {
        setTouchConfig(i, config);
    }
}


void MotorConfig::setTouchConfig(size_t index, TouchConfig config) {
    if (index < touch_configs_.size()) {
        if (config.lower_border > config.upper_border) {
            std::swap(config.lower_border, config.upper_border);
        }
        touch_configs_[index] = config;
    }
}


TouchConfig MotorConfig::getTouchConfig(size_t index) const {
    if (index < touch_configs_.size()) {
        return touch_configs_[index];
    } else {
        return touch_configs_.back();
    }
}