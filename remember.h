#ifndef REMEMBER_GAME_H
#define REMEMBER_GAME_H

#include "engine.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define MAX_WIDTH 128
#define MAX_HEIGHT 64
#define NUM_SQUARES 10

struct remember_game {
	int score;
	int chances;
	int guess1;
	int guess2;
	int hovered;
	char state[NUM_SQUARES][2];
};

void shuffle_remember(char *array[], int n);
int play_remember(struct remember_game *game, struct button_state *buttons, struct fb *fb, struct font *ft);
int remember(struct font *ft, struct fb *fb, int buttons_fd);

#endif // REMEMBER_GAME_H
