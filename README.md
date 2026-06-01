# Architecture & Spécifications Logicielles

Ce document fait office de spécification technique absolue pour le système embarqué multi-cible de biofeedback destiné aux coureurs asymétriques. L'architecture repose sur un découplage strict entre la logique métier (algorithmes purs), l'orchestration système (FSM) et l'infrastructure matérielle (HAL), permettant une testabilité à 100% sur plateforme native (PC).

---

## 1. Fichier de Configuration Global (`platformio.ini`)

Placé à la racine du projet, ce fichier orchestre la compilation multi-cible pour l'ESP32 du M5StickC Plus2 et la plateforme de test native.

```ini
[platformio]
default_envs = wrist, ankle_left, ankle_right

[env]
platform = espressif32
board = m5stick-c
framework = arduino
monitor_speed = 115200
lib_deps = 
    m5stack/M5Unified @ ^0.1.12
build_flags = 
    -Wall
    -Wextra

[env:wrist]
build_flags = ${env.build_flags} -D TARGET_WRIST
build_src_filter = +<common/> +<wrist/> -<ankle/>

[env:ankle_left]
build_flags = ${env.build_flags} -D TARGET_ANKLE -D IS_LEFT_ANKLE
build_src_filter = +<common/> +<ankle/> -<wrist/>

[env:ankle_right]
build_flags = ${env.build_flags} -D TARGET_ANKLE -D IS_RIGHT_ANKLE
build_src_filter = +<common/> +<ankle/> -<wrist/>

[env:native]
platform = native
build_flags = -std=gnu++17 -D TARGET_NATIVE
lib_compat_mode = off
build_src_filter = +<common/> -<wrist/> -<ankle/>
```

---

## 2. Dossier `include/` : Constantes et Protocoles Globaux

Ces fichiers contiennent des données immuables et partagées. Ils ne contiennent aucune logique algorithmique.

