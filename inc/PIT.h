#ifndef PIT
#define PIT

#include "io.h"

volatile unsigned long timer_ticks = 0;

static inline void pit_handler() {
    timer_ticks++;
    outb(0x20, 0x20);
}

static inline void pit_init(int hz) {
    int divisor = 1193180 / hz;    
    outb(0x43, 0x36);              
    outb(0x40, divisor & 0xFF);   
    outb(0x40, divisor >> 8);     
}

#endif