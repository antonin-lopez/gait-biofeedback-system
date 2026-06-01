#include "AnkleApp.h"
#include "../../include/NetworkConfig.h"
#include "../../include/Protocol.h"
#include "../../lib/network/ProtocolCodec.h"
#include "../../include/AppConfig.h"
#include "../../include/Types.h"
#include <Arduino.h>

AnkleApp::AnkleApp(Imu& imu, NetworkManager& network)
    : imu_(imu), network_(network), detector_(IMPACT_DETECTION_THRESHOLD_G), seqNum_(0) {}

void AnkleApp::setup() {
    imu_.init();
    network_.init();
}

void AnkleApp::loop() {
    imu_.update();
    const float accelZ = imu_.getAccelerationZ();

    float outPeak = 0.0f;
    if (detector_.processSample(accelZ, outPeak)) {
        ImpactPayload payload;
        payload.timestampMs = millis();
        payload.peakDeceleration = outPeak;
        payload.seqNum = seqNum_++;

#ifdef IS_LEFT_ANKLE
        payload.footSide = static_cast<uint8_t>(FootSide::LEFT);
#else
        payload.footSide = static_cast<uint8_t>(FootSide::RIGHT);
#endif

        uint8_t wireBuffer[IMPACT_PAYLOAD_WIRE_SIZE];
        const size_t wireLength = serializeImpactPayload(payload, wireBuffer, sizeof(wireBuffer));
        if (wireLength == IMPACT_PAYLOAD_WIRE_SIZE) {
            network_.send(WRIST_HUB_MAC, wireBuffer, wireLength);
        }
    }
}
