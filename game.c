#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#include "fb.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define UP 103
#define DOWN 108
#define OK 28
#define BACK 14


struct game_state {
	int score;
	int x;
	int y;
};

struct button_state {
    int up;  // Top left
    int down;  // Bottom left
    int ok;  // Top right
    int back;  // Bottom right
};

struct button_state prev_buttons = {0, 0, 0, 0};
struct button_state buttons = {0, 0, 0, 0};


int setup_buttons(char* devname) {
    int fd = open(devname, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", devname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}


void read_buttons(int fd) {
    struct input_event ev;
    prev_buttons = buttons; // save previous state

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(struct input_event));
        if (n == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            break;
        } else if (n >= sizeof(struct input_event)) {
            if (ev.type == EV_KEY) {
                switch (ev.code) {
                    case UP:
                        buttons.up = ev.value == KEY_PRESS ? 1 : 0;
                        break;
                    case DOWN:
                        buttons.down = ev.value == KEY_PRESS ? 1 : 0;
                        break;
                    case OK:
                        buttons.ok = ev.value == KEY_PRESS ? 1 : 0;
                        break;
                    case BACK:
                        buttons.back = ev.value == KEY_PRESS ? 1 : 0;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


void draw_box(struct fb *fb, int x_orig, int y_orig, int width, int height, bool filled) {
	// Adjust width and height if they extend beyond the screen's dimensions
    if (x_orig + width > 128) {
        width = 128 - x_orig;
    }
    if (y_orig + height > 64) {
        height = 64 - y_orig;
    }
    if (x_orig < 0) {
        width += x_orig;
        x_orig = 0;
    }
    if (y_orig < 0) {
        height += y_orig;
        y_orig = 0;
    }

	if (x_orig < 0 || y_orig < 0 || width <= 0 || height <= 0) {
        return;
    }
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (filled) {
				set_bit(fb, x + x_orig, y + y_orig);
			} else {
				if (y == 0 || y == (height - 1)) {
					set_bit(fb, x + x_orig, y + y_orig);
				} else if (x == 0 || x == (width - 1)) {
					set_bit(fb, x + x_orig, y + y_orig);
				}
			}
		}
	}
}


void play(struct game_state *game, struct fb *fb) {
	//render_string(fb, ft, "-Opengear IM7200-", false, 0, 38);

	//printf("Frame: %d\n", frame);
	draw_box(fb, game->x, game->y, 20, 20, true);

	if (buttons.up) {
		game->x += 5;
	}
	if (buttons.down) {
		game->x -= 5;
	}
	if (buttons.ok) {
		game->y -= 5;
	}
	if (buttons.back) {
		game->y += 5;
	}
}


int main(int argc, char **argv) {
    char devname[] = "/dev/input/event0";

    int fd = setup_buttons(devname);
	struct fb *fb = fb_init("/dev/fb0");
	struct font *ft = load_font("/etc/lcd-assets/Tamsyn6x12r.psf");
	struct game_state game;
	game.x = 20;
	game.y = 20;

	int frame = 0;

    while (true) {
		frame++;
		clear_buffer(fb);
		read_buttons(fd);

		// Game logic

		play(&game, fb);

		swap_buffer(fb);
        usleep(1000 * 100);
    }
	free_font(ft);

    close(fd);
    return EXIT_SUCCESS;
}
