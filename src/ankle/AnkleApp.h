#pragma once
#include "Imu.h"
#include "NetworkManager.h"
#include "ImpactDetector.h"
#include <freertos/FreeRTOS.h>

class AnkleApp
{
private:
    Imu &imu_;
    NetworkManager &network_;
    ImpactDetector detector_;
    uint32_t seqNum_;
    uint32_t lastHeartbeatSentMs_ = 0;
    TickType_t xLastWakeTime_;
    uint8_t wristHubMac_[6];

public:
    AnkleApp(Imu &imu, NetworkManager &network);
    void setup();
    void loop();
};