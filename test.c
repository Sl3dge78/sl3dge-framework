#include "sl3dge.h"

#include <windows.h>

int main(const int argc, const char *argv[]) {
    sLogLevel(LOG_LEVEL_LOG);
    sInitPerf();

    const u32 iterations = 1;
    for(u32 i = 0; i < iterations; i++) {
        sBeginTimer("Load");
        sLoadImage("1.png");
        sEndTimer("Load");
    }
    sDumpPerf();
}