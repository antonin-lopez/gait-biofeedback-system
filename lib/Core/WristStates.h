#pragma once

#include "../../include/Types.h"

class Feedback;

namespace WristStates {
    void handleWristState(SystemState state, Feedback& ui, bool btnShort, bool btnLong, float currentAsymmetry);
}
