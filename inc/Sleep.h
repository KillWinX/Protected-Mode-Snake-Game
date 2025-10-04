#ifndef SLEEP
#define SLEEP


static unsigned long long rdtsc() {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)hi << 32) | lo;
}

#define CPU_HZ 3000000000ULL 

static inline void Sleep(unsigned int ms) {
    unsigned long long start = rdtsc();
    unsigned long long ticks_to_wait = (CPU_HZ / 1000) * ms;

    while ((rdtsc() - start) < ticks_to_wait) {
    }
}

#endif