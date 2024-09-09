#include "DebugLogger.h"

Stream* DebugLogger::s_stream = nullptr;
LogLevel DebugLogger::s_logLevel = LogLevel::CRITICAL;

void DebugLogger::init(Stream& stream, LogLevel level) {
    s_stream = &stream;
    s_logLevel = level;
}

void DebugLogger::setLogLevel(LogLevel level) {
    s_logLevel = level;
}

void DebugLogger::log(LogLevel level, const char* format, va_list args) {
    if (s_stream && level <= s_logLevel) {
        char buffer[256];
        unsigned long timestamp = millis();
        int prefixLen = snprintf(buffer, sizeof(buffer), "%lu [%s] ", timestamp, getLevelString(level));
        vsnprintf(buffer + prefixLen, sizeof(buffer) - prefixLen, format, args);
        s_stream->println(buffer);
    }
}

void DebugLogger::critical(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::CRITICAL, format, args);
    va_end(args);
}

void DebugLogger::error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::ERROR, format, args);
    va_end(args);
}

void DebugLogger::warn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::WARN, format, args);
    va_end(args);
}

void DebugLogger::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::INFO, format, args);
    va_end(args);
}

void DebugLogger::gameState(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::GAME_STATE, format, args);
    va_end(args);
}

void DebugLogger::debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, format, args);
    va_end(args);
}

const char* DebugLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::CRITICAL:   return "CRITICAL";
        case LogLevel::ERROR:      return "ERROR";
        case LogLevel::WARN:       return "WARN";
        case LogLevel::INFO:       return "INFO";
        case LogLevel::GAME_STATE: return "GAME_STATE";
        case LogLevel::DEBUG:      return "DEBUG";
        default:                   return "UNKNOWN";
    }
}