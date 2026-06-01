#pragma once

#include "../Imu.h"

class MockImu : public Imu {
private:
    float accelX_ = 0.0f;
    float accelY_ = 0.0f;
    float accelZ_ = 0.0f;

public:
    MockImu() = default;

    bool init() override;
    void update() override;
    float getAccelerationZ() const override;
    float getAccelerationMagnitude() const override;

    void setSimulatedAcceleration(float x, float y, float z);
};
