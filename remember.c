#include "engine.h"
#include "remember.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>


int play_remember(struct remember_game *game, struct button_state *buttons, struct fb *fb, struct font *ft) {
	if (buttons->up) {
		do {
			game->hovered++;
			if (game->hovered > NUM_SQUARES - 1) {
				game->hovered = 0;
			}
		} while (game->state[game->hovered][0] == '\0');
	} else if (buttons->down) {
		do {
			game->hovered--;
			if (game->hovered < 0) {
				game->hovered = NUM_SQUARES - 1;
			}
		} while (game->state[game->hovered][0] == '\0');
	} else if (buttons->ok) {
		// don't select empty
		if (game->state[game->hovered][0] != '\0') {
			if (game->guess1 == 99) {
				game->guess1 = game->hovered;
			} else if (game->guess2 == 99 && (game->guess1 != game->hovered)) {
				game->guess2 = game->hovered;
			}
		}
	} else if (buttons->back) {
		return 1;
	}

	if (game->hovered > NUM_SQUARES - 1) {
		game->hovered = 0;
	}

	if (game->hovered < 0) {
		game->hovered = 0;
	}

	if (game->guess1 != 99 && game->guess2 != 99) {
		if (strcmp(game->state[game->guess1], game->state[game->guess2]) == 0) {
			strcpy(game->state[game->guess1], "");
			strcpy(game->state[game->guess2], "");
			game->score++;
		} else {
			game->chances--;
		}
		usleep(1000 * 500);
	}

	if (game->chances <= 0) {
		return 1;
	}

	// Draw game state
	printf("Rendering\n");
	// Draw letters or boxes
	for (int i = 0; i < NUM_SQUARES; i++) {
		if (game->state[i][0] != '\0') {
			if (i == game->guess1 || i == game->guess2) {
				render_string(fb, ft, game->state[i], false, 0 + 10*i, 10);
			} else {
				draw_box(fb, 0 + 10*i, 10, 8, 8, i == game->hovered);
			}
		}
	}
	swap_buffer(fb);

	return 0;
}


int remember(struct font *ft, struct fb *fb, int buttons_fd) {
	printf("INIT REMEMBER\n");
	srand(time(0));

	struct remember_game game = {0};

	strcpy(game.state[0], "A");
	strcpy(game.state[1], "A");
	strcpy(game.state[2], "B");
	strcpy(game.state[3], "B");
	strcpy(game.state[4], "C");
	strcpy(game.state[5], "C");
	strcpy(game.state[6], "D");
	strcpy(game.state[7], "D");
	strcpy(game.state[8], "E");
	strcpy(game.state[9], "E");
	game.score = 0;
	game.chances = 5;
	game.guess1 = 99;
	game.guess2 = 99;
	game.hovered = 0;

	shuffle_remember(&(game.state), 5);

	printf("START\n");
	while (true) {
		clear_buffer(fb);
		struct button_state *buttons = read_buttons(buttons_fd);

		if (play_remember(&game, buttons, fb, ft)) {
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


		if (game.guess1 != 99 && game.guess2 != 99) {
			// clear guesses and give them a moment
			game.guess1 = 99;
			game.guess2 = 99;
			usleep(1000 * 2000);
		}

		usleep(1000 * 500);
	}

	return EXIT_SUCCESS;
}

void shuffle_remember(char *array[], int n) {
	if (n > 1) {
		for (int i = 0; i < n - 1; i++) {
			int j = i + rand() / (RAND_MAX / (n - i) + 1);
			char *t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}
