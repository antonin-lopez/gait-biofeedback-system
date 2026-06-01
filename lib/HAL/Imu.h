#pragma once

// Interface capteur inertiel (accélération verticale).
class Imu {
public:
    virtual ~Imu() = default;
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual float getAccelerationZ() = 0;
    virtual float getAccelerationMagnitude() = 0;
};
