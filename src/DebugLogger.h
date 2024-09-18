#pragma once
#include <Arduino.h>

enum class LogLevel {
    NONE,
    CRITICAL,
    ERROR,
    WARN,
    INFO,
    GAME_STATE,
    DEBUG
};

class DebugLogger {
public:
    static void init(Stream& stream, LogLevel level = LogLevel::CRITICAL);
    static void setLogLevel(LogLevel level);

    static void critical(const char* format, ...);
    static void error(const char* format, ...);
    static void warn(const char* format, ...);
    static void info(const char* format, ...);
    static void gameState(const char* format, ...);
    static void debug(const char* format, ...);

private:
    static void log(LogLevel level, const char* format, va_list args);
    static const char* getLevelString(LogLevel level);

    static Stream* s_stream;
    static LogLevel s_logLevel;
};