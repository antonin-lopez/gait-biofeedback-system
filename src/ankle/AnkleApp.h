#pragma once

#include "Imu.h"
#include "NetworkManager.h"
#include "ImpactDetector.h"
#include "Types.h"
#include <cstdint>

class AnkleApp {
private:
    Imu& imu_;
    NetworkManager& network_;
    ImpactDetector detector_;
    uint32_t seqNum_;
    uint32_t lastHeartbeatSentMs_ = 0;

public:
    AnkleApp(Imu& imu, NetworkManager& network);
    void setup();
    void loop();
};
