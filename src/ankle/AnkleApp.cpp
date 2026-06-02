#include "AnkleApp.h"
#include "AppConfig.h"
#include "ProtocolCodec.h"
#include "NetworkConfig.h" // Inclus pour la MAC de repli
#include <Preferences.h>

AnkleApp::AnkleApp(Board &board, Imu &imu, NetworkManager &network)
    : board_(board), imu_(imu), network_(network), detector_(IMPACT_DETECTION_THRESHOLD_G), seqNum_(0) {}

void AnkleApp::setup()
{
    // Si l'initialisation échoue, on évite le blocage infini et silencieux
    if (!imu_.init() || !network_.init())
    {
        for (uint8_t i = 0; i < FAULT_BLINK_COUNT; ++i)
        {
            // Signalement visuel d'erreur sur le boîtier cheville
            board_.update();
            // Note: Optionnel selon votre matériel, vous pouvez faire clignoter une LED dédiée ici
            vTaskDelay(pdMS_TO_TICKS(FAULT_BLINK_DELAY_MS));
        }
        esp_restart(); // Redémarrage automatique du nœud défaillant
    }

    Preferences prefs;
    prefs.begin("pairing", true);
    size_t readBytes = prefs.getBytes("wrist_mac", wristHubMac_, 6);
    prefs.end();

    if (readBytes != 6)
    {
        memcpy(wristHubMac_, WRIST_HUB_MAC, 6);
    }

    xLastWakeTime_ = xTaskGetTickCount();
}

void AnkleApp::loop()
{
    imu_.update();
    board_.update();
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
            board_.getBatteryLevel()};
        uint8_t hbBuf[HEARTBEAT_PAYLOAD_WIRE_SIZE];
        serializeHeartbeatPayload(heartbeat, hbBuf, sizeof(hbBuf));
        network_.send(wristHubMac_, hbBuf, sizeof(hbBuf));

        // CORRIGÉ : Syntaxe rectifiée avec l'underscore variable membre correct (_)
        lastHeartbeatSentMs_ = nowMs;
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