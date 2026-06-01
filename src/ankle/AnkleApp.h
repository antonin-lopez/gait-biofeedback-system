#ifndef ANKLEAPP_H
#define ANKLEAPP_H

#include "../../lib/HAL/IImu.h"
#include "../../lib/Network/INetworkManager.h"
#include "../../lib/Algorithms/ImpactDetector.h"
#include "../../include/Types.h"

class AnkleApp {
private:
    IImu* _imu;
    INetworkManager* _net;
    ImpactDetector _detector;

public:
    AnkleApp(IImu* imu, INetworkManager* net);
    void setup();
    void loop();
};

#endif // ANKLEAPP_H
