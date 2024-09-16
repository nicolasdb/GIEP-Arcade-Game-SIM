#pragma once
#include "DebugLogger.h"

enum class SystemState {
    INITIALIZING,
    MATRIX_READY,
    SCENE_LOADED,
    RUNNING,
    ERROR
};

class StateTracker {
public:
    static void setState(SystemState newState);
    static SystemState getState();
    static const char* getCurrentStateString();

private:
    static const char* getStateString(SystemState state);
    static SystemState s_currentState;
};