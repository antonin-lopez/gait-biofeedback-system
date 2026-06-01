#include "M5Imu.h"
#include <M5Unified.h>
#include <cmath>

bool M5Imu::init() {
    return true;
}

void M5Imu::update() {
    M5.update();
    M5.Imu.getAccelData(&accelX_, &accelY_, &accelZ_);
}

float M5Imu::getAccelerationZ() {
    return accelZ_;
}

float M5Imu::getAccelerationMagnitude() {
    return std::sqrt(accelX_ * accelX_ + accelY_ * accelY_ + accelZ_ * accelZ_);
}
