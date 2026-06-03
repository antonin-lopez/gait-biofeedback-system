#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "Protocol.h"
#include "Hardware.h"
#include "GaitAlgorithms.h"

// ─── VARIABLES GLOBALES DE STOCKAGE (PARTAGÉES AVEC L'INTERRUPTION SANS FIL) ───
volatile float leftForce = 0.0f, rightForce = 0.0f;              // Stockent la force instantanée du dernier impact de chaque pied.
volatile uint32_t leftImpactTime = 0, rightImpactTime = 0;       // Enregistrent l'heure précise (en ms) des derniers impacts reçus.
volatile uint32_t leftHeartbeatTime = 0, rightHeartbeatTime = 0; // Heure de réception des derniers signaux de vie (Heartbeats).
volatile bool hasNewImpact = false;                              // Flag mis à 'true' dès qu'un message d'impact arrive via ESP-NOW.
volatile float lastIncomingForce = 0.0f;                         // Valeur brute de la force du tout dernier impact reçu.
volatile bool lastIncomingIsLeft = false;                        // Origine du dernier impact reçu (true = gauche, false = droit).

volatile uint8_t leftBattery = 0;  // Niveau de batterie de la cheville gauche.
volatile uint8_t rightBattery = 0; // Niveau de batterie de la cheville droite.

// ─── LOGIQUE ET ÉTATS DU SYSTÈME ───
SystemState currentState = SystemState::IDLE; // État initial du système : REPOS (IDLE).
GaitAnalyzer analyzer;                        // Instance de notre outil de calcul de marche.
float asymmetry = 0.0f;                       // Variable qui stocke le taux d'asymétrie calculé en direct.
uint32_t lastDisplayTime = 0;                 // Stocke l'heure du dernier rafraîchissement d'affichage.

/**
 * @brief Gère l'affichage sur l'écran LCD du M5Stick en fonction de l'état actuel.
 * @param dataOverride Permet de forcer un texte spécifique au milieu de l'écran (ex: "GAUCHE", "PAS: 5/32").
 */
void updateDisplay(const char *dataOverride = nullptr)
{
    uint32_t now = millis();
    int leftBatParam = -2; // Valeur par défaut (-2) = ne pas afficher la batterie sur l'écran.
    int rightBatParam = -2;

    // On affiche l'état des batteries des chevilles uniquement en mode DIAGNOSTIC ou PAUSE
    if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        // Si le dernier heartbeat de la cheville a moins de 1.5s, on affiche sa batterie. Sinon, on met -1 ("--") pour signaler une déconnexion.
        leftBatParam = (now - leftHeartbeatTime < 1500) ? leftBattery : -1;
        rightBatParam = (now - rightHeartbeatTime < 1500) ? rightBattery : -1;
    }

    const char *title = "";
    const char *data = "";
    uint32_t bgColor = 0x000000; // Couleur de fond au format RVB (Hexadécimal)

    // Configuration des textes et couleurs selon l'état de la Machine d'État
    switch (currentState)
    {
    case SystemState::IDLE:
        title = "REPOS";
        data = "";
        bgColor = 0x000000; // Noir
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

    // Si un texte personnalisé est fourni en paramètre, il remplace le texte par défaut
    if (dataOverride != nullptr)
    {
        data = dataOverride;
    }

    // Envoi des ordres à la bibliothèque matérielle (HAL) pour dessiner l'écran
    Hardware::setBackgroundColor(bgColor);
    Hardware::display(title, data, leftBatParam, rightBatParam);
}

/**
 * @brief FONCTION CALLBACK SANS FIL (ESP-NOW) : Déclenchée automatiquement dès qu'une cheville envoie des données.
 * @note Cette fonction s'exécute en tâche de fond (Interruption), elle doit être très rapide.
 */
