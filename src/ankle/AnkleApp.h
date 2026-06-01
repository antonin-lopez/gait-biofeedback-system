#pragma once

#include "../../lib/HAL/Imu.h"
#include "../../lib/network/NetworkManager.h"
#include "../../lib/algorithms/ImpactDetector.h"
#include "../../include/Types.h"
#include <cstdint>

// Application cheville (émetteur d'impacts vers le poignet).
class AnkleApp {
private:
    Imu& imu_;
    NetworkManager& network_;
    ImpactDetector detector_;
    uint32_t seqNum_;

public:
    AnkleApp(Imu& imu, NetworkManager& network);
    void setup();
    void loop();
};
