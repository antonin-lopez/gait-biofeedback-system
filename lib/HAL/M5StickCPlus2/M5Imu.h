#ifndef M5IMU_H
#define M5IMU_H

#include "../IImu.h"

class M5Imu : public IImu {
private:
    float _accelX, _accelY, _accelZ;

public:
    bool init() override;
    void update() override;
    float getAccelerationZ() override;
    float getAccelerationMagnitude() override;
};

#endif // M5IMU_H
