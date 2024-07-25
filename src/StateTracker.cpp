#include "StateTracker.h"

SystemState StateTracker::s_currentState = SystemState::INITIALIZING;

void StateTracker::setState(SystemState newState) {
    if (newState != s_currentState) {
        DebugLogger::info("State changed from %s to %s", 
                          getStateString(s_currentState), 
                          getStateString(newState));
        s_currentState = newState;
    }
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