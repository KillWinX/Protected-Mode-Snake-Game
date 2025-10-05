#include "inc/printf.h"
#include "inc/io.h"
#include "inc/halt_cpu.h"
#include "inc/Sleep.h"
#include "inc/rand.h"
#include "inc/scancodes.h"
#include "inc/beep.h"

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
int snake_length = 1;
Point food;
int gameOver = 0;
char direction = RIGHT;
int speed = 100;

char score_text[32] = "SCORE: 0";

void increase_score() {
    int len = 0;

    for(int i = 0; i < 32 && score_text[i]; i++) {
        if(score_text[i] >= '0' && score_text[i] <= '9') len++;
    }

    if(len == 0) return;

    for(int i = 7 + len - 1; i >= 7; i--) {
        if(score_text[i] >= '0' && score_text[i] <= '8') {
            score_text[i]++;
            break;
        } else if(score_text[i] == '9') {
            score_text[i] = '0';
        }

        if(i == 7 && score_text[i] == '0') {
            for(int j = 7 + len; j > 7; j--) {
                score_text[j] = score_text[j-1];
            }
            score_text[7] = '1';
        }
    }
}


void draw_score() {
    printf(score_text, 7, 0, 0, 0, 1); 
}

void draw_snake() {
    fill_bg(1, 1); 
    
    printf("@", 4, 1, food.x, food.y, 1); 
    printf("O", 2, 1, snake[0].x, snake[0].y, 1); 
    
    for (int i = 1; i < snake_length; i++) { 
        printf("o", 2, 1, snake[i].x, snake[i].y, 2); 
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
            snake_length++; // Increase snake length
            increase_score();
            if(speed > 20) speed -= 5;
            food.x = rand() % VGA_WIDTH;    
            food.y = rand() % VGA_HEIGHT;  
        }
    
        if (snake[0].x < 0 || snake[0].x >= VGA_WIDTH || 
            snake[0].y < 0 || snake[0].y >= VGA_HEIGHT) {
            gameOver++;
            printf("GAME OVER", 1, 4, VGA_WIDTH / 2 - 5, VGA_HEIGHT / 2, 1);
            break;
        }
        
        for (int i = 1; i < snake_length; i++) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) { 
                gameOver++; 
                printf("GAME OVER", 1, 4, VGA_WIDTH / 2 - 5, VGA_HEIGHT / 2, 1);
                break; 
            }
        }

        draw_snake(); 
        draw_score();
        Sleep(speed); 
    }
}


void logic() {
    outb(0x3D4, 0x0A);  
    outb(0x3D5, 0x20);

    fill_bg(0,0);

    printf("SNAKE GAME MADE BY KILLWINX\n", 2, 0, 35 - 8, 10, 1); 
	
	int show_text = 1;  
    
    while (1) {
       uint8_t sc = inb(0x60); 
       if (sc == ENTER) { 
          break; 
       }
       if (show_text) {
          printf("   Press [ENTER]", 7, 0, 30, 12, 1);
       } else {
          printf("                                  ", 7, 0, 30  + 2, 12, 2);
    }

    show_text = !show_text;     
    Sleep(500);               
  }

    fill_bg(1,1); 

    pit_init(1000);

    srand(timer_ticks);

    unsigned int r = rand(); 

    snake_length = 1; 
    snake[0].x = 40;
    snake[0].y = 12;
    snake[1].x = 39;
    snake[1].y = 12;
    snake[2].x = 38;
    snake[2].y = 12;
    
    food.x = rand() % VGA_WIDTH;
    food.y = rand() % VGA_HEIGHT;
    
    hand_key();
}

void kmain() {
   logic(); 
   halt(); 
}