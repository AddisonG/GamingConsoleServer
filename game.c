#include "engine.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_SNAKE_LEN 64

#define WIDTH 25
#define HEIGHT 13


#define UP 0
#define LEFT 1
#define DOWN 2
#define RIGHT 3

struct game_state {
	int x[MAX_SNAKE_LEN + 1];
	int y[MAX_SNAKE_LEN + 1];
	int length;
	int direction;
	int food_x;
	int food_y;
};


void move(struct game_state *game, struct button_state *buttons, struct fb *fb) {
	// Move the tail of the snake
	printf("Moving tail\n");
	for (int i = game->length; i > 0; i--) {
		game->x[i] = game->x[i - 1];
		game->y[i] = game->y[i - 1];
	}

	// Move the head of the snake
	int new_coord;
	printf("Moving head\n");
	switch (game->direction) {
		case UP:
			new_coord = game->y[0] - 1;
			game->y[0] = new_coord < 0 ? HEIGHT - 1 : new_coord;
			break;
		case LEFT:
			new_coord = game->x[0] - 1;
			game->x[0] = new_coord < 0 ? WIDTH - 1 : new_coord;
			break;
		case DOWN:
			new_coord = game->y[0] + 1;
			game->y[0] = new_coord >= HEIGHT ? 0 : new_coord;
			break;
		case RIGHT:
			new_coord = game->x[0] + 1;
			game->x[0] = new_coord >= WIDTH ? 0 : new_coord;
			break;
	}

	// Check the head of the snake hasn't overlapped anything, or eaten food
	if (game->y[0] == game->food_y && game->x[0] == game->food_x) {
		printf("NOM NOM NOM\n");
		game->length++;
		game->food_x = rand() % WIDTH;
		game->food_y = rand() % HEIGHT;
	}

	// Draw the snake
	printf("Drawing snake\n");
	clear_buffer(fb);
	for (int i = 0; i < game->length; i++) {
		if (i == 0) {
			// Head
			switch (game->direction) {
				case UP:
					// The image is 6 pixels tall, because of the tounge. Line it up correctly
					render_bitmap(fb, "assets/snake_head_north.png", game->x[i] * 5, (game->y[i] * 5) - 1);
					break;
				case LEFT:
					render_bitmap(fb, "assets/snake_head_west.png", game->x[i] * 5, game->y[i] * 5);
					break;
				case DOWN:
					render_bitmap(fb, "assets/snake_head_south.png", game->x[i] * 5, game->y[i] * 5);
					break;
				case RIGHT:
					render_bitmap(fb, "assets/snake_head_east.png", game->x[i] * 5, game->y[i] * 5);
					break;
			}
		} else {
			draw_box(fb, game->x[i] * 5, game->y[i] * 5, 5, 5, true);
		}
		printf("%d,%d  ", game->x[i], game->y[i]);
	}
	printf(" (DIR: %d)\n", game->direction);

	// Draw food
	draw_box(fb, game->food_x * 5, game->food_y * 5, 5, 5, false);

	swap_buffer(fb);

	printf("Changing dir\n");
	// Change the direction of the snake
	if (buttons->up) {
		game->direction = (game->direction + 4 + 1) % 4;
		printf("UP\n");
	}
	if (buttons->down) {
		game->direction = (game->direction + 4 - 1) % 4;
		printf("DOWN\n");
	}
}

int main(int argc, char **argv) {
	printf("INIT\n");
	char devname[] = "/dev/input/event0";

	int fd = setup_buttons(devname);
	struct fb *fb = fb_init("/dev/fb0");
	struct game_state game = {0};
	int frame_num = 0;

	// Speed is ms delay until next frame
	int speed = 300;

	game.length = 4;
	game.direction = RIGHT;

	for (int i = 0; i < game.length; i++) {
		game.x[i] = 6 - i;
		game.y[i] = 4;
	}

	game.food_x = 20;
	game.food_y = 4;

	printf("START\n");
	while (true) {
		frame_num++;
		struct button_state *buttons = read_buttons(fd);

		move(&game, buttons, fb);

		// Get 10% faster every 100 frames
		if (frame_num % 100 == 99) {
			speed = speed / 1.1;
			printf("New speed: %f\n", speed);
		}

		usleep(1000 * speed);
	}

	close(fd);
	return EXIT_SUCCESS;
}
