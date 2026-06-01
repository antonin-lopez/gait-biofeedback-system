#ifndef ANKLEAPP_H
#define ANKLEAPP_H

#include "../../lib/HAL/IImu.h"
#include "../../lib/Network/INetworkManager.h"
#include "../../lib/Algorithms/ImpactDetector.h"
#include "../../include/Types.h"
#include <cstdint>

class AnkleApp {
private:
    IImu* _imu;
    INetworkManager* _net;
    ImpactDetector _detector;
    uint32_t _seqNum;

public:
    AnkleApp(IImu* imu, INetworkManager* net);
    void setup();
    void loop();
};

#endif // ANKLEAPP_H

