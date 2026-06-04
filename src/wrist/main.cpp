#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

// ─── VARIABLES GLOBALES ───
volatile float leftForce = 0.0f, rightForce = 0.0f;
volatile uint32_t leftImpactTime = 0, rightImpactTime = 0;
volatile uint32_t leftHeartbeatTime = 0, rightHeartbeatTime = 0;
volatile bool hasNewImpact = false;
volatile float lastIncomingForce = 0.0f;
volatile bool lastIncomingIsLeft = false;

volatile uint8_t leftBattery = 0;
volatile uint8_t rightBattery = 0;

// ─── LOGIQUE ET ÉTATS ───
SystemState currentState = SystemState::IDLE;
GaitAnalyzer analyzer(DEFAULT_VALIDATION_THRESHOLD);
float asymmetry = 0.0f;
uint32_t lastDisplayTime = 0;

// ─── SURCHARGE 1 : Affichage Centré (Repos, Diagnostic, Calibration, Pause) ───
void updateDisplay(const char *bodyCenter = nullptr)
{
    uint32_t now = millis();
    int leftBatParam = -2;
    int rightBatParam = -2;

    // Gestion des batteries uniquement dans les états prévus
    if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        leftBatParam = (now - leftHeartbeatTime < 1500) ? leftBattery : -1;
        rightBatParam = (now - rightHeartbeatTime < 1500) ? rightBattery : -1;
    }

    const char *title = "";
    const char *centerData = "";
    uint32_t bgColor = 0x000000;

    switch (currentState)
    {
    case SystemState::IDLE:
        title = "REPOS";
        bgColor = 0x000000;
        break;
    case SystemState::DIAGNOSTIC:
        title = "DIAGNOSTIC";
        bgColor = 0xFFFF00;
        break;
    case SystemState::CALIBRATION:
        title = "CALIBRATION";
        centerData = "PAS: 0/32";
        bgColor = 0x0000FF;
        break;
    case SystemState::RUNNING_NORMAL:
        title = "COURSE (OK)";
        bgColor = 0x00FF00;
        break;
    case SystemState::RUNNING_ALERT:
        title = "ALERTE ASYM!";
        bgColor = 0xFF0000;
        break;
    case SystemState::PAUSE:
        title = "PAUSE";
        bgColor = 0xFF00FF;
        break;
    }

    // Si on force un texte au centre (ex: "GAUCHE" ou "PAS: 14/32")
    if (bodyCenter != nullptr)
    {
        centerData = bodyCenter;
    }

    Hardware::setBackgroundColor(bgColor);
    // Appel automatique de la Surcharge 1 de la HAL (Centré)
    Hardware::display(title, centerData, leftBatParam, rightBatParam);
}

// ─── SURCHARGE 2 : Affichage Scindé (Utilisé uniquement en Course) ───
void updateDisplay(const char *bodyLeft, const char *bodyRight)
{
    const char *title = "";
    uint32_t bgColor = 0x000000;

    switch (currentState)
    {
    case SystemState::RUNNING_NORMAL:
        title = "COURSE (OK)";
        bgColor = 0x00FF00;
        break;
    case SystemState::RUNNING_ALERT:
        title = "ALERTE ASYM!";
        bgColor = 0xFF0000;
        break;
    default:
        title = "COURSE";
        bgColor = 0x000000;
        break;
    }

    Hardware::setBackgroundColor(bgColor);
    // Appel automatique de la Surcharge 2 de la HAL (Scindé gauche/droite)
    // Les batteries restent à -2 par défaut (bandeau inférieur masqué en course)
    Hardware::display(title, bodyLeft, bodyRight);
}

// ─── CALLBACK RECEPTION SANS FIL ───
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
    updateDisplay(); // Appelle par défaut la version centrée (Surcharge 1)

    switch (currentState)
    {
    case SystemState::DIAGNOSTIC:
        Hardware::beep(1000, 50);
        break;
    case SystemState::CALIBRATION:
        analyzer.reset();
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
    bool runningImpactProcessed = false;

    // ─── ÉTAPE 1 : TRAITEMENT CENTRALISÉ DES NOUVEAUX IMPACTS ───
    if (hasNewImpact)
    {
        hasNewImpact = false;

        if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
        {
            analyzer.addRunningStep(lastIncomingForce, lastIncomingIsLeft);
            runningImpactProcessed = true;
        }
        else if (currentState == SystemState::DIAGNOSTIC)
        {
            Hardware::beep(1000, 50);
            const char *impactSide = lastIncomingIsLeft ? "GAUCHE" : "DROITE";
            updateDisplay(impactSide); // Appelle Surcharge 1
            delay(200);
            updateDisplay(); // Appelle Surcharge 1
            lastDisplayTime = now;
        }
        else if (currentState == SystemState::CALIBRATION && lastIncomingForce >= analyzer.getMinForceThreshold())
        {
            bool calibDone = analyzer.addCalibrationStep(lastIncomingForce, lastIncomingIsLeft);
            char str[16];
            sprintf(str, "PAS: %d/32", analyzer.getTotalSteps());
            updateDisplay(str); // Appelle Surcharge 1

            if (calibDone)
                transitionTo(SystemState::RUNNING_NORMAL);
        }
    }

    // ─── ÉTAPE 2 : CALCULS ET AFFICHAGE EN TEMPS RÉEL DE LA COURSE ───
    if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
    {
        if (!anklesConnected)
        {
            transitionTo(SystemState::PAUSE);
        }
        else
        {
            float avgLeft = analyzer.getLeftAverage();
            float avgRight = analyzer.getRightAverage();
            float total = avgLeft + avgRight;

            float pctLeft = (total > 0.0f) ? (avgLeft / total) * 100.0f : 50.0f;
            float pctRight = (total > 0.0f) ? (avgRight / total) * 100.0f : 50.0f;

            asymmetry = analyzer.computeAsymmetry(avgLeft, avgRight);

            // MODIFIER ICI : Remplacer 10.0f par le seuil dynamique personnalisé
            if (currentState == SystemState::RUNNING_NORMAL && asymmetry > analyzer.getPersonalizedAsymmetryThreshold())
            {
                transitionTo(SystemState::RUNNING_ALERT);
                Hardware::beep(2000, 150);
            }
            // MODIFIER ICI : Remplacer 10.0f par le seuil dynamique personnalisé
            else if (currentState == SystemState::RUNNING_ALERT && asymmetry <= analyzer.getPersonalizedAsymmetryThreshold())
            {
                transitionTo(SystemState::RUNNING_NORMAL);
            }

            if (refreshDue || runningImpactProcessed)
            {
                char strLeft[16];
                char strRight[16];
                snprintf(strLeft, sizeof(strLeft), "G: %.0f%%", pctLeft);
                snprintf(strRight, sizeof(strRight), "D: %.0f%%", pctRight);

                // AVANT : updateDisplay(nullptr, strLeft, strRight);
                // MAINTENANT : L'appel est direct, propre et sans placeholder grâce à la surcharge !
                updateDisplay(strLeft, strRight);
                lastDisplayTime = now;
            }
        }
    }
    else if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        if (refreshDue)
        {
            updateDisplay(); // Appelle Surcharge 1
            lastDisplayTime = now;
        }
    }

    // ─── ÉTAPE 3 : MACHINE D'ÉTAT ───
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