### `include/AppConfig.h`
* **Description :** Seuils métiers, paramètres de filtrage et timings globaux.
* **Imports :** Aucun.
* **Contenu :**
    * `constexpr float ASYMMETRY_THRESHOLD = 10.0f;` (Seuil de déclenchement de l'alerte en %).
    * `constexpr uint8_t CALIBRATION_REQUIRED_STEPS = 30;` (Nombre de pas pour validation de la baseline).
    * `constexpr float FILTER_EMA_ALPHA = 0.2f;` (Coefficient de l'Exponential Moving Average pour les impacts).
    * `constexpr uint32_t DEBOUNCE_DELAY_MS = 50;` (Anti-rebond logiciel du bouton).
    * `constexpr uint32_t SAMPLING_RATE_IMU_HZ = 100;` (Fréquence d'acquisition de l'accéléromètre).

### `include/Protocol.h`
* **Description :** Structures de données exactes alignées en mémoire pour les paquets réseau ESP-NOW.
* **Imports :** `<cstdint>`
* **Contenu :**
    ```cpp
    #pragma pack(push, 1)
    struct ImpactPayload {
        uint32_t timestampMs;    // Heure locale de l'événement chiffrée par le nœud
        float peakDeceleration;  // Valeur brute du pic de décélération en G
        uint8_t footSide;        // 0 = Gauche, 1 = Droite (basé sur Types.h::FootSide)
    };
    
    struct HeartbeatPayload {
        uint8_t deviceRole;      // Rôle de l'appareil émetteur
        uint8_t batteryLevel;    // Pourcentage de batterie restant
    };
    #pragma pack(pop)
    ```

### `include/Types.h`
* **Description :** Énumérations et types forts transverses.
* **Imports :** Aucun.
* **Contenu :**
    * `enum class SystemState : uint8_t { REPOS, DIAGNOSTIC, CALIBRATION, COURSE_NORMAL, COURSE_ALERTE, PAUSE };`
    * `enum class DeviceRole : uint8_t { WRIST_HUB, ANKLE_LEFT, ANKLE_RIGHT };`
    * `enum class FootSide : uint8_t { LEFT = 0, RIGHT = 1 };`
    * `enum class FeedbackColor : uint8_t { ORANGE_BREATH, WHITE_FIXED, BLUE_FLASH, GREEN_FIXED, RED_FLASH, ORANGE_FIXED };`

---

## 3. Dossier `lib/HAL/` : Couche d'Abstraction Matérielle

Le HAL isole complètement le matériel. La logique applicative ne manipule que les interfaces pures (`I...`).

### `lib/HAL/IBoard.h` (Interface Pure)
* **Imports :** `<cstdint>`
* **Méthodes :**
    * `virtual void init() = 0;`
    * `virtual bool isButtonPressed() = 0;`
    * `virtual uint8_t getBatteryLevel() = 0;`
    * `virtual void sleep(uint32_t durationMs) = 0;`

### `lib/HAL/IImu.h` (Interface Pure)
* **Imports :** Aucun.
* **Méthodes :**
    * `virtual bool init() = 0;`
    * `virtual void update() = 0;`
    * `virtual float getAccelerationZ() = 0;` (Axe vertical principal de l'impact)
    * `virtual float getAccelerationMagnitude() = 0;` (Norme $\sqrt{x^2+y^2+z^2}$)

### `lib/HAL/IFeedback.h` (Interface Pure)
* **Imports :** `"../include/Types.h"`
* **Méthodes :**
    * `virtual void setLedPattern(FeedbackColor color) = 0;`
    * `virtual void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) = 0;`
    * `virtual void updateDisplay(SystemState state, float currentAsymmetry) = 0;`

### Sous-dossier `lib/HAL/M5StickCPlus2/`
Implémentations matérielles réelles exploitant la bibliothèque globale `M5Unified`.
* **`M5Board.h/.cpp` :** Implémente `IBoard`. Encapsule `M5.begin()` et `M5.BtnA.wasPressed()`.
* **`M5Imu.h/.cpp` :** Implémente `IImu`. Configure le capteur d'origine (InvenSense) à ±16G. `getAccelerationZ()` retourne `m5.Imu.getGz()`.
* **`M5Feedback.h/.cpp` :** Implémente `IFeedback`. Reçoit les énumérations de couleurs et écrit sur la LED interne SK6812 via protocole RMT/Adafruit NeoPixel interne, génère des tonalités PWM via `M5.Speaker.tone()`, et affiche l'interface graphique sur l'écran TFT (240x135) selon l'état système.

### Sous-dossier `lib/HAL/Mock/`
* **`MockBoard.h/.cpp`, `MockImu.h/.cpp`, `MockFeedback.h/.cpp` :** Implémentations alternatives contenant des variables d'états publiques simples (ex: `bool buttonStateMock`). Permettent de simuler des comportements matériels par injection de données lors des tests sur PC (environnement `native`).

---

## 4. Dossier `lib/Algorithms/` : Traitement Mathématique Pur

**Règle stricte :** Aucun import Arduino/M5 dans ce dossier. C++ natif uniquement.

### `lib/Algorithms/Filters.h`
* **Description :** Fonctions mathématiques inline réutilisables.
* **Imports :** Aucun.
* **Méthodes :**
    * `inline float computeEMA(float current, float previous, float alpha)` $\rightarrow$ Retourne $current \times alpha + previous \times (1.0f - alpha)$.

### `lib/Algorithms/ImpactDetector.h/.cpp`
* **Description :** Détection en temps réel des chocs et extraction du pic maximal de décélération.
* **Attributs privés :**
    * `float _previousVal;`
    * `float _thresholdG;` (Seuil minimal d'activation du choc)
    * `bool _isInsideImpact;`
    * `float _currentPeak;`
* **Méthodes publiques :**
    * `ImpactDetector(float thresholdG);`
    * `bool processSample(float currentSample, float& outPeak);` (Analyse l'échantillon. Si un pic de décélération se termine, retourne `true` et peuple `outPeak`).

### `lib/Algorithms/GaitAnalyzer.h/.cpp`
* **Description :** Analyse comparative de la foulée, calcul de la baseline glissante (calibration) et de l'asymétrie courante.
* **Attributs privés :**
    * `float _leftBaseline;`, `float _rightBaseline;`
    * `uint8_t _calibStepCount;`
    * `float _leftAccumulator;`, `float _rightAccumulator;`
* **Méthodes publiques :**
    * `GaitAnalyzer();`
    * `void resetCalibration();`
    * `bool addCalibrationStep(float peakForce, FootSide side);` (Retourne `true` quand les 30 pas de référence valides sont atteints)
    * `void setBaselines(float left, float right);` (Injection directe pour reprise après pause)
    * `float computeAsymmetry(float leftPeak, float rightPeak);` $\rightarrow$ Formule : $\frac{|Left - Right|}{Max(Left, Right)} \times 100.0$.

---

## 5. Dossier `lib/Core/` : Logique Métier et FSM Transverse

### `lib/Core/IState.h` (Interface Pure du State Pattern)
* **Description :** Interface de base pour le pattern State polymorphe. Chaque état du système hérite de cette interface.
* **Méthodes virtuelles :**
    * `virtual void onEnter(IStateMachine* fsm, IFeedback* ui) = 0;` (Exécuté lors de l'entrée dans l'état)
    * `virtual void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) = 0;` (Boucle principale)
    * `virtual void onExit(IStateMachine* fsm, IFeedback* ui) = 0;` (Exécuté lors de la sortie)
    * `virtual SystemState getStateType() const = 0;` (Retourne le type d'état)

### `lib/Core/IStateMachine.h` (Interface pour les Transitions)
* **Description :** Interface que la FSM expose aux états pour permettre les transitions sécurisées.
* **Méthodes :**
    * `virtual void requestTransition(SystemState target) = 0;` (Un état demande une transition)
    * `virtual SystemState getCurrentState() const = 0;` (Getter de l'état courant)

### `lib/Core/StateMachine.h/.cpp` (Implémentation Concrète)
* **Description :** Moteur de FSM polymorphe basé sur le Design Pattern State (6 classes concrètes : `ReposState`, `DiagnosticState`, `CalibrationState`, `CourseNormalState`, `CourseAlerteState`, `PauseState`).
* **Attributs privés :**
    * `IState* _currentState;` (Pointeur vers l'objet état courant, non un enum)
    * `ReposState* _reposState;` et 5 autres instances... (Pool statique)
    * `bool _transitionRequested;` + `SystemState _requestedState;` (Flags de transition)
* **Méthodes publiques :**
    * `void requestTransition(SystemState target)` (Enregistre une demande de transition)
    * `SystemState getCurrentState() const` (Retourne l'état courant)
    * `void update(IFeedback* ui, bool btnShort, bool btnLong, float asymmetry)` (Exécute la boucle FSM)
* **Avantages du polymorphisme :**
    * Chaque état est une classe isolée → zéro « switch géant »
    * Chaque état gère ses propres LED/buzzer dans `onEnter/onExit`
    * Les transitions sont explicites (pas de spaghetti if/else)
    * Scalable : ajouter un état = créer une classe, implémenter `IState`

### `lib/Core/WristStatesImpl.h/.cpp`
* **Description :** 6 implémentations concrètes de `IState` pour le nœud Poignet.
    * `ReposState` : Veille orange respiration lente
    * `DiagnosticState` : Test blanc, bip
    * `CalibrationState` : Calibration bleue, flash à chaque pas
    * `CourseNormalState` : Vert fixe, aucun problème
    * `CourseAlerteState` : Rouge flash, alerte asymétrie
    * `PauseState` : Orange fixe, suspendu
* Chaque classe implémente la transition bouton-basée (court/long) dans `execute()`
* Les transitions asymétrie-basées (COURSE_NORMAL ↔ COURSE_ALERTE) sont pilotées par `WristApp`

### `lib/Core/AnkleStates.h/.cpp` (Future)
* **Description :** FSM simplifié pour la cheville (à implémenter avec le même pattern).

---

## 6. Dossier `lib/Network/` : Couche de Communication Sans-Fil

### `lib/Network/INetworkManager.h` (Interface Pure)
* **Imports :** `<cstdint>`, `<stddef.h>`
* **Méthodes :**
    * `virtual bool init() = 0;`
    * `virtual bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) = 0;`
    * `virtual void registerReceiveCallback(ReceiveCallback cb) = 0;`
    * `virtual bool getNextMessage(ImpactPayload* outPayload)` (Consomme les messages de la queue)

### `lib/Network/EspNowManager.h/.cpp`
* **Description :** Implémentation ESP-NOW avec **FreeRTOS Queue** pour sécurité multithread.
* **Architecture :**
    * `onReceiveISR()` statique → appelé par ISR du WiFi (thread différent)
    * ISR pousse les `ImpactPayload` dans une `QueueHandle_t` au lieu d'appeler un callback direct
    * `getNextMessage()` → consomme la queue de manière thread-safe dans le contexte app
* **Avantages :**
    * ✅ Aucun risque de data tearing sur `_lastLeftImpact/_lastRightImpact`
    * ✅ Pas de dépendance critique WiFi → app, la queue absorbe les variations
    * ✅ Déterministe : la queue peut contenir jusqu'à 32 messages
* **Attributs privés :**
    * `QueueHandle_t _messageQueue;` (Queue FreeRTOS)
    * `static void onReceiveISR(...)` (Callback ISR-safe)

### `lib/Network/Mock/MockNetwork.h/.cpp`
* **Description :** Permet d'injecter artificiellement des structures `ImpactPayload` de test pour les tests PC natifs.

---

## 7. Dossier `src/` : Points d'Entrée Applicatifs Minimaux

Les fichiers de ce dossier n'effectuent aucun calcul direct. Ils instancient les dépendances matérielles (HAL) et les injectent dans les objets applicatifs.

### `src/ankle/AnkleApp.h/.cpp`
* **Description :** Contrôleur principal s'exécutant sur les nœuds de chevilles.
* **Attributs privés :**
    * `IImu* _imu;`
    * `INetworkManager* _net;`
    * `ImpactDetector _detector;`
* **Méthodes publiques :**
    * `AnkleApp(IImu* imu, INetworkManager* net);`
    * `void setup();` (Initialise l'IMU et le réseau)
    * `void loop();` (Lit l'IMU à 100Hz, transmet la valeur à `_detector`. Si un impact est détecté, remplit la structure `ImpactPayload` et l'envoie immédiatement via le réseau au Poignet).

### `src/ankle/main.cpp`
* **Description :** Fichier d'ancrage hardware compilé uniquement pour `env:ankle_left` et `env:ankle_right`.
* **Contenu :**
    ```cpp
    #include <Arduino.h>
    #include "M5Imu.h"
    #include "EspNowManager.h"
    #include "AnkleApp.h"
    
    M5Imu realImu;
    EspNowManager realNet;
    AnkleApp app(&realImu, &realNet);
    
    void setup() { app.setup(); }
    void loop() { app.loop(); }
    ```

### `src/wrist/WristApp.h/.cpp`
* **Description :** Chef d'orchestre central s'exécutant sur le module Poignet (Hub Maître).
* **Attributs privés :**
    * `IBoard* _board;`, `IFeedback* _ui;`, `INetworkManager* _net;`
    * `StateMachine _fsm;`, `GaitAnalyzer _analyzer;`
    * `float _lastLeftImpact;`, `float _lastRightImpact;`
    * `float _currentAsymmetry;`
* **Méthodes publiques :**
    * `WristApp(IBoard* b, IFeedback* ui, INetworkManager* n);`
    * `void setup();` (Enregistre la fonction de rappel réseau `onDataReceived`)
    * `void loop();` (Traite l'anti-rebond du bouton, calcule la durée d'appui, interroge la FSM via `WristStates::handleWristState`, et rafraîchit l'UI)
    * `void handleIncomingImpact(float peak, uint8_t side);` (Appelée par la fonction de rappel de réception : met à jour le pic correspondant, pousse l'échantillon dans `_analyzer` et recalcule l'asymétrie courante).

### `src/wrist/main.cpp`
* **Description :** Fichier d'ancrage hardware compilé uniquement pour `env:wrist`.
* **Contenu :**
    ```cpp
    #include <Arduino.h>
    #include "M5Board.h"
    #include "M5Feedback.h"
    #include "EspNowManager.h"
    #include "WristApp.h"
    
    M5Board realBoard;
    M5Feedback realUi;
    EspNowManager realNet;
    WristApp app(&realBoard, &realUi, &realNet);
    
    // Passerelle statique obligatoire pour la fonction de rappel du C-API d'ESP-NOW
    void networkCallback(const uint8_t* mac, const uint8_t* data, int len) {
        if(len == sizeof(ImpactPayload)) {
            ImpactPayload* packet = (ImpactPayload*)data;
            app.handleIncomingImpact(packet->peakDeceleration, packet->footSide);
        }
    }
    
    void setup() {
        realNet.registerReceiveCallback(networkCallback);
        app.setup();
    }
    
    void loop() { app.loop(); }
    ```

---

## 8. Dossier `test/` : Automatisation des Bancs de Tests

Exécutable localement sur PC de développement via la commande terminal `pio test -e native`.

### `test/env_native/test_algorithms/test_gait_analyzer.cpp`
* **Description :** Validation unitaire des calculs biomédicaux.
* **Tests implémentés (Framework Unity intégré à PIO) :**
    * `test_perfect_symmetry()` : Injecte 10.0G à gauche et 10.0G à droite. Valide que `computeAsymmetry()` retourne exactement `0.0f`.
    * `test_asymmetry_calculation()` : Injecte 10.0G à gauche et 8.0G à droite. Valide que le retour est exactement de `20.0f` (car $\frac{|10-8|}{10} \times 100 = 20$).
    * `test_calibration_baseline_completion()` : Simule une boucle de 30 appels consécutifs à `addCalibrationStep()` et valide que la fonction retourne `true` précisément au trentième pas.

### `test/env_native/test_core/test_state_machine.cpp`
* **Description :** Validation des transitions logiques de la machine à états.
* **Tests implémentés :**
    * `test_initial_state_is_repos()` : Vérifie la bonne initialisation du système.
    * `test_forbidden_transitions()` : Tente de forcer un passage direct de l'état `REPOS` à l'état `COURSE_NORMAL` sans passer par l'état `CALIBRATION` et valide que le système rejette l'action ou l'interdit conformément aux spécifications du projet.