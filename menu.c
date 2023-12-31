#include "engine.h"
#include "space.h"
#include "space_too.h"
#include "snake.h"
#include "tetris.h"
#include "remember.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>


#define NUM_GAMES 5

#define SNAKE 0
#define SPACE 1
#define TETRIS 2
#define SPACE_TOO 3
#define REMEMBER 4


void segfault_handler(int signal) {
	printf("ERROR! SEGFAULT!\n");
}


void draw_options(struct fb *fb, struct font *ft, int selected) {
	clear_buffer(fb);
	draw_box(fb, 0, 0, 128, 64, false);
	render_string(fb, ft, "=OG GAMING CONSOLE=", false, 7, 1);

	render_string(fb, ft, "SNAKE", false, 5, 10);
	render_string(fb, ft, "SPACE", false, 5, 19);
	render_string(fb, ft, "TETRIS", false, 5, 28);
	render_string(fb, ft, "SPACE_TOO", false, 5, 37);
	render_string(fb, ft, "REMEMBER", false, 5, 46);
	// render_string(fb, ft, "YYYYYY", false, 5, 55);

	// Draw box around selected option
	draw_box(fb, 4, 10 + (selected * 9), 60, 11, false);

	// Draw the image
	render_bitmap(fb, "assets/ogflower.png", 67, 11);

	swap_buffer(fb);
}


int handle_buttons(struct button_state *buttons, int *selected) {
	if (buttons->ok) {
		return 1;
	}
	if (buttons->up) {
		(*selected)--;
	}
	if (buttons->down) {
		(*selected)++;
	}

	// Fix negatives and stuff
	*selected = (*selected + NUM_GAMES) % NUM_GAMES;

	return 0;
}


int main(int argc, char **argv) {
	printf("INIT MENU\n");
	srand(time(0));
	signal(SIGSEGV, segfault_handler);
	char devname[] = "/dev/input/event0";

	int buttons_fd = setup_buttons(devname);
	struct font *ft = load_font("Tamsyn6x12r.psf");
	struct fb *fb = fb_init("/dev/fb0");

	int selected = SNAKE;

	while (true) {
		struct button_state *buttons = read_buttons(buttons_fd);

		draw_options(fb, ft, selected);

		if (handle_buttons(buttons, &selected)) {
			switch (selected) {
				case SPACE:
					printf("SPACE\n");
					space(ft, fb, buttons_fd);
					break;
				case SNAKE:
					printf("SNAKE\n");
					snake(ft, fb, buttons_fd);
					break;
				case TETRIS:
					printf("TETRIS\n");
					tetris(ft, fb, buttons_fd);
					break;
				case SPACE_TOO:
					printf("SPACE_TOO\n");
					space_too(ft, fb, buttons_fd);
					break;
				case REMEMBER:
					printf("REMEMBER\n");
					remember(ft, fb, buttons_fd);
					break;
				default:
					break;
			}
		}

		usleep(1000 * 200);
	}

	free_font(ft);
	close(buttons_fd);
	return EXIT_SUCCESS;
}
