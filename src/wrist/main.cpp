#include <Arduino.h>
#include "../../lib/HAL/M5StickCPlus2/M5Board.h"
#include "../../lib/HAL/M5StickCPlus2/M5Feedback.h"
#include "../../lib/Network/EspNowManager.h"
#include "WristApp.h"
#include "../../include/Protocol.h"

M5Board realBoard;
M5Feedback realUi;
EspNowManager realNet;
WristApp app(&realBoard, &realUi, &realNet);

void networkCallback(const uint8_t* mac, const uint8_t* data, int len) {
    if (len == sizeof(ImpactPayload)) {
        ImpactPayload* packet = (ImpactPayload*)data;
        app.handleIncomingImpact(packet->peakDeceleration, packet->footSide);
    }
}

void setup() {
    realNet.registerReceiveCallback(networkCallback);
    app.setup();
}

void loop() {
    app.loop();
}
