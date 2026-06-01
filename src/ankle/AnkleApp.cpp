#include "AnkleApp.h"
#include "../../include/Protocol.h"
#include "../../include/AppConfig.h"
#include "../../include/Types.h"

AnkleApp::AnkleApp(IImu* imu, INetworkManager* net)
    : _imu(imu), _net(net), _detector(IMPACT_DETECTION_THRESHOLD_G) {}

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
        payload.timestampMs = 0; // Peut être défini avec millis() sur ESP32
        payload.peakDeceleration = outPeak;

#ifdef IS_LEFT_ANKLE
        payload.footSide = (uint8_t)FootSide::LEFT;
#else
        payload.footSide = (uint8_t)FootSide::RIGHT;
#endif

        // Envoi au poignet (adresse de broadcast ou adresse connue)
        uint8_t wristMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        _net->send(wristMac, (uint8_t*)&payload, sizeof(payload));
    }
}
