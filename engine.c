#include "engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

struct button_state true_buttons = {0, 0, 0, 0};
struct button_state return_buttons = {0, 0, 0, 0};

int setup_buttons(char* devname) {
	printf("SETTING UP BUTTONS\n");
    int fd = open(devname, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", devname, strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

struct button_state* read_buttons(int fd) {
    struct input_event ev;
    return_buttons = true_buttons;

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(struct input_event));
        if (n == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            break;
        } else if (n >= sizeof(struct input_event)) {
			// The strange math means that any NEW push will always result in a positive number.
			// Any existing push will be overridden by releasing it.
            if (ev.type == EV_KEY) {
                switch (ev.code) {
                    case UP_BUTTON:
                        true_buttons.up = ev.value;
                        return_buttons.up += (ev.value * 5) - 1;
                        break;
                    case DOWN_BUTTON:
                        true_buttons.down = ev.value;
                        return_buttons.down += (ev.value * 5) - 1;
                        break;
                    case OK_BUTTON:
                        true_buttons.ok = ev.value;
                        return_buttons.ok += (ev.value * 5) - 1;
                        break;
                    case BACK_BUTTON:
                        true_buttons.back = ev.value;
                        return_buttons.back += (ev.value * 5) - 1;
                        break;
                }
            }
        }
    }

    // Convert negatives into 0 (false)
	return_buttons.up = return_buttons.up > 0;
	return_buttons.down = return_buttons.down > 0;
	return_buttons.ok = return_buttons.ok > 0;
	return_buttons.back = return_buttons.back > 0;

	return &return_buttons;
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
