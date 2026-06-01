# Architecture & Conventions d'Équipe — Système de Biofeedback

**Équipe :** 4 développeurs | **Objectif :** Système professionnel, scalable, testable  
**Date de création :** 2026-06-01 | **Statut :** Refactoring en cours

---

## 1. Principes Directeurs

### 1.1 Scalabilité et Maintenabilité
- **Aucun hardcoding** : Tous les seuils et MAC vont en `AppConfig.h` ou NVS
- **HAL stricte** : Zéro dépendance Arduino/M5 en dehors de `lib/HAL/M5StickCPlus2/`
- **Testabilité** : Tous les algorithmes vivent en `lib/Algorithms/` (C++ pur, pas d'Arduino)
- **Design Patterns** : On utilise le vrai polymorphisme, pas de faux switch géants

### 1.2 Concurrence et Sécurité Mémoire
- **Aucun accès concurrent sans protection** : FreeRTOS Queue pour callbacks réseau
- **Mutex** où nécessaire : Données partagées entre WiFi task et app task
- **Endianness** : Network Byte Order systématiquement pour compatibilité future

### 1.3 Qualité de Code
- **C++17 minimum** : `-std=gnu++17` forcé partout (`platformio.ini`)
- **Pas de mock de base de données en tests** : Tests d'intégration réels si possible
- **Commits atomiques** : Une feature = un commit avec message clair

---

## 2. Architecture Cible (Après Refactor)

### 2.1 État Actuel vs Cible

| Aspect | Actuel | Cible |
|--------|--------|-------|
| **State Machine** | Enum + grand switch | Polymorphe (classes IState) ou pointeurs fonctions |
| **Callback réseau** | Direct dans app | Queue FreeRTOS (message passing) |
| **IMU** | Polling pur | Polling + FIFO hardware (futur) |
| **Pairing** | Broadcast blind | MAC stockés NVS + filtrage |
| **Bouton** | Logique custom | M5Unified native (`wasClicked/wasHold`) |
| **Timestamps** | Toujours 0 | `millis()` systématiquement |

### 2.2 Refactor Phasé

```
Phase 1 (CRITIQUE)
├─ State Pattern polymorphe (remplacer switch)
├─ FreeRTOS Queue pour ESP-NOW callback
└─ Mutex sur données partagées _lastLeftImpact/_lastRightImpact

Phase 2 (IMPORTANT)  
├─ Pairing/MAC binding en NVS
├─ Watchdog connexion (2s timeout)
└─ Timestamps millis() remplis

Phase 3 (NETTOYAGE)
├─ IMU FIFO si disponible
├─ wasClicked/wasHold native M5Unified
└─ NVS thresholds (éditable sans recompile)

Phase 4 (OPTIONNEL)
├─ Compression timestamps (delta encoding)
└─ Métriques télémétrie
```

---

## 3. Conventions de Codage

### 3.1 Nommage et Style
- **Namespaces** : Oui si groupe logique (`WristStates::`, `Algorithms::`)
- **Private members** : Préfixe `_` (ex: `_currentState`)
- **Constants** : `constexpr` en capitales (ex: `ASYMMETRY_THRESHOLD`)
- **Enums** : `enum class` + type explicite (ex: `SystemState : uint8_t`)
- **Methods** : camelCase, getters sans `get` optionnel (ex: `currentState()` vs `getCurrentState()`)

### 3.2 Allocation Mémoire
- **Zéro `new/delete` en production** : Réserver au setup()
- **Stack par défaut** : Petites structs (ImpactPayload ~12 bytes)
- **Heap critique uniquement** : Queues FreeRTOS, buffers long-lived
- **RAII** : Destructeurs clairs pour cleanup

### 3.3 Gestion d'Erreurs
- **Retour codes** : `bool` pour succès/échec simple
- **Pas d'exceptions** : Embarqué = déterministe
- **Assertions** : `assert()` en debug, compilé out en release
- **Validation aux frontières** : User input + API réseau, pas interne

### 3.4 Fichiers et Organisation
```
include/
├─ AppConfig.h          (tous les seuils)
├─ Protocol.h           (#pragma pack, structures réseau)
└─ Types.h             (enums, types forts)

lib/
├─ Algorithms/         (C++ pur, aucun Arduino)
├─ Core/               (FSM, business logic)
├─ HAL/                (interfaces + M5StickCPlus2/ + Mock/)
└─ Network/            (ESP-NOW, + Mock/)

src/
├─ wrist/              (WristApp + main, TARGET_WRIST)
└─ ankle/              (AnkleApp + main, TARGET_ANKLE)

test/
└─ env_native/         (tests PC via pio test -e native)
```

---

## 4. Flux de Travail en Équipe

### 4.1 Branches et Commits
```bash
# Feature branch (one per person)
git checkout -b feat/phase1-state-pattern
git commit -m "refactor: implement polymorphic State Pattern with IState interface"
git push origin feat/phase1-state-pattern

# Lors du merge (rebase + squash ou merge commit?)
→ Choisir en équipe selon convention
```

### 4.2 Assignation Phase 1
**Personne A** → `lib/Core/IState.h` + état classes  
**Personne B** → `lib/Core/StateMachine.cpp` (transition engine)  
**Personne C** → FreeRTOS Queue intégration  
**Personne D** (chef) → Coordination + revue + merge

### 4.3 Review & Test
- Avant merge : `pio test -e native` doit passer
- Review par au moins 1 autre dev (pas l'auteur)
- Tests hardware sur M5Stick après merge

---

## 5. Points d'Entrée Clés pour le Refactor

### 5.1 Fichiers à Toucher Phase 1

| Fichier | Raison | Impact |
|---------|--------|--------|
| `lib/Core/IState.h` (NEW) | Base polymorphe | 🔴 Critique |
| `lib/Core/StateMachine.cpp` | Moteur transitions | 🔴 Critique |
| `lib/Network/EspNowManager.h` | Queue FreeRTOS | 🔴 Critique |
| `src/wrist/WristApp.cpp` | Consommer la Queue | 🔴 Critique |
| `src/wrist/main.cpp` | Callback sécurisé | 🔴 Critique |
| `platformio.ini` | C++17 homogène | 🟡 Important |

### 5.2 Fichiers Intouchés Phase 1
- `lib/Algorithms/*` (aucun changement nécessaire)
- `test/` (sauf si tests d'intégration Queue)
- `src/ankle/*` (attendre sync après wrist stabilisé)

---

## 6. Stratégies de Transition

### 6.1 State Pattern — Approche Recommandée

**Option A : Classes polymorphes (recommandée pour scalabilité)**
```cpp
class IState {
    virtual void onEnter(WristApp* context) = 0;
    virtual void execute(WristApp* context) = 0;
    virtual void onExit(WristApp* context) = 0;
};

class ReposState : public IState { /*...*/ };
class DiagnosticState : public IState { /*...*/ };
// ... 6 états total

// Dans StateMachine
void StateMachine::transitionTo(IState* newState) {
    if (_currentState) _currentState->onExit(context);
    _currentState = newState;
    _currentState->onEnter(context);
}
```

**Option B : Pointeurs de fonctions (plus léger)**
```cpp
using StateHandler = void (*)(WristApp*, IFeedback*);
// Nécessite une table de transitions statique
```

→ **Décision équipe** : Aller avec Option A (Option B plus tard si RAM critique)

### 6.2 FreeRTOS Queue — Template
```cpp
// Dans EspNowManager
static QueueHandle_t impactQueue = nullptr;

void receiveCallback(const uint8_t* mac, const uint8_t* data, int len) {
    if (len == sizeof(ImpactPayload)) {
        ImpactPayload* pkt = (ImpactPayload*)data;
        xQueueSendFromISR(impactQueue, pkt, nullptr);
    }
}

// Dans WristApp::loop()
ImpactPayload incoming;
if (xQueueReceive(impactQueue, &incoming, 0) == pdTRUE) {
    handleIncomingImpact(incoming.peakDeceleration, incoming.footSide);
}
```

---

## 7. Définitions d'Objets Terminés

### 7.1 Phase 1 — TERMINÉ quand :
- ✅ IState interface + 6 implémentations compilent
- ✅ Transitions valident les états autorisés (pas direct REPOS → COURSE)
- ✅ Queue FreeRTOS créée dans setup, callback l'utilise
- ✅ Aucun accès concurrent à `_lastLeftImpact/_lastRightImpact`
- ✅ `pio test -e native` : tous les tests passent
- ✅ Code compilé sur 3 targets (wrist, ankle_left, ankle_right) sans erreur
- ✅ Hardware M5Stick : capteurs et LEDs réagissent (visual check suffisant)

### 7.2 Test Checklist Phase 1
```
[ ] État initial = REPOS
[ ] Bouton court : REPOS → DIAGNOSTIC
[ ] Bouton long : REPOS/DIAGNOSTIC → CALIBRATION
[ ] 30 pas en CALIBRATION → auto COURSE_NORMAL
[ ] Impact > seuil en COURSE_NORMAL → COURSE_ALERTE
[ ] Pas de crash multithread (LED stable après 5 impacts rapides)
```

---

## 8. Dépendances Externes & Versions

```
[platformio]
lib_deps = 
    m5stack/M5Unified @ ^0.1.12
    # (Arduino + FreeRTOS inclus par espressif32)

[env:_esp32_base]
build_flags = 
    -std=gnu++17
    -Wall -Wextra
```

- **FreeRTOS** : Inclus dans Arduino framework (pas d'install)
- **C++ Standard** : Minimum C++17 (pour `std::optional`, `std::variant`)

---

## 9. Ressources de Référence

- **FreeRTOS Queues** : https://www.freertos.org/xQueueCreate.html
- **ESP-NOW** : https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html
- **M5Unified** : https://github.com/m5stack/M5Unified
- **State Pattern** : https://refactoring.guru/design-patterns/state

---

## 10. Changelog & Décisions

| Date | Décision | Responsable |
|------|----------|-------------|
| 2026-06-01 | Phase 1 kickoff, Polymorphic State Pattern | Antonin |
| TBD | Phase 2 planning | TBD |

---

**Version du doc :** 1.0  
**Prochain review :** Après Phase 1 complète
