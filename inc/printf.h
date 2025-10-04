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

static inline void safe_putchar(char c, uint8_t fg, uint8_t bg) {
    uint8_t color = make_color(fg, bg);

    if (c == '\n') {
        col = 0;
        row++;
    } else {
        if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;   // clamp
        if (col >= VGA_WIDTH)  col = 0;                // clamp

        VGA_PTR[row * VGA_WIDTH + col] = ((uint16_t)color << 8) | c;
        col++;

        if (col >= VGA_WIDTH) {
            col = 0;
            row++;
        }
    }

    // scroll
    if (row >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
            VGA_PTR[i] = VGA_PTR[i + VGA_WIDTH];
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
            VGA_PTR[i] = ((uint16_t)make_color(7, 0) << 8) | ' ';
        row = VGA_HEIGHT - 1;
    }
}

static inline void print_int(int value, uint8_t fg, uint8_t bg) {
    char buffer[12];
    int i = 0;

    if (value == 0) {
        safe_putchar('0', fg, bg);
        return;
    }

    unsigned int v;
    if (value < 0) {
        safe_putchar('-', fg, bg);
        v = (unsigned int)(-(value + 1)) + 1; // avoid INT_MIN overflow
    } else {
        v = (unsigned int)value;
    }

    while (v > 0 && i < sizeof(buffer)) {
        buffer[i++] = '0' + (v % 10);
        v /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        safe_putchar(buffer[j], fg, bg);
    }
}

static inline void printf_Ex(const char* label, int value,
                             uint8_t fg, uint8_t bg,
                             int x, int y, int use_start) {
    // start position only if requested
    if (use_start) {
        if (x >= 0 && x < VGA_WIDTH)  col = x;
        if (y >= 0 && y < VGA_HEIGHT) row = y;
    }

    for (int i = 0; label[i]; i++) {
        safe_putchar(label[i], fg, bg);
    }

    print_int(value, fg, bg);
}


static inline int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}
#endif
