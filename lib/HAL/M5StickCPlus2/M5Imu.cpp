#include "M5Imu.h"
#include <M5Unified.h>
#include <cmath>

bool M5Imu::init() {
    return true;
}

void M5Imu::update() {
    M5.update();
    M5.Imu.getAccelData(&_accelX, &_accelY, &_accelZ);
}

float M5Imu::getAccelerationZ() {
    return _accelZ;
}

float M5Imu::getAccelerationMagnitude() {
    return std::sqrt(_accelX * _accelX + _accelY * _accelY + _accelZ * _accelZ);
}

