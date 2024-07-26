#include "StateTracker.h"
#include <string.h>

SystemState StateTracker::s_currentState = SystemState::INITIALIZING;

void StateTracker::setState(SystemState newState) {
    const char* oldStateStr = getStateString(s_currentState);
    const char* newStateStr = getStateString(newState);
    
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "State changed from %s to %s", oldStateStr, newStateStr);
    DebugLogger::info(buffer);
    
    s_currentState = newState;
}

SystemState StateTracker::getState() {
    return s_currentState;
}

const char* StateTracker::getCurrentStateString() {
    return getStateString(s_currentState);
}

const char* StateTracker::getStateString(SystemState state) {
    switch (state) {
        case SystemState::INITIALIZING:  return "INITIALIZING";
        case SystemState::MATRIX_READY:  return "MATRIX_READY";
        case SystemState::SCENE_LOADED:  return "SCENE_LOADED";
        case SystemState::RUNNING:       return "RUNNING";
        case SystemState::ERROR:         return "ERROR";
        default:                         return "UNKNOWN";
    }
}