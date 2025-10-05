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
int score = 0;

char *buffer;

void draw_snake() {
    fill_bg(1, 1); // Fill the background with black.
    
    printf("@", 4, 1, food.x, food.y, 1); // Draw the food at (x,y).
    printf("O", 2, 1, snake[0].x, snake[0].y, 1); // Draw the snake head.
    
    for (int i = 1; i < snake_length; i++) { // Loop through the snake length for the tail.
        printf("o", 2, 1, snake[i].x, snake[i].y, 2); // Draw the tail.
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
            score++; // Increase the score by 1.
            // Food respawn (simple PRNG with PIT seed)
            food.x = (food.x + 13) % VGA_WIDTH;    
            food.y = (food.y + 7) % VGA_HEIGHT;  
        }
    
        // Collision (Game Over) logic
        // Wall collision detection
        if (snake[0].x < 0 || snake[0].x >= VGA_WIDTH || 
            snake[0].y < 0 || snake[0].y >= VGA_HEIGHT) {
            gameOver++;
            printf("GAME OVER", 1, 4, VGA_WIDTH / 2 - 5, VGA_HEIGHT / 2, 1);
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
	
	int show_text = 1;  
    
    // Wait for ENTER to start
    while (1) {
       uint8_t sc = inb(0x60); 
       if (sc == ENTER) { 
          break; 
      }

       if (show_text) {
          printf("   Press [ENTER]", 7, 0, 30, 12, 1); // show text
       } else {
        // Clear the text with spaces of the same length
          printf("                                  ", 7, 0, 30  + 2, 12, 2);
    }

    show_text = !show_text;      // toggle flag
    Sleep(500);                  // flash every 0.5s
  }

    fill_bg(1,1); // Game background

    // Snake setup
    snake_length = 3; 
    score = 0; 
    snake[0].x = 40;
    snake[0].y = 12;
    snake[1].x = 39;
    snake[1].y = 12;
    snake[2].x = 38;
    snake[2].y = 12;
    
    // Food setup
    food.x = 60;
    food.y = 12;
    
    hand_key();
}

void kmain() {
   logic(); 
   halt(); // Halt the CPU
}