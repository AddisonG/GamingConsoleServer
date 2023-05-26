#ifndef SPACE_GAME_H
#define SPACE_GAME_H

#include "engine.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define MAX_WIDTH 128
#define MAX_HEIGHT 64
#define MAX_ASTEROIDS 50

struct spaceship {
	int x, y;
	int width, height;
};

struct asteroid {
	int x, y;
	int width, height;
};

struct space_game {
	int score;
	int speed;
	int danger;
	struct spaceship player;
	struct asteroid asteroids[MAX_ASTEROIDS];
	int num_asteroids;
};

int play(struct space_game *game, struct button_state *buttons, struct fb *fb);
int space(struct font *ft, struct fb *fb, int buttons_fd);

#endif // SPACE_GAME_H
