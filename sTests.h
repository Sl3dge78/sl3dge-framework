#pragma once

#include "sTypes.h"
#include "sLogging.h"

#define TEST_EQUALS(v1, v2, fmt)                                                                   \
    {                                                                                              \
        if(v1 == v2) {                                                                             \
            TestSuccess(__FILE__, __LINE__);                                                       \
            sLogSetColor(LOG_COLOR_GREEN);                                                         \
            sLogOutputLine(0, fmt " == " fmt, v1, v2);                                             \
        } else {                                                                                   \
            TestFailure(__FILE__, __LINE__);                                                       \
            sLogSetColor(LOG_COLOR_RED);                                                           \
            sLogOutputLine(0, fmt " != " fmt, v1, v2);                                             \
        }                                                                                          \
    }

#define TEST_PTR(ptr)                                                                              \
    {                                                                                              \
        if(ptr)                                                                                    \
            TestSuccess(__FILE__, __LINE__);                                                       \
        else                                                                                       \
            TestFailure(__FILE__, __LINE__);                                                       \
    }

typedef struct {
    u32 test_count;
    u32 test_ok;
    u32 test_nok;
} TestState;

global TestState state;

void TEST_BEGIN() {
    state = (TestState){0};
    sLog("TEST BEGIN");
    sLog("GOT -- EXPECTED");
}

void TEST_END() {
    sLog("TESTS COMPLETED");
    if(state.test_ok == state.test_count) {
        sLog("ALL TESTS ARE OK");
    } else {
        sLog("OK   %d/%d", state.test_ok, state.test_count);
        sError("FAIL %d/%d", state.test_nok, state.test_count);
    }
}

internal void TestSuccess(const char *file, const u32 line) {
    sTrace("OK  : %s:%d", file, line);
    state.test_count++;
    state.test_ok++;
}

internal void TestFailure(const char *file, const u32 line) {
    sError("FAIL: %s:%d", file, line);
    state.test_count++;
    state.test_nok++;
}
