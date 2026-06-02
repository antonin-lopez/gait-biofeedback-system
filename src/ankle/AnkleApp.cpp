#include "AnkleApp.h"
#include "NetworkConfig.h"
#include "Protocol.h"
#include "ProtocolCodec.h"
#include "AppConfig.h"
#include "Types.h"
#include <Arduino.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

AnkleApp::AnkleApp(Imu& imu, NetworkManager& network)
    : imu_(imu), network_(network), detector_(IMPACT_DETECTION_THRESHOLD_G), seqNum_(0) {}

void AnkleApp::setup() {
    if (!imu_.init() || !network_.init()) {
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }
}

void AnkleApp::loop() {
    imu_.update();
    const float accel = imu_.getAccelerationMagnitude();
    const uint32_t nowMs = pdTICKS_TO_MS(xTaskGetTickCount());

    if ((nowMs - lastHeartbeatSentMs_) >= HEARTBEAT_SEND_INTERVAL_MS) {
        HeartbeatPayload heartbeat{};
#ifdef IS_LEFT_ANKLE
        heartbeat.deviceRole = DeviceRole::ANKLE_LEFT;
#else
        heartbeat.deviceRole = DeviceRole::ANKLE_RIGHT;
#endif
        heartbeat.batteryLevel = 0;

        uint8_t heartbeatBuffer[HEARTBEAT_PAYLOAD_WIRE_SIZE];
        const size_t heartbeatLen =
            serializeHeartbeatPayload(heartbeat, heartbeatBuffer, sizeof(heartbeatBuffer));
        if (heartbeatLen == HEARTBEAT_PAYLOAD_WIRE_SIZE) {
            network_.send(WRIST_HUB_MAC, heartbeatBuffer, heartbeatLen);
            lastHeartbeatSentMs_ = nowMs;
        }
    }

    float outPeak = 0.0f;
    if (detector_.processSample(accel, nowMs, outPeak)) {
        ImpactPayload payload;
        payload.peakDeceleration = outPeak;
        payload.seqNum = seqNum_++;

#ifdef IS_LEFT_ANKLE
        payload.footSide = FootSide::LEFT;
#else
        payload.footSide = FootSide::RIGHT;
#endif

        uint8_t wireBuffer[IMPACT_PAYLOAD_WIRE_SIZE];
        const size_t wireLength = serializeImpactPayload(payload, wireBuffer, sizeof(wireBuffer));
        if (wireLength == IMPACT_PAYLOAD_WIRE_SIZE) {
            network_.send(WRIST_HUB_MAC, wireBuffer, wireLength);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
}
