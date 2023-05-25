#include "engine.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void play(struct game_state *game, struct button_state *buttons, struct fb *fb) {
	draw_box(fb, game->x, game->y, 20, 20, true);

	if (buttons->up) {
		game->x += 5;
	}
	if (buttons->down) {
		game->x -= 5;
	}
	if (buttons->ok) {
		game->y -= 5;
	}
	if (buttons->back) {
		game->y += 5;
	}
}

int main(int argc, char **argv) {
	printf("INIT\n");
    char devname[] = "/dev/input/event0";

    int fd = setup_buttons(devname);
	struct fb *fb = fb_init("/dev/fb0");
	//struct font *ft = load_font("/etc/lcd-assets/Tamsyn6x12r.psf");
	struct game_state game;
	game.x = 20;
	game.y = 20;

	int frame = 0;
	printf("START\n");

    while (true) {
		frame++;
		clear_buffer(fb);
		struct button_state *buttons = read_buttons(fd);

		play(&game, buttons, fb);

		swap_buffer(fb);
        usleep(1000 * 100);
    }
	//free_font(ft);

    close(fd);
    return EXIT_SUCCESS;
}

