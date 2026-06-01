#include "MockImu.h"
#include <cmath>

bool MockImu::init() {
    return true;
}

void MockImu::update() {}

float MockImu::getAccelerationZ() const {
    return accelZ_;
}

float MockImu::getAccelerationMagnitude() const {
    return std::sqrt(accelX_ * accelX_ + accelY_ * accelY_ + accelZ_ * accelZ_);
}

void MockImu::setSimulatedAcceleration(float x, float y, float z) {
    accelX_ = x;
    accelY_ = y;
    accelZ_ = z;
}
