/*
 * Created by Ed Fillingham on 24/04/2026.
*/

#ifndef HEARTLEDMATRIX_WATCHDOG_H
#define HEARTLEDMATRIX_WATCHDOG_H

void init_watchdog() {
    // 1. Enable write access to IWDG registers
    IWDG->CTLR = 0x5555;

    // 2. Set prescaler.
    // 40kHz / 64 = 625Hz (1.6ms per tick)
    IWDG->PSCR = 0x04;

    // 3. Set reload value.
    // 1250 * 1.6ms = ~2000ms (2 seconds)
    IWDG->RLDR = 1250;

    // 4. Start the dog
    IWDG->CTLR = 0xCCCC;
}

inline void feed_watchdog() {
    // Reload the counter with the value in RLDR
    IWDG->CTLR = 0xAAAA;
}

#endif //HEARTLEDMATRIX_WATCHDOG_H