#include "inc/printf.h"
#include "inc/io.h"
#include "inc/halt_cpu.h"
#include "inc/rand.h"
#include "inc/PIT.h"
#include "inc/Sleep.h"


#include "inc/scancodes.h"

typedef struct {
    int x, y;
} Point;

Point snake[100];
int snake_length;
Point food;
int score;
int gameOver;


typedef enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT
} Direction;

char direction = 0;

void draw_snake() {
    printf("@", 4, 0, food.x, food.y, 1);
    
    printf("O", 5, 0, snake[0].x, snake[0].y, 1);
    

    for (int i = 1; i < snake_length; i++) {
        printf("o", 5, 0, snake[i].x, snake[i].y, 2);
    }
}

void move_snake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    switch(direction) {
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }

    if (snake[0].x < 0) snake[0].x = VGA_WIDTH - 1;
    if (snake[0].x >= VGA_WIDTH) snake[0].x = 0;
    if (snake[0].y < 0) snake[0].y = VGA_HEIGHT - 1;
    if (snake[0].y >= VGA_HEIGHT) snake[0].y = 0;
}

void hand_key() {
    while (!gameOver) {
        uint8_t sc = inb(0x60);
        if (!(sc & 0x80)) { 
            if (sc == W && direction != DOWN) direction = UP;
            else if (sc == S && direction != UP) direction = DOWN;
            else if (sc == A && direction != RIGHT) direction = LEFT;
            else if (sc == D && direction != LEFT) direction = RIGHT;
        }

        move_snake();

        if (snake[0].x == food.x && snake[0].y == food.y) {
            snake_length++;    
            score++;           
            food.x = rand() % VGA_WIDTH;   
            food.y = rand() % VGA_HEIGHT;  
        }
       
        for (int i = 1; i < snake_length; i++) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                gameOver = 1;
                break;
            }
        }

        fill_bg(0, 0);
       // draw_walls();
        draw_snake();
        Sleep(100);
    }
}

void logic() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    fill_bg(0,0);

    pit_init(1000);

    srand(timer_ticks); 

    unsigned int r = rand(); 

    snake_length = 3;
    snake[0].x = 80 / 2;
    snake[0].y = 50 / 2;
    score = 0;
    gameOver = 0;
    food.x = rand() % VGA_WIDTH;
    food.y = rand() % VGA_HEIGHT;
     
    hand_key();
}

void kmain() {
   logic();

   halt();
}