//
//  check_clock.cpp
//  Free-Copter
//
//  Created by Master on 7/16/15.
//
//

#include <stdio.h>
#include <time.h>


void go(const char* str, const clockid_t clock) {
    const char* format = "clock: %s\t\t\tres: %ds %dns\n";
    struct timespec tp;
    clock_getres(clock, &tp);
    printf(format, str, tp.tv_sec, tp.tv_nsec);
}

int main (const int argc, const char** argv) {
    go("CLOCK_REALTIME", CLOCK_REALTIME);
    go("CLOCK_REALTIME_COARSE", CLOCK_REALTIME_COARSE);
    go("CLOCK_MONOTONIC", CLOCK_MONOTONIC);
    go("CLOCK_MONOTONIC_COARSE", CLOCK_MONOTONIC_COARSE);
    go("CLOCK_MONOTONIC_RAW", CLOCK_MONOTONIC_RAW);
    go("CLOCK_BOOTTIME", CLOCK_BOOTTIME);
    go("CLOCK_PROCESS_CPUTIME_ID", CLOCK_PROCESS_CPUTIME_ID);
    go("CLOCK_THREAD_CPUTIME_ID", CLOCK_THREAD_CPUTIME_ID);
    return 0;
}
