#pragma once

// Interface capteur inertiel (accélération verticale).
class Imu {
public:
    virtual ~Imu() = default;
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual float getAccelerationZ() const = 0;
    virtual float getAccelerationMagnitude() const = 0;
};
