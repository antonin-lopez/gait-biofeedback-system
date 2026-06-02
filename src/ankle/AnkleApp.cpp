#include "AnkleApp.h"
#include "AppConfig.h"
#include "ProtocolCodec.h"
#include <Preferences.h> // Pour la mémoire flash non volatile

AnkleApp::AnkleApp(Imu &imu, NetworkManager &network)
    : imu_(imu), network_(network), detector_(IMPACT_DETECTION_THRESHOLD_G), seqNum_(0) {}

void AnkleApp::setup()
{
    if (!imu_.init() || !network_.init())
    {
        // Mode panique / Secours au lieu du bootloop de redémarrage automatique
        while (true)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // Récupération de l'adresse MAC dynamique appairée
    Preferences prefs;
    prefs.begin("pairing", true);
    prefs.getBytes("wrist_mac", wristHubMac_, 6);
    prefs.end();

    xLastWakeTime_ = xTaskGetTickCount();
}

void AnkleApp::loop()
{
    imu_.update();
    const float accel = imu_.getAccelerationMagnitude();
    const uint32_t nowMs = pdTICKS_TO_MS(xTaskGetTickCount());

    // Envoi Heartbeat
    if ((nowMs - lastHeartbeatSentMs_) >= HEARTBEAT_SEND_INTERVAL_MS)
    {
        HeartbeatPayload heartbeat{
#ifdef IS_LEFT_ANKLE
            DeviceRole::ANKLE_LEFT,
#else
            DeviceRole::ANKLE_RIGHT,
#endif
            0};
        uint8_t hbBuf[HEARTBEAT_PAYLOAD_WIRE_SIZE];
        serializeHeartbeatPayload(heartbeat, hbBuf, sizeof(hbBuf));
        network_.send(wristHubMac_, hbBuf, sizeof(hbBuf));
        lastHeartbeatSentMs_ = nowMs;
    }

    // Traitement IMU moderne (std::optional)
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

    // Cadencement strict sans dérive
    vTaskDelayUntil(&xLastWakeTime_, pdMS_TO_TICKS(LOOP_PERIOD_MS));
}