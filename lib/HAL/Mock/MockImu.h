#pragma once

#include "../Imu.h"

class MockImu : public Imu {
private:
    float accelX_;
    float accelY_;
    float accelZ_;

public:
    MockImu();

    bool init() override;
    void update() override;
    float getAccelerationZ() const override;
    float getAccelerationMagnitude() const override;

    void setSimulatedAcceleration(float x, float y, float z);
};
