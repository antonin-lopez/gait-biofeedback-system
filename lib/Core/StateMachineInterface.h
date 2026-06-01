#pragma once

#include "../../include/Types.h"

class AppState;

// Contrat de la machine à états (transitions et état courant).
class StateMachineInterface {
public:
    virtual ~StateMachineInterface() = default;
    virtual void requestTransition(AppState* target) = 0;
    virtual SystemState getCurrentState() const = 0;
};