void onDataReceived(const uint8_t *mac, const uint8_t *data, int len)
{
    // CAS 1 : C'est un message d'impact (un pas vient d'avoir lieu)
    if (len == sizeof(ImpactMessage))
    {
        ImpactMessage msg;
        memcpy(&msg, data, sizeof(msg)); // On copie les données brutes reçues dans notre structure structurée

        lastIncomingForce = msg.peakForce; // On sauvegarde la force du pas
        lastIncomingIsLeft = msg.isLeft;   // On sauvegarde le côté (1=gauche, 0=droit)
        hasNewImpact = true;               // On active le flag pour prévenir la boucle principale (loop)

        // On met à jour les variables spécifiques au côté concerné
        if (msg.isLeft)
        {
            leftForce = msg.peakForce;
            leftImpactTime = millis(); // Enregistrement du moment exact de l'impact gauche
        }
        else
        {
            rightForce = msg.peakForce;
            rightImpactTime = millis(); // Enregistrement du moment exact de l'impact droit
        }
    }
    // CAS 2 : C'est un message de signal de vie (Heartbeat périodique toutes les 500ms)
    else if (len == sizeof(HeartbeatMessage))
    {
        HeartbeatMessage hb;
        memcpy(&hb, data, sizeof(hb));

        if (hb.role == DeviceRole::ANKLE_LEFT)
        {
            leftHeartbeatTime = millis();  // On actualise l'heure du dernier signe de vie gauche
            leftBattery = hb.batteryLevel; // Sauvegarde du niveau de batterie gauche
        }
        if (hb.role == DeviceRole::ANKLE_RIGHT)
        {
            rightHeartbeatTime = millis();  // On actualise l'heure du dernier signe de vie droit
            rightBattery = hb.batteryLevel; // Sauvegarde du niveau de batterie droit
        }
    }
}

/**
 * @brief Gère proprement le changement d'état du système et déclenche les bips sonores associés.
 */
void transitionTo(SystemState newState)
{
    currentState = newState; // On applique le nouvel état
    updateDisplay();         // On rafraîchit immédiatement l'écran avec la nouvelle couleur de fond

    // Génération de signaux sonores d'information (Bips) selon la transition
    switch (currentState)
    {
    case SystemState::DIAGNOSTIC:
        Hardware::beep(1000, 50); // 1 bip court
        break;
    case SystemState::CALIBRATION:
        Hardware::beep(1000, 50);
        delay(80);
        Hardware::beep(1000, 50); // 2 bips courts
        break;
    case SystemState::RUNNING_NORMAL:
        Hardware::beep(1500, 400); // 1 bip long aigu pour signaler le début du suivi de course
        break;
    }
    lastDisplayTime = millis(); // Initialisation du chrono de rafraîchissement
}

/**
 * @brief Initialisation du matériel au démarrage du bracelet.
 */
void setup()
{
    Serial.begin(115200);                     // Activation du moniteur série pour le débogage informatique
    Hardware::init();                         // Initialisation de l'écran, boutons et capteurs du M5Stick
    WiFi.mode(WIFI_STA);                      // Activation du mode Wi-Fi Station (obligatoire pour utiliser ESP-NOW)
    esp_now_init();                           // Initialisation du protocole sans fil ESP-NOW
    esp_now_register_recv_cb(onDataReceived); // Liaison de notre fonction callback de réception

    analyzer.setMinForceThreshold(DEFAULT_VALIDATION_THRESHOLD); // Configuration du seuil de validation des pas pour l'analyseur de marche

    transitionTo(SystemState::IDLE); // On démarre le système au repos ("REPOS")
}

/**
 * @brief BOUCLE PRINCIPALE (S'exécute en boucle infinie)
 */
