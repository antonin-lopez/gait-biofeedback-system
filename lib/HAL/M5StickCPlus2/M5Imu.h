#pragma once

#include "../Imu.h"

class M5Imu : public Imu {
private:
    float accelX_;
    float accelY_;
    float accelZ_;

public:
    bool init() override;
    void update() override;
    float getAccelerationZ() const override;
    float getAccelerationMagnitude() const override;
};
