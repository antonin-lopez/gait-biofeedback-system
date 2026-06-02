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

SystemState currentState = SystemState::IDLE;
GaitAnalyzer analyzer;
float asymmetry = 0.0f;

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

void transitionTo(SystemState newState)
{
    currentState = newState;
    switch (currentState)
    {
    case SystemState::IDLE:
        Hardware::setBackgroundColor(0x000000); // Noir
        Hardware::display("REPOS");
        break;
    case SystemState::DIAGNOSTIC:
        Hardware::setBackgroundColor(0xFFFF00); // Jaune
        Hardware::display("DIAGNOSTIC");
        Hardware::beep(1000, 50);
        break;
    case SystemState::CALIBRATION:
        analyzer.reset();
        Hardware::setBackgroundColor(0x0000FF); // Bleu
        Hardware::display("CALIBRATION", "Pas : 0/32");
        Hardware::beep(1000, 50);
        delay(80);
        Hardware::beep(1000, 50);
        break;
    case SystemState::RUNNING_NORMAL:
        Hardware::setBackgroundColor(0x00FF00); // Vert
        Hardware::display("COURSE (OK)", "Asym : --%");
        Hardware::beep(1500, 400);
        break;
    case SystemState::RUNNING_ALERT:
        Hardware::setBackgroundColor(0xFF0000); // Rouge
        Hardware::display("ALERTE ASYM!");
        break;
    case SystemState::PAUSE:
        Hardware::setBackgroundColor(0xFF00FF); // Violet
        Hardware::display("PAUSE");
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    Hardware::init();
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(onDataReceived);
    transitionTo(SystemState::IDLE);

    Serial.println();
    Serial.print("L'ADRESSE MAC DU POIGNET EST: ");
    Serial.println(WiFi.macAddress());
}

void loop()
{
    Hardware::update();
    uint32_t now = millis();
    bool btnShort = Hardware::isShortPress();
    bool btnLong = Hardware::isLongPress();

    bool anklesConnected = (now - leftHeartbeatTime < 1500) && (now - rightHeartbeatTime < 1500);

    // 1. Traitement des impacts avec effet d'extinction/flash sur l'écran global
    if (hasNewImpact)
    {
        hasNewImpact = false;

        if (currentState == SystemState::DIAGNOSTIC)
        {
            Hardware::beep(1000, 50);
            Hardware::setBackgroundColor(0x000000); // Écran Noir furtif
            Hardware::display("DIAGNOSTIC");
            delay(40);
            Hardware::setBackgroundColor(0xFFFF00); // Retour au Jaune
            Hardware::display("DIAGNOSTIC");
        }
        else if (currentState == SystemState::CALIBRATION && lastIncomingForce >= 3.0f)
        {
            bool calibDone = analyzer.addCalibrationStep(lastIncomingForce, lastIncomingIsLeft);
            char str[16];
            sprintf(str, "Pas: %d/32", analyzer.getTotalSteps());

            Hardware::setBackgroundColor(0x000000); // Écran Noir furtif
            Hardware::display("CALIBRATION", str);
            delay(40);
            Hardware::setBackgroundColor(0x0000FF); // Retour au Bleu
            Hardware::display("CALIBRATION", str);

            if (calibDone)
                transitionTo(SystemState::RUNNING_NORMAL);
        }
    }

    // 2. Calcul d'analyse de foulée
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
                Hardware::display(currentState == SystemState::RUNNING_NORMAL ? "COURSE (OK)" : "ALERTE ASYM!", str);
            }
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