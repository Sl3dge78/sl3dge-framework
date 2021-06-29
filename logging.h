#ifndef LOGGING_H
#define LOGGING_H

#include "types.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_LOG_LENGTH 256

#define sTrace(message, ...) LogOutput(0, message, ##__VA_ARGS__)
#define sLog(message, ...) LogOutput(1, message, ##__VA_ARGS__)
#define sWarn(message, ...) LogOutput(2, message, ##__VA_ARGS__)
#define sError(message, ...) LogOutput(3, message, ##__VA_ARGS__);

typedef void LogCallback_t(const char *message, const u8 level);
typedef LogCallback_t *PFN_LogCallback;
LogCallback_t DefaultLog;

global PFN_LogCallback callback = &DefaultLog;

void DefaultLog(const char *message, const u8 level) {
    printf("%s", message);
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