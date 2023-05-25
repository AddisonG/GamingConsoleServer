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


struct button_state read_buttons(int fd) {
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

    return buttons;
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


int main(int argc, char **argv) {
    char devname[] = "/dev/input/event0";

    int fd = setup_buttons(devname);
	struct fb *fb = fb_init("/dev/fb0");
	struct font *ft = load_font("/etc/lcd-assets/Tamsyn6x12r.psf");

	int frame = 0;
	int x = 0, y = 0;

    while (true) {
		frame++;
		clear_buffer(fb);
		draw_box(fb, x, y, 20, 20, true);
		//render_string(fb, ft, "-Opengear IM7200-", false, 0, 38);
		swap_buffer(fb);
		//free_font(ft);
		//close_fb(fb);
		//return 0;

        struct button_state buttons = read_buttons(fd);

        printf("Frame: %d\n", frame);

        if (buttons.up) {
            printf("Up key is pressed\n");
			x += 5;
        }
        if (buttons.down) {
            printf("Down key is pressed\n");
			x -= 5;
        }
        if (buttons.ok) {
            printf("OK key is pressed\n");
			y -= 5;
        }
        if (buttons.back) {
            printf("back key is pressed\n");
			y += 5;
        }
        usleep(1000 * 100);
    }

    close(fd);
    return EXIT_SUCCESS;
}
