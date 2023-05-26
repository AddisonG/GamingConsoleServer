#include "engine.h"
#include "space.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define MAX_WIDTH 128
#define MAX_HEIGHT 64
#define MAX_ASTEROIDS 50


int play(struct space_game *game, struct button_state *buttons, struct fb *fb) {
	// update spaceship position based on button input
	if (buttons->up) {
		game->player.y--;
	} else if (buttons->down) {
		game->player.y++;
	} else if (buttons->ok) {
		// game->player.x--;
		// Fire cannons ?
		0;
	} else if (buttons->back) {
		// game->player.x++;
		0;
	}

	// keep spaceship within screen bounds
	if (game->player.x < 0) {
		game->player.x = 0;
	}
	if (game->player.y < 0) {
		game->player.y = 0;
	}
	if (game->player.x > MAX_WIDTH - game->player.width) {
		game->player.x = MAX_WIDTH - game->player.width;
	}
	if (game->player.y > MAX_HEIGHT - game->player.height) {
		game->player.y = MAX_HEIGHT - game->player.height;
	}

	// for every asteroid
	for (int i = 0; i < game->num_asteroids; i++) {
		// move asteroid from right to left
		game->asteroids[i].x--;

		// if asteroid has gone off screen (to the left), remove it
		if (game->asteroids[i].x <= (game->asteroids[i].width * -1)) {
			// move last asteroid in array to this position, and decrement asteroid count
			game->asteroids[i] = game->asteroids[--game->num_asteroids];

			// increment score
			game->score++;
		}

		// if asteroid has hit spaceship, end game
		int asteroid_left = game->asteroids[i].x;
		int asteroid_top = game->asteroids[i].y;
		int asteroid_bottom = game->asteroids[i].y + game->asteroids[i].height;
		int asteroid_right = game->asteroids[i].x + game->asteroids[i].width;
		int player_left = game->player.x;
		int player_top = game->player.y;
		int player_bottom = game->player.y + game->player.height;
		int player_right = game->player.x + game->player.width;
		printf("%d, %d, %d, %d, %d, %d, %d, %d\n",asteroid_left, asteroid_top, asteroid_bottom, asteroid_right, player_left, player_top, player_bottom, player_right);
		if (
			((player_top < asteroid_bottom && player_top > asteroid_top) || (player_bottom < asteroid_bottom && player_bottom > asteroid_top)) &&
			((player_right > asteroid_left && player_right < asteroid_right) || (player_left < asteroid_right && player_left > asteroid_left))
		) {
			return 1;
		}
	}

	// Add a new asteroid at random y position, at the right edge of screen
	if (game->num_asteroids < MAX_ASTEROIDS && rand() % (11 - game->danger) == 0) {
		printf("Adding asteroid\n");
		game->asteroids[game->num_asteroids].x = MAX_WIDTH;
		game->asteroids[game->num_asteroids].y = rand() % (MAX_HEIGHT - 6);
		if ((rand() % 100) > (30 + 5 * game->danger)) {
			game->asteroids[game->num_asteroids].height = 7;
			game->asteroids[game->num_asteroids].width = 7;
		} else {
			game->asteroids[game->num_asteroids].height = 11;
			game->asteroids[game->num_asteroids].width = 11;
		}
		game->num_asteroids++;
	}

	// Display the spaceship and asteroids
	printf("Rendering\n");
	render_bitmap(fb, "assets/spaceship.png", game->player.x, game->player.y);
	for (int i = 0; i < game->num_asteroids; i++) {
		if (game->asteroids[i].width == 7) {
			render_bitmap(fb, "assets/asteroid-small.png", game->asteroids[i].x, game->asteroids[i].y);
		} else if (game->asteroids[i].width == 11) {
			render_bitmap(fb, "assets/asteroid-big.png", game->asteroids[i].x, game->asteroids[i].y);
		}
	}
	swap_buffer(fb);

	return 0;
}


int space(struct font *ft, struct fb *fb, int buttons_fd) {
	printf("INIT SPACE\n");
	srand(time(0));

	struct space_game game = {0};
	int frame_num = 0;

	// Speed is ms delay until next frame
	game.speed = 100;
	game.danger = 1;

	game.player.x = 2;
	game.player.y = 30;
	game.player.height = 7;
	game.player.width = 8;

	printf("START\n");
	while (true) {
		clear_buffer(fb);
		frame_num++;
		struct button_state *buttons = read_buttons(buttons_fd);

		// Let the player know about the increased danger!
		if ((frame_num % 100) > 90) {
			render_string(fb, ft, "INCREASED DANGER!", false, 20, 30);
		}

		if (play(&game, buttons, fb)) {
			printf("GAME OVER\n");
			printf("Score: %d\n", game.score);

			usleep(1000 * 1000 * 2);

			char score_string[50];
			sprintf(score_string, "SCORE: %d", game.score);

			clear_buffer(fb);
			draw_box(fb, 1, 1, 126, 62, false);
			render_string(fb, ft, "= GAME OVER =", false, 20, 20);
			render_string(fb, ft, "= YOU LOSE =", false, 22, 28);
			render_string(fb, ft, score_string, false, 33, 36);
			swap_buffer(fb);

			usleep(1000 * 1000 * 3);

			return EXIT_SUCCESS;
		}

		// Increase danger every 100 frames
		if (frame_num % 100 == 90) {
			game.speed = game.speed / 1.05;
			printf("==SPEED INCREASE==!\nNew speed: %d\n", game.speed);

			// Max danger level is 10
			game.danger = game.danger + 1 < 10 ? game.danger + 1 : 10;
		}

		usleep(1000 * game.speed);
	}

	return EXIT_SUCCESS;
}
