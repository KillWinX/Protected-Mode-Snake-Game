#ifndef RAND
#define RAND 

static unsigned int seed = 1;

static inline unsigned int rand() {
     seed = (1103515245u * seed + 12345u);
     return (seed >> 16) & 0x7FFF;           
}

static inline void srand(unsigned int s) {
    seed = s;
}


#endif