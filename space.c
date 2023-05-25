#include "engine.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define MAX_WIDTH 127
#define MAX_HEIGHT 63
#define MAX_ASTEROIDS 20

// define struct for spaceship
struct spaceship {
    int x, y;
};

// define struct for asteroid
struct asteroid {
    int x, y;
};

// define struct for game_state
struct game_state {
    int score;
    struct spaceship player;
    struct asteroid asteroids[MAX_ASTEROIDS];
    int num_asteroids;
};

int play(struct game_state *game, struct button_state *buttons, struct fb *fb) {
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
    if (game->player.x > MAX_WIDTH) {
        game->player.x = MAX_WIDTH;
    }
    if (game->player.y > MAX_HEIGHT) {
        game->player.y = MAX_HEIGHT;
    }

    // for every asteroid
    for (int i = 0; i < game->num_asteroids; i++) {
        // move asteroid from right to left
        game->asteroids[i].x--;

        // if asteroid has gone off screen (to the left), remove it
        if (game->asteroids[i].x < 0) {
            // move last asteroid in array to this position, and decrement asteroid count
            game->asteroids[i] = game->asteroids[--game->num_asteroids];
        }

        // if asteroid has hit spaceship, end game
        if (game->asteroids[i].x == game->player.x && game->asteroids[i].y == game->player.y) {
            return 1;
        }
    }

    // every 10 frames, add a new asteroid at random y position, at the right edge of screen
    if (game->num_asteroids < MAX_ASTEROIDS && rand() % 10 == 0) {
	    printf("Adding asteroid\n");
        game->asteroids[game->num_asteroids].x = MAX_WIDTH;
        game->asteroids[game->num_asteroids].y = rand() % MAX_HEIGHT;
        game->num_asteroids++;
    }

    // increment score
    game->score++;

    // Display the spaceship and asteroids
	printf("Rendering\n");
	clear_buffer(fb);
    render_bitmap(fb, "assets/spaceship.png", game->player.x, game->player.y);
    for (int i=0; i<game->num_asteroids; i++) {
        render_bitmap(fb, "assets/asteroid-big.png", game->asteroids[i].x, game->asteroids[i].y);
    }
	swap_buffer(fb);

    return 0;
}



int main(int argc, char **argv) {
	printf("INIT\n");
    srand(time(0));
	char devname[] = "/dev/input/event0";

	int fd = setup_buttons(devname);
	struct font *ft = load_font("Tamsyn6x12r.psf");
	struct fb *fb = fb_init("/dev/fb0");
	struct game_state game = {0};
	int frame_num = 0;

	// Speed is ms delay until next frame
	int speed = 100;

	printf("START\n");
	while (true) {
		frame_num++;
		struct button_state *buttons = read_buttons(fd);

		if (play(&game, buttons, fb)) {
            printf("GAME OVER\n");
            printf("Score: %d\n", game.score);
            return EXIT_SUCCESS;
        }

		// Get 10% faster every 100 frames
		if (frame_num % 100 == 99) {
			speed = speed / 1.1;
			printf("==SPEED INCREASE==!\nNew speed: %d\n", speed);
		}

		usleep(1000 * speed);
	}

	free_font(ft);
	close(fd);
	return EXIT_SUCCESS;
}
