#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <M5Unified.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

ImpactDetector detector(DEFAULT_DETECTION_THRESHOLD); // Configuration locale du capteur : Seuil d'impact défini dans Protocol.h
uint32_t lastHeartbeatMs = 0;                         // Chronomètre pour envoyer le signal de vie toutes les 500ms
uint32_t seqNum = 0;                                  // Numéro de séquence incrémental pour identifier chaque message d'impact unique

void setup()
{
    Hardware::init(); // Initialisation des composants matériels

    M5.Lcd.fillScreen(0x000000);
    M5.Lcd.setTextColor(0xFFFFFF);
    M5.Lcd.setTextSize(5);

    int w = M5.Lcd.width();
    int h = M5.Lcd.height();
    const char *sideText = "";

    // CONFIGURATION DU TEXTE DE L'ÉCRAN AU FLASHAGE
    // La macro ANKLE_SIDE est configurée via le fichier platformio.ini pour chaque module
#if ANKLE_SIDE == 0
    sideText = "GAUCHE"; // Module gauche
#else
    sideText = "DROITE"; // Module droit
#endif

    // Centrage du texte sur le petit écran du module de cheville
    int textW = M5.Lcd.textWidth(sideText);
    int textH = M5.Lcd.fontHeight();
    M5.Lcd.setCursor((w - textW) / 2, (h - textH) / 2);
    M5.Lcd.printf("%s", sideText);

    // Initialisation de la communication radio
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
        esp_restart(); // Redémarre le microcontrôleur en cas d'erreur critique

    // Configuration de l'adresse du destinataire (Le Bracelet / WRIST_MAC)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, WRIST_MAC, 6); // Copie l'adresse MAC du bracelet définie dans Protocol.h
    peerInfo.channel = 0;                     // Canal par défaut
    peerInfo.encrypt = false;                 // Pas de chiffrement pour maximiser la vitesse
    esp_now_add_peer(&peerInfo);              // Enregistrement de l'appareil récepteur
}

void loop()
{
    Hardware::update();
    uint32_t now = millis();

    // ÉTAPE 1 : Envoi automatique du Heartbeat (Signal de vie) toutes les 500 ms
    if (now - lastHeartbeatMs >= 500)
    {
        HeartbeatMessage hb;
#if ANKLE_SIDE == 0
        hb.role = DeviceRole::ANKLE_LEFT; // Indique au bracelet qu'il s'agit de la jambe gauche
#else
        hb.role = DeviceRole::ANKLE_RIGHT; // Indique au bracelet qu'il s'agit de la jambe droite
#endif
        hb.batteryLevel = M5.Power.getBatteryLevel(); // Récupère le pourcentage réel de la batterie

        // Envoi radio direct de la structure au bracelet
        esp_now_send(WRIST_MAC, (uint8_t *)&hb, sizeof(hb));
        lastHeartbeatMs = now;
    }

    // ÉTAPE 2 : Traitement et filtrage de l'accélération tridimensionnelle
    float accel = Hardware::getAccelMagnitude(); // Calcule la force résultante combinant les axes X, Y et Z (Racine carrée de x²+y²+z²)

    // Soumission de la mesure brute au détecteur de pics d'impact
    auto peak = detector.processSample(accel, now);

    // Si la fonction retourne une valeur (.has_value()), c'est qu'un pas complet vient d'être validé !
    if (peak.has_value())
    {
        ImpactMessage msg;
        msg.peakForce = peak.value(); // On extrait la valeur maximale atteinte en G
        msg.seqNum = seqNum++;        // Attribution d'un index de message unique
#if ANKLE_SIDE == 0
        msg.isLeft = 1; // Identifiant Gauche
#else
        msg.isLeft = 0; // Identifiant Droit
#endif
        // Envoi immédiat et prioritaire de la force de l'impact au bracelet
        esp_now_send(WRIST_MAC, (uint8_t *)&msg, sizeof(msg));
    }

    delay(10); // Cadence d'acquisition fixée à 100Hz (un échantillon toutes les 10ms)
}