#ifndef HALT
#define HALT

static inline void halt() {
        for (;;) {
        __asm__ __volatile__("hlt"); 
    }
}

#endif