#ifndef WRISTSTATES_H
#define WRISTSTATES_H

#include "../../include/Types.h"
#include "../HAL/IFeedback.h"

namespace WristStates {
    void handleWristState(SystemState state, IFeedback* ui, bool btnShort,
                         bool btnLong, float currentAsymmetry);
}

#endif // WRISTSTATES_H

