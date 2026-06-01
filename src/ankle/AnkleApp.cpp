#include "AnkleApp.h"
#include "../../include/Protocol.h"
#include "../../include/AppConfig.h"
#include "../../include/Types.h"
#include <Arduino.h>

AnkleApp::AnkleApp(IImu* imu, INetworkManager* net)
    : _imu(imu), _net(net), _detector(IMPACT_DETECTION_THRESHOLD_G), _seqNum(0) {}

void AnkleApp::setup() {
    if (_imu) _imu->init();
    if (_net) _net->init();
}

void AnkleApp::loop() {
    if (!_imu || !_net) return;

    _imu->update();
    float accelZ = _imu->getAccelerationZ();

    float outPeak = 0.0f;
    if (_detector.processSample(accelZ, outPeak)) {
        ImpactPayload payload;
        payload.timestampMs = millis();
        payload.peakDeceleration = outPeak;
        payload.seqNum = _seqNum++;

#ifdef IS_LEFT_ANKLE
        payload.footSide = (uint8_t)FootSide::LEFT;
#else
        payload.footSide = (uint8_t)FootSide::RIGHT;
#endif

        uint8_t wristMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        _net->send(wristMac, (uint8_t*)&payload, sizeof(payload));
    }
}

