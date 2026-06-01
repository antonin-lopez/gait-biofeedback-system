#include "WristApp.h"
#include "../../include/AppConfig.h"
#include "../../include/Protocol.h"
#include <Arduino.h>
#include <cmath>

WristApp::WristApp(IBoard* b, IFeedback* ui, INetworkManager* n)
    : _board(b), _ui(ui), _net(n),
      _fsm(_reposState, _diagnosticState, _calibrationState, _courseNormalState, _courseAlerteState, _pauseState),
      _lastLeftImpact(0.0f), _lastRightImpact(0.0f),
      _lastLeftImpactTime(0), _lastRightImpactTime(0), _currentAsymmetry(0.0f),
      _lastButtonTime(0), _buttonPressed(false) {}

void WristApp::setup() {
    if (_board) _board->init();
    if (_net) _net->init();
}

bool WristApp::_isImpactValid(uint32_t impactTime) const {
    uint32_t now = millis();
    return (now - impactTime) < IMPACT_TIMEOUT_MS;
}

bool WristApp::_isAbsoluteThresholdMet(float left, float right) const {
    const float MIN_IMPACT_FORCE_G = 3.0f;
    return (left > MIN_IMPACT_FORCE_G && right > MIN_IMPACT_FORCE_G);
}

void WristApp::loop() {
    if (!_board || !_ui || !_net) return;

    // ── Consommer les impacts reçus de la queue (sécurisé en multithread) ──
    ImpactPayload incoming;
    while (_net->getNextMessage(&incoming)) {
        handleIncomingImpact(incoming.peakDeceleration, incoming.footSide);
    }

    // ── Détection des actions bouton (court/long) ──
    bool btnShort = false;
    bool btnLong = false;

    if (_board->isButtonPressed()) {
        if (!_buttonPressed) {
            _buttonPressed = true;
            _lastButtonTime = 0;
        }
    } else {
        if (_buttonPressed) {
            _buttonPressed = false;
            btnShort = true;
        }
    }

    // ── Valider les impacts (timeouts) ──
    if (!_isImpactValid(_lastLeftImpactTime)) {
        _lastLeftImpact = 0.0f;
    }
    if (!_isImpactValid(_lastRightImpactTime)) {
        _lastRightImpact = 0.0f;
    }

    // ── Recalculer asymétrie si seuil absolu atteint ──
    if (_isAbsoluteThresholdMet(_lastLeftImpact, _lastRightImpact)) {
        _currentAsymmetry = _analyzer.computeAsymmetry(_lastLeftImpact, _lastRightImpact);
    } else {
        _currentAsymmetry = 0.0f;
    }

    // ── Transitions asymétrie-basées (avant mise à jour FSM) ──
    SystemState currentState = _fsm.getCurrentState();
    if (currentState == SystemState::COURSE_NORMAL && _currentAsymmetry > ASYMMETRY_THRESHOLD) {
        _fsm.requestTransition(SystemState::COURSE_ALERTE);
    } else if (currentState == SystemState::COURSE_ALERTE && _currentAsymmetry <= (ASYMMETRY_THRESHOLD * 0.95f)) {
        _fsm.requestTransition(SystemState::COURSE_NORMAL);
    }

    // ── Mise à jour de la FSM (traite les transitions, execute(), etc.) ──
    _fsm.update(_ui, btnShort, btnLong, _currentAsymmetry);
}

void WristApp::handleIncomingImpact(float peak, uint8_t side) {
    if (side == (uint8_t)FootSide::LEFT) {
        _lastLeftImpact = peak;
        _lastLeftImpactTime = millis();
    } else {
        _lastRightImpact = peak;
        _lastRightImpactTime = millis();
    }
}


