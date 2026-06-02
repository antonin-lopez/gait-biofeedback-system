#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

// Variables partagées modifiées sous interruption réseau
volatile float leftForce = 0.0f, rightForce = 0.0f;
volatile uint32_t leftImpactTime = 0, rightImpactTime = 0;
volatile uint32_t leftHeartbeatTime = 0, rightHeartbeatTime = 0;
volatile bool hasNewImpact = false;
volatile float lastIncomingForce = 0.0f;
volatile bool lastIncomingIsLeft = false;

SystemState currentState = SystemState::IDLE;
GaitAnalyzer analyzer;
float asymmetry = 0.0f;

// Callback Réception Réseau Rapide
void onDataReceived(const uint8_t *mac, const uint8_t *data, int len)
{
    if (len == sizeof(ImpactMessage))
    {
        ImpactMessage msg;
        memcpy(&msg, data, sizeof(msg));
        lastIncomingForce = msg.peakForce;
        lastIncomingIsLeft = msg.isLeft;
        hasNewImpact = true;

        if (msg.isLeft)
        {
            leftForce = msg.peakForce;
            leftImpactTime = millis();
        }
        else
        {
            rightForce = msg.peakForce;
            rightImpactTime = millis();
        }
    }
    else if (len == sizeof(HeartbeatMessage))
    {
        HeartbeatMessage hb;
        memcpy(&hb, data, sizeof(hb));
        if (hb.role == DeviceRole::ANKLE_LEFT)
            leftHeartbeatTime = millis();
        if (hb.role == DeviceRole::ANKLE_RIGHT)
            rightHeartbeatTime = millis();
    }
}

// Gestionnaire Central des Changements d'États (Comportements d'Entrée)
void transitionTo(SystemState newState)
{
    currentState = newState;
    switch (currentState)
    {
    case SystemState::IDLE:
        Hardware::setLEDColor(0xFF6600); // Orange lent/respiration simulé par couleur fixe
        Hardware::display("REPOS");
        break;
    case SystemState::DIAGNOSTIC:
        Hardware::setLEDColor(0xFFFFFF); // Blanc Fixe
        Hardware::display("DIAGNOSTIC", "Pret a tester");
        Hardware::beep(1000, 50);
        break;
    case SystemState::CALIBRATION:
        analyzer.reset();
        Hardware::setLEDColor(0x0066FF); // Bleu
        Hardware::display("CALIBRATION", "Pas: 0/30");
        Hardware::beep(1000, 50);
        delay(80);
        Hardware::beep(1000, 50); // Bip-Bip
        break;
    case SystemState::RUNNING_NORMAL:
        Hardware::setLEDColor(0x00FF00); // Vert fixe
        Hardware::display("COURSE (OK)", "Asym: --%");
        Hardware::beep(1500, 400); // Long bip
        break;
    case SystemState::RUNNING_ALERT:
        Hardware::setLEDColor(0xFF0000); // Rouge
        Hardware::display("ALERTE ASYM!");
        break;
    case SystemState::PAUSE:
        Hardware::setLEDColor(0xFF6600); // Orange Fixe
        Hardware::display("PAUSE");
        break;
    }
}

void setup()
{
    Hardware::init();
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onDataReceived);
    transitionTo(SystemState::IDLE);
}

void loop()
{
    Hardware::update();
    uint32_t now = millis();
    bool btnShort = Hardware::isShortPress();
    bool btnLong = Hardware::isLongPress();

    // Vérification de la connectivité (Timeout 1500ms)
    bool anklesConnected = (now - leftHeartbeatTime < 1500) && (now - rightHeartbeatTime < 1500);

    // 1. Traitement des événements asynchrones liés aux impacts reçus
    if (hasNewImpact)
    {
        hasNewImpact = false;

        if (currentState == SystemState::DIAGNOSTIC)
        {
            Hardware::beep(1000, 50);
            Hardware::setLEDColor(0x000000);
            delay(20);
            Hardware::setLEDColor(0xFFFFFF); // Flash blanc
        }
        else if (currentState == SystemState::CALIBRATION && lastIncomingForce >= 3.0f)
        {
            bool calibDone = analyzer.addCalibrationStep(lastIncomingForce, lastIncomingIsLeft);
            char str[16];
            sprintf(str, "Pas: %d/30", analyzer.getTotalSteps());
            Hardware::display("CALIBRATION", str);
            Hardware::setLEDColor(0x000000);
            delay(20);
            Hardware::setLEDColor(0x0066FF); // Flash bleu

            if (calibDone)
                transitionTo(SystemState::RUNNING_NORMAL);
        }
    }

    // 2. Calcul d'analyse de foulée périodique (en mode course)
    if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
    {
        if (!anklesConnected)
        {
            transitionTo(SystemState::PAUSE); // Auto-pause si déconnexion
        }
        else if (now - leftImpactTime < 1500 && now - rightImpactTime < 1500)
        {
            // Appariement d'une foulée complète si l'écart est inférieur à 600ms
            if (abs((long)(leftImpactTime - rightImpactTime)) <= 600)
            {
                asymmetry = analyzer.computeAsymmetry(leftForce, rightForce);
                char str[16];
                sprintf(str, "Asym: %.1f%%", asymmetry);

                if (currentState == SystemState::RUNNING_NORMAL && asymmetry > 10.0f)
                {
                    transitionTo(SystemState::RUNNING_ALERT);
                    Hardware::beep(2000, 150);
                }
                else if (currentState == SystemState::RUNNING_ALERT && asymmetry <= 10.0f)
                {
                    transitionTo(SystemState::RUNNING_NORMAL);
                }
                Hardware::display(currentState == SystemState::RUNNING_NORMAL ? "COURSE (OK)" : "ALERTE ASYM!", str);
            }
        }
    }

    // 3. Logique de transition de la Machine d'État (Boutons)
    switch (currentState)
    {
    case SystemState::IDLE:
        if (btnShort)
            transitionTo(SystemState::DIAGNOSTIC);
        if (btnLong)
            transitionTo(SystemState::CALIBRATION);
        break;
    case SystemState::DIAGNOSTIC:
        if (btnShort)
            transitionTo(SystemState::IDLE);
        if (btnLong)
            transitionTo(SystemState::CALIBRATION);
        break;
    case SystemState::CALIBRATION:
        if (btnLong)
            transitionTo(SystemState::IDLE);
        break;
    case SystemState::RUNNING_NORMAL:
    case SystemState::RUNNING_ALERT:
        if (btnShort)
            transitionTo(SystemState::PAUSE);
        if (btnLong)
            transitionTo(SystemState::IDLE);
        break;
    case SystemState::PAUSE:
        if (btnShort)
            transitionTo(SystemState::RUNNING_NORMAL);
        if (btnLong)
            transitionTo(SystemState::IDLE);
        break;
    }

    delay(20); // Fréquence de rafraîchissement de la logique générale (50Hz)
}