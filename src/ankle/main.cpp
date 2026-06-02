#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

ImpactDetector detector(2.0f); // Seuil d'impact à 2.0G
uint32_t lastHeartbeatMs = 0;
uint32_t seqNum = 0;

void setup()
{
    Hardware::init();
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
        esp_restart();

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, WRIST_MAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void loop()
{
    Hardware::update();
    uint32_t now = millis();

    // Envoi du Heartbeat toutes les 500ms
    if (now - lastHeartbeatMs >= 500)
    {
        HeartbeatMessage hb;
#ifdef IS_LEFT_ANKLE
        hb.role = DeviceRole::ANKLE_LEFT;
#else
        hb.role = DeviceRole::ANKLE_RIGHT;
#endif
        hb.batteryLevel = M5.Power.getBatteryLevel();
        esp_now_send(WRIST_MAC, (uint8_t *)&hb, sizeof(hb));
        lastHeartbeatMs = now;
    }

    // Traitement de l'accélération (Fréquence de boucle ~100Hz grâce au delay)
    float accel = Hardware::getAccelMagnitude();
    auto peak = detector.processSample(accel, now);
    if (peak.has_value())
    {
        ImpactMessage msg;
        msg.peakForce = peak.value();
        msg.seqNum = seqNum++;
#ifdef IS_LEFT_ANKLE
        msg.isLeft = 1;
#else
        msg.isLeft = 0;
#endif
        esp_now_send(WRIST_MAC, (uint8_t *)&msg, sizeof(msg));
    }
    delay(10);
}