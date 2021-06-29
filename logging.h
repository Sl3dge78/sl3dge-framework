#ifndef LOGGING_H
#define LOGGING_H

#include "types.h"
#include <stdarg.h>
#include <stdio.h>

#define MAX_LOG_LENGTH 256

typedef void LogCallback_t(const char *message);
typedef LogCallback_t *PFN_LogCallback;
LogCallback_t DefaultLog;

global PFN_LogCallback callback = &DefaultLog;

void DefaultLog(const char *message) {
    printf("%s", message);
}

void sLogSetCallback(PFN_LogCallback cb) {
    callback = cb;
}

// Outputs string format to console. Adds a new line.
void sLog(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[MAX_LOG_LENGTH];
    vsnprintf_s(buffer, MAX_LOG_LENGTH, MAX_LOG_LENGTH, fmt, args);
    va_end(args);

    const u32 length = strlen(buffer);
    strncat_s(buffer, MAX_LOG_LENGTH, "\n\0", MAX_LOG_LENGTH);

    callback(buffer);
}

#endif