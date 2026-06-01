#include "WristApp.h"
#include "../../lib/Core/WristStates.h"
#include "../../include/AppConfig.h"

WristApp::WristApp(IBoard* b, IFeedback* ui, INetworkManager* n)
    : _board(b), _ui(ui), _net(n), _lastLeftImpact(0.0f), _lastRightImpact(0.0f),
      _currentAsymmetry(0.0f), _lastButtonTime(0), _buttonPressed(false) {}

void WristApp::setup() {
    if (_board) _board->init();
    if (_net) _net->init();
}

void WristApp::loop() {
    if (!_board || !_ui) return;

    // Simulation de lecture du bouton
    bool btnShort = false;
    bool btnLong = false;

    if (_board->isButtonPressed()) {
        if (!_buttonPressed) {
            _buttonPressed = true;
            _lastButtonTime = 0; // Simulé
        }
    } else {
        if (_buttonPressed) {
            _buttonPressed = false;
            btnShort = true; // Simplification
        }
    }

    WristStates::handleWristState(_fsm.getCurrentState(), _ui, btnShort, btnLong, _currentAsymmetry);
}

void WristApp::handleIncomingImpact(float peak, uint8_t side) {
    if (side == (uint8_t)FootSide::LEFT) {
        _lastLeftImpact = peak;
    } else {
        _lastRightImpact = peak;
    }

    if (_lastLeftImpact > 0.0f && _lastRightImpact > 0.0f) {
        _currentAsymmetry = _analyzer.computeAsymmetry(_lastLeftImpact, _lastRightImpact);
    }
}
