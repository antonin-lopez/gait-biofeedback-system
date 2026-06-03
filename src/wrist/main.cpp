#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

volatile float leftForce = 0.0f, rightForce = 0.0f;
volatile uint32_t leftImpactTime = 0, rightImpactTime = 0;
volatile uint32_t leftHeartbeatTime = 0, rightHeartbeatTime = 0;
volatile bool hasNewImpact = false;
volatile float lastIncomingForce = 0.0f;
volatile bool lastIncomingIsLeft = false;

volatile uint8_t leftBattery = 0;
volatile uint8_t rightBattery = 0;

SystemState currentState = SystemState::IDLE;
GaitAnalyzer analyzer;
float asymmetry = 0.0f;
uint32_t lastDisplayTime = 0;

// ─── FONCTION DE RENDU CENTRALISÉE (NETTOYÉE DE "forceBlackBg") ───
void updateDisplay(const char *dataOverride = nullptr)
{
    uint32_t now = millis();

    int leftBatParam = -2;
    int rightBatParam = -2;

    if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        leftBatParam = (now - leftHeartbeatTime < 1500) ? leftBattery : -1;
        rightBatParam = (now - rightHeartbeatTime < 1500) ? rightBattery : -1;
    }

    const char *title = "";
    const char *data = "";
    uint32_t bgColor = 0x000000;

    switch (currentState)
    {
    case SystemState::IDLE:
        title = "REPOS";
        data = "";
        bgColor = 0x000000;
        break;
    case SystemState::DIAGNOSTIC:
        title = "DIAGNOSTIC";
        data = "";
        bgColor = 0xFFFF00; // Jaune
        break;
    case SystemState::CALIBRATION:
        title = "CALIBRATION";
        data = "PAS: 0/32";
        bgColor = 0x0000FF; // Bleu
        break;
    case SystemState::RUNNING_NORMAL:
        title = "COURSE (OK)";
        data = "Asym : --%";
        bgColor = 0x00FF00; // Vert
        break;
    case SystemState::RUNNING_ALERT:
        title = "ALERTE ASYM!";
        data = "";
        bgColor = 0xFF0000; // Rouge
        break;
    case SystemState::PAUSE:
        title = "PAUSE";
        data = "";
        bgColor = 0xFF00FF; // Violet
        break;
    }

    if (dataOverride != nullptr)
    {
        data = dataOverride;
    }

    Hardware::setBackgroundColor(bgColor);
    Hardware::display(title, data, leftBatParam, rightBatParam);
}

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
        {
            leftHeartbeatTime = millis();
            leftBattery = hb.batteryLevel;
        }
        if (hb.role == DeviceRole::ANKLE_RIGHT)
        {
            rightHeartbeatTime = millis();
            rightBattery = hb.batteryLevel;
        }
    }
}

void transitionTo(SystemState newState)
{
    currentState = newState;
    updateDisplay();

    switch (currentState)
    {
    case SystemState::DIAGNOSTIC:
        Hardware::beep(1000, 50);
        break;
    case SystemState::CALIBRATION:
        Hardware::beep(1000, 50);
        delay(80);
        Hardware::beep(1000, 50);
        break;
    case SystemState::RUNNING_NORMAL:
        Hardware::beep(1500, 400);
        break;
    }
    lastDisplayTime = millis();
}

void setup()
{
    Serial.begin(115200);
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

    bool anklesConnected = (now - leftHeartbeatTime < 1500) && (now - rightHeartbeatTime < 1500);
    bool refreshDue = (now - lastDisplayTime >= 500);

    // 1. Traitement des événements d'impacts (Nouveau comportement demandé)
    if (hasNewImpact)
    {
        hasNewImpact = false;

        if (currentState == SystemState::DIAGNOSTIC)
        {
            Hardware::beep(1000, 50);

            // On écrit textuellement "GAUCHE" ou "DROITE" selon l'origine de l'impact
            const char *impactSide = lastIncomingIsLeft ? "GAUCHE" : "DROITE";
            updateDisplay(impactSide);

            delay(200); // Maintien de l'écriture pendant 200ms

            updateDisplay(); // Restaure l'écran jaune par défaut ("Pret a tester")
            lastDisplayTime = now;
        }
        else if (currentState == SystemState::CALIBRATION && lastIncomingForce >= 3.0f)
        {
            bool calibDone = analyzer.addCalibrationStep(lastIncomingForce, lastIncomingIsLeft);
            char str[16];
            sprintf(str, "PAS: %d/32", analyzer.getTotalSteps());

            // PLUS DE FLASH : On écrit juste le nouveau score directement sur fond bleu
            updateDisplay(str);

            if (calibDone)
                transitionTo(SystemState::RUNNING_NORMAL);
        }
    }

    // 2. Traitement des calculs de course continus ou rafraîchissement périodique
    if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
    {
        if (!anklesConnected)
        {
            transitionTo(SystemState::PAUSE);
        }
        else if (now - leftImpactTime < 1500 && now - rightImpactTime < 1500)
        {
            if (abs((long)(leftImpactTime - rightImpactTime)) <= 600)
            {
                asymmetry = analyzer.computeAsymmetry(leftForce, rightForce);
                char str[16];
                sprintf(str, "Asym : %.1f%%", asymmetry);

                if (currentState == SystemState::RUNNING_NORMAL && asymmetry > 10.0f)
                {
                    transitionTo(SystemState::RUNNING_ALERT);
                    Hardware::beep(2000, 150);
                }
                else if (currentState == SystemState::RUNNING_ALERT && asymmetry <= 10.0f)
                {
                    transitionTo(SystemState::RUNNING_NORMAL);
                }
                updateDisplay(str);
            }
        }
    }
    else if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        if (refreshDue)
        {
            updateDisplay();
            lastDisplayTime = now;
        }
    }

    // 3. Machine d'État
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

    delay(20);
}
