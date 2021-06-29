#ifndef TEST_H
#define TEST_H

#include "types.h"
#include "logging.h"

#define TEST_EQUALS(val1, val2)                                                                    \
    {                                                                                              \
        if(val1 == val2)                                                                           \
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
    state = (TestState){};
}

void TEST_END() {
    sLog("Tests completed");
    sLog("%d/%d tests are ok", state.test_ok, state.test_count);
    if(state.test_nok > 0) {
        sError("%d/%d tests failed", state.test_nok, state.test_count);
    }
}

internal void TestSuccess(const char *file, const u32 line) {
    sLog("OK  : %s:%d", file, line);
    state.test_count++;
    state.test_ok++;
}

internal void TestFailure(const char *file, const u32 line) {
    sError("FAIL: %s:%d", file, line);
    state.test_count++;
    state.test_nok++;
}

#endif