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

static inline void itoa(int value, char* str) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;
    int is_negative = 0;

    // Handle zero case
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    // Handle negative numbers
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    // Convert number to string (in reverse)
    while (value != 0) {
        tmp_value = value;
        value /= 10;
        *ptr++ = '0' + (tmp_value - value * 10);
    }

    // Add negative sign if needed
    if (is_negative) {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

// Simple printf without variable arguments - use specific functions instead
static inline void printf(const char* str, uint8_t fg, uint8_t bg, int x, int y, int use_start) {
    if (use_start) {
        if (x >= 0 && x < VGA_WIDTH) col = x;
        if (y >= 0 && y < VGA_HEIGHT) row = y;
    }

    for (int i = 0; str[i]; i++) {
        putchar(str[i], fg, bg);
    }
}

static inline void printf_int(int value, uint8_t fg, uint8_t bg, int x, int y, int use_start) {
    if (use_start) {
        if (x >= 0 && x < VGA_WIDTH) col = x;
        if (y >= 0 && y < VGA_HEIGHT) row = y;
    }

    char buffer[32];
    itoa(value, buffer);
    
    for (int i = 0; buffer[i]; i++) {
        putchar(buffer[i], fg, bg);
    }
}

// Combined string and integer (for "Value: %d" style output)
static inline void printf_str_int(const char* str, int value, uint8_t fg, uint8_t bg, int x, int y, int use_start) {
    if (use_start) {
        if (x >= 0 && x < VGA_WIDTH) col = x;
        if (y >= 0 && y < VGA_HEIGHT) row = y;
    }

    for (int i = 0; str[i]; i++) {
        if (str[i] == '%' && str[i + 1] == 'd') {
            char buffer[32];
            itoa(value, buffer);
            
            for (int j = 0; buffer[j]; j++) {
                putchar(buffer[j], fg, bg);
            }
            i++; // Skip the 'd'
        } else {
            putchar(str[i], fg, bg);
        }
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