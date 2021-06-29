#ifndef LOGGING_H
#define LOGGING_H

#include "types.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_LOG_LENGTH 256

#ifdef DEBUG
#define sTrace(message, ...) LogOutput(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#define sLog(message, ...) LogOutput(LOG_LEVEL_LOG, message, ##__VA_ARGS__)
#define sWarn(message, ...) LogOutput(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#define sError(message, ...) LogOutput(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#else
#define sTrace(message, ...)
#define sLog(message, ...)
#define sWarn(message, ...)
#define sError(message, ...)
#endif

enum LogLevel { LOG_LEVEL_TRACE, LOG_LEVEL_LOG, LOG_LEVEL_WARN, LOG_LEVEL_ERROR };

typedef void LogCallback_t(const char *message, const u8 level);
typedef LogCallback_t *PFN_LogCallback;
LogCallback_t DefaultLog;

global PFN_LogCallback callback = &DefaultLog;

void DefaultLog(const char *message, const u8 level) {
    switch(level) {
    case LOG_LEVEL_ERROR: printf("\033[0;31m"); break;
    case LOG_LEVEL_WARN: printf("\033[0;33m"); break;
    case LOG_LEVEL_LOG: printf("\033[0m"); break;
    case LOG_LEVEL_TRACE: printf("\033[1;30m"); break;
    }
    printf("%s", message);
    printf("\033[0m");
}

void sLogSetCallback(PFN_LogCallback cb) {
    callback = cb;
}

// Outputs string format to console. Adds a new line.
void LogOutput(u8 level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[MAX_LOG_LENGTH];
    vsnprintf_s(buffer, MAX_LOG_LENGTH, MAX_LOG_LENGTH, fmt, args);
    va_end(args);

    const u32 length = strlen(buffer);
    strncat_s(buffer, MAX_LOG_LENGTH, "\n\0", MAX_LOG_LENGTH);

    callback(buffer, level);
}

#endif