void loop()
{
    Hardware::update(); // Actualise l'état interne des boutons matériels du M5Stick
    uint32_t now = millis();

    // Lecture des actions de l'utilisateur sur le bouton principal (Bouton A du M5Stick)
    bool btnShort = Hardware::isShortPress();
    bool btnLong = Hardware::isLongPress();

    // Vérification de la connexion sans fil : On considère les chevilles connectées si on a reçu
    // un signal de vie de CHACUNE d'elles il y a moins de 1,5 seconde.
    bool anklesConnected = (now - leftHeartbeatTime < 1500) && (now - rightHeartbeatTime < 1500);

    // Détermine si 500 ms se sont écoulées pour rafraîchir périodiquement l'affichage
    bool refreshDue = (now - lastDisplayTime >= 500);

    // ÉTAPE 1 : TRAITEMENT EN DIRECT DES NOUVEAUX IMPACTS
    if (hasNewImpact)
    {
        hasNewImpact = false; // On abaisse le flag pour attendre le prochain pas

        // Comportement dans l'état DIAGNOSTIC
        if (currentState == SystemState::DIAGNOSTIC)
        {
            Hardware::beep(1000, 50); // Petit bip de confirmation d'impact

            // On affiche temporairement de quel côté provient l'impact reçu
            const char *impactSide = lastIncomingIsLeft ? "GAUCHE" : "DROITE";
            updateDisplay(impactSide);

            delay(200); // On fige l'affichage pendant 200 ms pour laisser le temps de lire

            updateDisplay(); // On restaure l'écran jaune par défaut ("DIAGNOSTIC")
            lastDisplayTime = now;
        }
        // Comportement dans l'état CALIBRATION
        else if (currentState == SystemState::CALIBRATION && lastIncomingForce >= analyzer.getMinForceThreshold())
        {
            // On soumet le pas valide (>= 3.0G) à l'analyseur pour incrémenter les compteurs
            bool calibDone = analyzer.addCalibrationStep(lastIncomingForce, lastIncomingIsLeft);

            char str[16];
            sprintf(str, "PAS: %d/32", analyzer.getTotalSteps()); // On prépare la chaîne (ex: "PAS: 12/32")
            updateDisplay(str);                                   // On met à jour directement le texte sur fond bleu

            // Si l'analyseur confirme qu'on a fait 16 pas de chaque côté (32 au total)
            if (calibDone)
                transitionTo(SystemState::RUNNING_NORMAL); // On lance automatiquement le mode course !
        }
    }

    // ÉTAPE 2 : CALCULS CONTINUS ET RAFRAÎCHISSEMENT DE L'ÉCRAN
    if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
    {
        // SÉCURITÉ : Si un capteur d'ancre se déconnecte pendant la course, on passe instantanément en mode PAUSE
        if (!anklesConnected)
        {
            transitionTo(SystemState::PAUSE);
        }
        // Si les deux derniers impacts reçus (gauche et droit) ont eu lieu il y a moins de 1,5 seconde
        else if (now - leftImpactTime < 1500 && now - rightImpactTime < 1500)
        {
            // Cohérence temporelle : On vérifie que l'écart de temps entre le pas gauche et le pas droit
            // est inférieur à 600 ms (ce qui correspond au rythme naturel d'une foulée de course).
            if (abs((long)(leftImpactTime - rightImpactTime)) <= 600)
            {
                // CALCUL DE L'ASYMÉTRIE : On compare les forces maximales instantanées des derniers pas gauches et droits.
                asymmetry = analyzer.computeAsymmetry(leftForce, rightForce);

                char str[16];
                sprintf(str, "Asym : %.1f%%", asymmetry); // Préparation de l'affichage (ex: "Asym : 12.4%")

                // GESTION DU SEUIL D'ALERTE (Déclenchement du mode Alerte si asymétrie > 10.0%)
                if (currentState == SystemState::RUNNING_NORMAL && asymmetry > 10.0f)
                {
                    transitionTo(SystemState::RUNNING_ALERT); // Passage au fond rouge
                    Hardware::beep(2000, 150);                // Bip d'alerte strident pour avertir le coureur
                }
                // Rétablissement si l'utilisateur corrige sa posture et repasse sous les 10%
                else if (currentState == SystemState::RUNNING_ALERT && asymmetry <= 10.0f)
                {
                    transitionTo(SystemState::RUNNING_NORMAL); // Retour au fond vert
                }
                updateDisplay(str); // Affiche le taux d'asymétrie calculé
            }
        }
    }
    // Pour les modes Diagnostic et Pause, on se contente de rafraîchir périodiquement l'affichage des batteries
    else if (currentState == SystemState::DIAGNOSTIC || currentState == SystemState::PAUSE)
    {
        if (refreshDue)
        {
            updateDisplay();
            lastDisplayTime = now;
        }
    }

    // ÉTAPE 3 : MACHINE D'ÉTAT (GESTION DES BOUTONS ET TRANSITIONS)
    switch (currentState)
    {
    case SystemState::IDLE:
        if (btnShort)
            transitionTo(SystemState::DIAGNOSTIC); // Appui court -> Mode Diagnostic
        if (btnLong)
            transitionTo(SystemState::CALIBRATION); // Appui long  -> Mode Calibration
        break;

    case SystemState::DIAGNOSTIC:
        if (btnShort)
            transitionTo(SystemState::IDLE); // Appui court -> Retour au Repos
        if (btnLong)
            transitionTo(SystemState::CALIBRATION); // Appui long  -> Saute à la Calibration
        break;

    case SystemState::CALIBRATION:
        if (btnLong)
            transitionTo(SystemState::IDLE); // Appui long  -> Annulation et retour au Repos
        break;

    case SystemState::RUNNING_NORMAL:
    case SystemState::RUNNING_ALERT:
        if (btnShort)
            transitionTo(SystemState::PAUSE); // Appui court -> Met la course en Pause
        if (btnLong)
            transitionTo(SystemState::IDLE); // Appui long  -> Arrête la course et va au Repos
        break;

    case SystemState::PAUSE:
        if (btnShort)
            transitionTo(SystemState::RUNNING_NORMAL); // Appui court -> Reprend la course (Fond vert)
        if (btnLong)
            transitionTo(SystemState::IDLE); // Appui long  -> Arrête définitivement, retour au Repos
        break;
    }

    delay(20); // Petite pause de 20 millisecondes pour laisser respirer le processeur ESP32
}