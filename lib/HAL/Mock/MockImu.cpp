#include "MockImu.h"
#include <cmath>

bool MockImu::init() {
    return true;
}

void MockImu::update() {
}

float MockImu::getAccelerationZ() {
    return accelZMock;
}

float MockImu::getAccelerationMagnitude() {
    return std::sqrt(accelXMock * accelXMock + accelYMock * accelYMock + accelZMock * accelZMock);
}
