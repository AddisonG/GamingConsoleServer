#ifndef ENGINE_H
#define ENGINE_H

#include "fb.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define UP 103
#define DOWN 108
#define OK 28
#define BACK 14

struct button_state {
    int up;  // Top left
    int down;  // Bottom left
    int ok;  // Top right
    int back;  // Bottom right
};

struct game_state {
	int score;
	int x;
	int y;
};

int setup_buttons(char* devname);
struct button_state* read_buttons(int fd);
void draw_box(struct fb *fb, int x_orig, int y_orig, int width, int height, bool filled);

#endif /* ENGINE_H */
