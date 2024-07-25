#include "DebugLogger.h"

Stream* DebugLogger::s_stream = nullptr;
LogLevel DebugLogger::s_logLevel = LogLevel::INFO;

void DebugLogger::init(Stream& stream, LogLevel level) {
    s_stream = &stream;
    s_logLevel = level;
}

void DebugLogger::setLogLevel(LogLevel level) {
    s_logLevel = level;
}

void DebugLogger::log(LogLevel level, const char* format, va_list args) {
    if (s_stream && level <= s_logLevel) {
        s_stream->print(millis());
        s_stream->print(" [");
        s_stream->print(getLevelString(level));
        s_stream->print("] ");
        s_stream->printf(format, args);
        s_stream->println();
    }
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

void DebugLogger::debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LogLevel::DEBUG, format, args);
    va_end(args);
}

const char* DebugLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::DEBUG: return "DEBUG";
        default:              return "UNKNOWN";
    }
}