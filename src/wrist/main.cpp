#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

// ─── ARCHITECTURE DE DESIGN TRADITIONNELLE FREERTOS ───
enum class EventType
{
    IMPACT,
    HEARTBEAT
};

struct SystemEvent
{
    EventType type;
    union
    {
        ImpactMessage impact;
        HeartbeatMessage heartbeat;
    } data;
};

// File d'attente pour le passage sécurisé des messages inter-threads
QueueHandle_t eventQueue = nullptr;

// ─── STOCKS SÉCURISÉS (Exclusivement modifiés dans le thread principal loop) ───
float leftForce = 0.0f, rightForce = 0.0f;
uint32_t leftHeartbeatTime = 0, rightHeartbeatTime = 0;
uint8_t leftBattery = 0;
uint8_t rightBattery = 0;

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

    if (bodyCenter != nullptr)
    {
        centerData = bodyCenter;
    }

    Hardware::setBackgroundColor(bgColor);
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
    Hardware::display(title, bodyLeft, bodyRight);
}

// ─── CALLBACK RECEPTION SANS FIL (Thread Réseau - ISR/Asynchrones) ───
void onDataReceived(const uint8_t *mac, const uint8_t *data, int len)
{
    if (eventQueue == nullptr)
        return;

    SystemEvent event;

    if (len == sizeof(ImpactMessage))
    {
        event.type = EventType::IMPACT;
        memcpy(&event.data.impact, data, sizeof(ImpactMessage));
        // Envoi non bloquant (délai 0) pour ne pas figer la pile réseau ESP-NOW
        xQueueSend(eventQueue, &event, 0);
    }
    else if (len == sizeof(HeartbeatMessage))
    {
        event.type = EventType::HEARTBEAT;
        memcpy(&event.data.heartbeat, data, sizeof(HeartbeatMessage));
        xQueueSend(eventQueue, &event, 0);
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
        analyzer.reset();
        Hardware::beep(1000, 50);
        delay(80);
        Hardware::beep(1000, 50);
        break;
    case SystemState::RUNNING_NORMAL:
        Hardware::beep(1500, 400);
        break;
    default:
        break;
    }
    lastDisplayTime = millis();
}

void setup()
{
    Serial.begin(115200);
    Hardware::init();

    // Initialisation de la Queue capable de stocker 20 événements simultanés
    eventQueue = xQueueCreate(20, sizeof(SystemEvent));
    if (eventQueue == nullptr)
    {
        Serial.println("Erreur critique: Impossible de créer la Queue FreeRTOS");
        esp_restart();
    }

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

    // ÉTAPE 1 : DÉPILAGE ET TRAITEMENT SYNCHRONE DES ÉVÉNEMENTS (Zéro Concurrence/Race Condition)
    SystemEvent event;
    while (xQueueReceive(eventQueue, &event, 0) == pdTRUE)
    {
        if (event.type == EventType::IMPACT)
        {
            ImpactMessage msg = event.data.impact;

            if (msg.isLeft)
            {
                leftForce = msg.peakForce;
            }
            else
            {
                rightForce = msg.peakForce;
            }

            if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
            {
                analyzer.addRunningStep(msg.peakForce, msg.isLeft);
                runningImpactProcessed = true;
            }
            else if (currentState == SystemState::DIAGNOSTIC)
            {
                Hardware::beep(1000, 50);
                const char *impactSide = msg.isLeft ? "GAUCHE" : "DROITE";
                updateDisplay(impactSide);
                delay(200);
                updateDisplay();
                lastDisplayTime = now;
            }
            else if (currentState == SystemState::CALIBRATION && msg.peakForce >= analyzer.getMinForceThreshold())
            {
                bool calibDone = analyzer.addCalibrationStep(msg.peakForce, msg.isLeft);
                char str[16];
                snprintf(str, sizeof(str), "PAS: %d/32", analyzer.getTotalSteps());
                updateDisplay(str);

                if (calibDone)
                    transitionTo(SystemState::RUNNING_NORMAL);
            }
        }
        else if (event.type == EventType::HEARTBEAT)
        {
            HeartbeatMessage hb = event.data.heartbeat;
            if (hb.role == DeviceRole::ANKLE_LEFT)
            {
                leftHeartbeatTime = millis();
                leftBattery = hb.batteryLevel;
            }
            else if (hb.role == DeviceRole::ANKLE_RIGHT)
            {
                rightHeartbeatTime = millis();
                rightBattery = hb.batteryLevel;
            }
        }
    }

    // ÉTAPE 2 : CALCULS ET AFFICHAGE EN TEMPS RÉEL DE LA COURSE
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

            if (currentState == SystemState::RUNNING_NORMAL && asymmetry > analyzer.getPersonalizedAsymmetryThreshold())
            {
                transitionTo(SystemState::RUNNING_ALERT);
                Hardware::beep(2000, 150);
            }
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

                updateDisplay(strLeft, strRight);
                lastDisplayTime = now;
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

    // ÉTAPE 3 : MACHINE D'ÉTAT (Boutons physiques)
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

    delay(10); // Laisse s'exécuter l'ordonnanceur FreeRTOS de l'ESP32
}