#include "sl3dge.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(const int argc, const char *argv[]) {
    sLogLevel(LOG_LEVEL_LOG);
    sInitPerf();

    const u32 iterations = 10;
    for(u32 i = 0; i < iterations; i++) {
        sBeginTimer("Load");
        sLoadImage("1.png");
        sEndTimer("Load");
    }

    sDumpPerf();
}