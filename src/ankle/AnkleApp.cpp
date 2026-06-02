#include "AnkleApp.h"
#include "AppConfig.h"
#include "ProtocolCodec.h"
#include <Preferences.h>

AnkleApp::AnkleApp(Board &board, Imu &imu, NetworkManager &network)
    : board_(board), imu_(imu), network_(network), detector_(IMPACT_DETECTION_THRESHOLD_G), seqNum_(0) {}

void AnkleApp::setup()
{
    if (!imu_.init() || !network_.init())
    {
        while (true)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    Preferences prefs;
    prefs.begin("pairing", true);
    prefs.getBytes("wrist_mac", wristHubMac_, 6);
    prefs.end();

    xLastWakeTime_ = xTaskGetTickCount();
}

void AnkleApp::loop()
{
    imu_.update();
    board_.update(); // Indispensable pour rafraîchir l'état du matériel
    const float accel = imu_.getAccelerationMagnitude();
    const uint32_t nowMs = pdTICKS_TO_MS(xTaskGetTickCount());

    if ((nowMs - lastHeartbeatSentMs_) >= HEARTBEAT_SEND_INTERVAL_MS)
    {
        HeartbeatPayload heartbeat{
#ifdef IS_LEFT_ANKLE
            DeviceRole::ANKLE_LEFT,
#else
            DeviceRole::ANKLE_RIGHT,
#endif
            board_.getBatteryLevel() // CORRIGÉ : Transmet la vraie valeur lue sur le matériel !
        };
        uint8_t hbBuf[HEARTBEAT_PAYLOAD_WIRE_SIZE];
        serializeHeartbeatPayload(heartbeat, hbBuf, sizeof(hbBuf));
        network_.send(wristHubMac_, hbBuf, sizeof(hbBuf));
        lastHeartbeatSentMs = nowMs;
    }

    auto peakOpt = detector_.processSample(accel, nowMs);
    if (peakOpt.has_value())
    {
        ImpactPayload payload;
        payload.peakDeceleration = peakOpt.value();
        payload.seqNum = seqNum_++;
        payload.footSide =
#ifdef IS_LEFT_ANKLE
            FootSide::LEFT;
#else
            FootSide::RIGHT;
#endif

        uint8_t wireBuffer[IMPACT_PAYLOAD_WIRE_SIZE];
        serializeImpactPayload(payload, wireBuffer, sizeof(wireBuffer));
        network_.send(wristHubMac_, wireBuffer, sizeof(wireBuffer));
    }

    vTaskDelayUntil(&xLastWakeTime_, pdMS_TO_TICKS(LOOP_PERIOD_MS));
}