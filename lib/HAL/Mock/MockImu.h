#ifndef MOCKIMU_H
#define MOCKIMU_H

#include "../IImu.h"

class MockImu : public IImu {
public:
    float accelXMock = 0.0f;
    float accelYMock = 0.0f;
    float accelZMock = 0.0f;

    bool init() override;
    void update() override;
    float getAccelerationZ() override;
    float getAccelerationMagnitude() override;
};

#endif // MOCKIMU_H
