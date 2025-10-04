/// Includes 

#include "inc/printf.h"
#include "inc/io.h"
#include "inc/halt_cpu.h"
#include "inc/Sleep.h"
#include "inc/scancodes.h"

// Global variables for our Snake game
typedef struct {
    int x, y;
} Point;

typedef enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
} Direction;

Point snake[100];
int snake_length = 3;
Point food;
int gameOver = 0;
char direction = RIGHT;

void draw_snake() {
    fill_bg(0, 0); // Fill the background with black.
    
    printf("@", 4, 0, food.x, food.y, 1); // Draw the food at (x,y).
    printf("O", 5, 0, snake[0].x, snake[0].y, 1); // Draw the snake head.
    
    for (int i = 1; i < snake_length; i++) { // Loop through the snake length for the tail.
        printf("o", 5, 0, snake[i].x, snake[i].y, 2); // Draw the tail.
    }
}

void move_snake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1]; 
    } // Move the tail by shifting each segment.

    switch(direction) {
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    } // Move the snake head based on direction.
}

void hand_key() {
    while (!gameOver) { // Loop until game over
        uint8_t sc = inb(0x60); // Read the keyboard
        if (!(sc & 0x80)) {  // Ignore key releases
            if (sc == W && direction != DOWN) direction = UP; 
            else if (sc == S && direction != UP) direction = DOWN; 
            else if (sc == A && direction != RIGHT) direction = LEFT; 
            else if (sc == D && direction != LEFT) direction = RIGHT; 
        }

        move_snake(); // Update snake position

        // Food logic
        if (snake[0].x == food.x && snake[0].y == food.y) {  
            snake_length++; // Increase snake length
            // Food respawn (simple PRNG with PIT seed)
            food.x = (food.x + 13) % VGA_WIDTH;    
            food.y = (food.y + 7) % VGA_HEIGHT;  
        }
    
        // Collision (Game Over) logic
        // Wall collision detection
        if (snake[0].x < 0 || snake[0].x >= VGA_WIDTH || 
            snake[0].y < 0 || snake[0].y >= VGA_HEIGHT) {
            gameOver++;
            break;
        }
        
        // Self collision detection
        for (int i = 1; i < snake_length; i++) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) { 
                gameOver++; 
                break; 
            }
        }

        draw_snake(); 
        Sleep(100); // Game speed
    }
}

void logic() {
    // Disable the VGA cursor
    outb(0x3D4, 0x0A);  
    outb(0x3D5, 0x20);

    fill_bg(0,0); // Main menu background

    printf("SNAKE GAME MADE BY KILLWINX\n", 2, 0, 35 - 8, 10, 1); 
    printf("Press [ENTER] to start", 7, 0, 30, 12, 2);
    
    // Wait for ENTER to start
    while (1) {
        uint8_t sc = inb(0x60); 
        if (sc == ENTER) { 
            break; 
        }
        Sleep(10); 
    }

    fill_bg(0,0); // Game background

    // Snake setup
    snake_length = 3;  
    snake[0].x = 40;
    snake[0].y = 12;
    snake[1].x = 39;
    snake[1].y = 12;
    snake[2].x = 38;
    snake[2].y = 12;
    
    // Food setup
    food.x = 60;
    food.y = 12;
     
    hand_key(); // Start main game loop

    // Game Over screen
    printf("GAME OVER", 4, 0, 36, 12, 1);  
    Sleep(3000000); 
}

void kmain() {
   logic(); 
   halt(); // Halt the CPU
}