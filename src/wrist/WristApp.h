#ifndef WRISTAPP_H
#define WRISTAPP_H

#include "../../lib/HAL/IBoard.h"
#include "../../lib/HAL/IFeedback.h"
#include "../../lib/Network/INetworkManager.h"
#include "../../lib/Core/StateMachine.h"
#include "../../lib/Core/WristStatesImpl.h"
#include "../../lib/Algorithms/GaitAnalyzer.h"
#include "../../include/Types.h"
#include <cstdint>

class WristApp {
private:
    IBoard* _board;
    IFeedback* _ui;
    INetworkManager* _net;

    ReposState _reposState;
    DiagnosticState _diagnosticState;
    CalibrationState _calibrationState;
    CourseNormalState _courseNormalState;
    CourseAlerteState _courseAlerteState;
    PauseState _pauseState;

    StateMachine _fsm;
    GaitAnalyzer _analyzer;
    float _lastLeftImpact;
    float _lastRightImpact;
    uint32_t _lastLeftImpactTime;
    uint32_t _lastRightImpactTime;
    float _currentAsymmetry;
    uint32_t _lastButtonTime;
    bool _buttonPressed;

    bool _isImpactValid(uint32_t impactTime) const;
    bool _isAbsoluteThresholdMet(float left, float right) const;

public:
    WristApp(IBoard* b, IFeedback* ui, INetworkManager* n);
    void setup();
    void loop();
    void handleIncomingImpact(float peak, uint8_t side);
};

#endif // WRISTAPP_H


