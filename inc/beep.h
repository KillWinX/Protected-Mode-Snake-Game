#include "PIT.h"
#include "io.h"

#define PIT_CTRL 0x43
#define PIT_DATA 0x42
#define SPEAKER  0x61
#define PIT_FREQ 1193180

static inline void beep(unsigned int freq, unsigned int ms) {
    unsigned int divisor = PIT_FREQ / freq;

    outb(PIT_CTRL, 0xB6);         
    outb(PIT_DATA, divisor & 0xFF);
    outb(PIT_DATA, (divisor >> 8) & 0xFF);

    unsigned char tmp = inb(SPEAKER);
    outb(SPEAKER, tmp | 3);    

    for(unsigned int i=0;i<ms*1000;i++) __asm__ __volatile__("nop");

    tmp = inb(SPEAKER);
    outb(SPEAKER, tmp & 0xFC);
}