#ifndef PRINTF
#define PRINTF

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDR 0xB8000

static volatile uint16_t* const VGA_PTR = (volatile uint16_t*)VGA_ADDR;
static int row = 0, col = 0;

static inline uint8_t make_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

static inline void putchar(char c, uint8_t fg, uint8_t bg) {
    uint8_t color = make_color(fg, bg);

    if (c == '\n') {
        col = 0;
        row++;
    } else {
        if (row < VGA_HEIGHT && col < VGA_WIDTH) {
            VGA_PTR[row * VGA_WIDTH + col] = ((uint16_t)color << 8) | c;
            col++;
            if (col >= VGA_WIDTH) {
                col = 0;
                row++;
            }
        }
    }

    if (row >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
            VGA_PTR[i] = VGA_PTR[i + VGA_WIDTH];
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
            VGA_PTR[i] = ((uint16_t)make_color(7, 0) << 8) | ' ';
        row = VGA_HEIGHT - 1;
    }
}

static inline void printf(const char* str, uint8_t fg, uint8_t bg, int x, int y, int use_start) {
    if (use_start) {
        if (x >= 0 && x < VGA_WIDTH) col = x;
        if (y >= 0 && y < VGA_HEIGHT) row = y;
    }

    for (int i = 0; str[i]; i++) {
        putchar(str[i], fg, bg);
    }
}

static inline void fill_bg(uint8_t fg, uint8_t bg) {
    uint8_t color = make_color(fg, bg);
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_PTR[i] = ((uint16_t)color << 8) | ' ';
    row = col = 0;
}


static inline int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}
#endif
