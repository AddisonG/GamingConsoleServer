#include "engine.h"
#include "tetris.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>


#include <stdlib.h>
#include <stdbool.h>

#define GAME_WIDTH 24
#define GAME_HEIGHT 13

#define NUM_TETROMINOS 7
#define TETROMINO_SIZE 4

struct block {
    int x, y;
};

struct tetromino {
    struct block blocks[TETROMINO_SIZE];
};

struct tetris_game {
    int score;
    int field[GAME_HEIGHT][GAME_WIDTH];
    struct tetromino current_tetromino;
    int current_tetromino_x, current_tetromino_y;
};

static const struct tetromino tetrominos[NUM_TETROMINOS] = {
    {{{0, 0}, {1, 0}, {0, 1}, {1, 1}}}, // square
    {{{0, 0}, {0, 1}, {0, 2}, {0, 3}}}, // line
    {{{0, 0}, {1, 0}, {2, 0}, {1, 1}}}, // T
    {{{0, 0}, {1, 0}, {2, 0}, {2, 1}}}, // L
    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}}}, // mirrored L
    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}}}, // S
    {{{1, 0}, {0, 1}, {1, 1}, {2, 0}}}, // Z
};

void render_screen(struct tetris_game *game, struct fb *fb) {
    clear_buffer(fb);

    // draw game field
    for (int y = 0; y < GAME_HEIGHT; y++) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (game->field[y][x]) {
                draw_box(fb, x * 5, y * 5, 5, 5, true);
            }
        }
    }

    // draw current tetromino
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        int x = game->current_tetromino_x + game->current_tetromino.blocks[i].x;
        int y = game->current_tetromino_y + game->current_tetromino.blocks[i].y;
        draw_box(fb, x * 5, y * 5, 5, 5, true);
    }

    swap_buffer(fb);
}

bool is_collision(struct tetris_game *game, int x, int y) {
    for (int i = 0; i < TETROMINO_SIZE; i++) {
		printf("%d, %d\n", game->current_tetromino.blocks[i].x, game->current_tetromino.blocks[i].y);
        int block_x = x + game->current_tetromino.blocks[i].x;
        int block_y = y + game->current_tetromino.blocks[i].y;
		printf("%d, %d\n", block_x, block_y);
        if (block_x < 0 || block_x >= GAME_WIDTH || block_y < 0 || block_y >= GAME_HEIGHT || game->field[block_y][block_x]) {
            return true;
        }
    }
    return false;
}

void remove_line(struct tetris_game *game, int line) {
    for (int y = line; y > 0; y--) {
        for (int x = 0; x < GAME_WIDTH; x++) {
            game->field[y][x] = game->field[y - 1][x];
        }
    }
    for (int x = 0; x < GAME_WIDTH; x++) {
        game->field[0][x] = 0;
    }
}

void check_lines(struct tetris_game *game) {
	printf("TETRIS TIME");
    for (int y = 0; y < GAME_HEIGHT; y++) {
        bool line = true;
        for (int x = 0; x < GAME_WIDTH; x++) {
            if (!game->field[y][x]) {
                line = false;
                break;
            }
        }
        if (line) {
            remove_line(game, y);
            game->score++;
        }
    }
}

bool move_piece(struct tetris_game *game, struct button_state *buttons) {
    int new_x = game->current_tetromino_x;
    int new_y = game->current_tetromino_y;

    if (buttons->up) {
        new_y--;
    } else if (buttons->down) {
        new_y++;
    } else if (buttons->ok) {
        // TODO: rotate tetromino
		0;
    } else if (buttons->back) {
        new_x++;
    }

	printf("%d, %d \n", new_x, new_y);
	printf("%d, %d \n", game->current_tetromino_x, game->current_tetromino_y);

    if (!is_collision(game, new_x, new_y)) {
        game->current_tetromino_x = new_x;
        game->current_tetromino_y = new_y;
    } else {
		printf("COLLISION\n");
        // landed, fix in place and spawn new tetromino
        for (int i = 0; i < TETROMINO_SIZE; i++) {
            int x = game->current_tetromino_x + game->current_tetromino.blocks[i].x;
            int y = game->current_tetromino_y + game->current_tetromino.blocks[i].y;
            if (y < 0) {
                // game over
                return true;
            }
            game->field[y][x] = 1;
        }
        check_lines(game);
        game->current_tetromino = tetrominos[rand() % NUM_TETROMINOS];
        game->current_tetromino_x = GAME_WIDTH / 2;
        game->current_tetromino_y = 0;
    }

    return false;
}


int tetris(struct font *ft, struct fb *fb, int buttons_fd) {
	printf("INIT TETRIS\n");
    srand(time(0));

	struct tetris_game game = {0};
	int frame_num = 0;

	// Set up game
	game.current_tetromino = tetrominos[rand() % NUM_TETROMINOS];
	game.current_tetromino_x = 3;
	game.current_tetromino_y = 5;

	// Speed is ms delay until next frame
	int speed = 200;

	while (true) {
		frame_num++;
		struct button_state *buttons = read_buttons(buttons_fd);

		render_screen(&game, fb);

		if (move_piece(&game, buttons)) {
			printf("GAME OVER!\n");

			usleep(1000 * 1000 * 3);

			char score_string[50];
			sprintf(score_string, "SCORE: %d", game.score);

			clear_buffer(fb);
			draw_box(fb, 1, 1, 126, 62, false);
			render_string(fb, ft, "= GAME OVER =", false, 20, 20);
			render_string(fb, ft, "= YOU LOSE =", false, 23, 28);
			render_string(fb, ft, score_string, false, 33, 36);
			swap_buffer(fb);

			usleep(1000 * 1000 * 5);

			return EXIT_SUCCESS;
		}

		if (frame_num % 3 == 0) {
			game.current_tetromino_x++;
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
