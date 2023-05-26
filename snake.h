#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include "engine.h"
#include <stdbool.h>

#define MAX_SNAKE_LEN 64

#define WIDTH 25
#define HEIGHT 13

#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

struct snake_game {
	int x[MAX_SNAKE_LEN + 1];
	int y[MAX_SNAKE_LEN + 1];
	int length;
	int direction;
	int food_x;
	int food_y;
};

bool move(struct snake_game *game, struct button_state *buttons, struct fb *fb);
int snake(struct font *ft, struct fb *fb, int buttons_fd);

#endif // SNAKE_GAME_H
