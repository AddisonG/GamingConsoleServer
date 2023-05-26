#include "engine.h"
#include "snake.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>


bool move_snake(struct snake_game *game, struct button_state *buttons, struct fb *fb) {
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

	// Check if the head of the snake overlaps food
	if (game->y[0] == game->food_y && game->x[0] == game->food_x) {
		printf("NOM NOM NOM\n");
		game->length++;
		game->food_x = rand() % WIDTH;
		game->food_y = rand() % HEIGHT;
	}
	// Check if the head of the snake has overlapped any part of the body
	for (int i = 1; i < game->length; i++) {
		if (game->x[0] == game->x[i] && game->y[0] == game->y[i]) {
			return 1;
		}
	}

	// Draw the snake
	printf("Rendering\n");
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
	return 0;
}

int snake(struct font *ft, struct fb *fb, int buttons_fd) {
	printf("INIT SNAKE\n");
    srand(time(0));

	struct snake_game game = {0};
	int frame_num = 0;

	// Speed is ms delay until next frame
	int speed = 300;

	game.length = 7;
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
		struct button_state *buttons = read_buttons(buttons_fd);

		if (move_snake(&game, buttons, fb)) {
			printf("GAME OVER!\n");

			int score = game.length - 7;
			char score_string[50];
			sprintf(score_string, "SCORE: %d", score);

			clear_buffer(fb);
			draw_box(fb, 1, 1, 126, 62, false);
			render_string(fb, ft, "= GAME OVER =", false, 20, 20);
			render_string(fb, ft, "= YOU LOSE =", false, 23, 28);
			render_string(fb, ft, score_string, false, 33, 36);
			swap_buffer(fb);

			usleep(1000 * 1000 * 5);

			return EXIT_SUCCESS;
		}

		// Get 10% faster every 100 frames
		if (frame_num % 100 == 99) {
			speed = speed / 1.1;
			printf("==SPEED INCREASE==!\nNew speed: %d\n", speed);
		}

		usleep(1000 * speed);
	}

	return EXIT_SUCCESS;
}
