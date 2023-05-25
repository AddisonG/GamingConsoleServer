#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define UP 103
#define DOWN 108
#define OK 28
#define CANCEL 14

int main(int argc, char **argv) {
    struct input_event ev;
    int fd;
    char devname[] = "/dev/input/event0";

    fd = open(devname, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", devname, strerror(errno));
        return EXIT_FAILURE;
    }

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(struct input_event));

        if (n == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                break;
            }
        } else if (n >= sizeof(struct input_event)) {
            if (ev.type == EV_KEY) {
                switch (ev.code) {
                    case UP:
                        if (ev.value == KEY_PRESS)
                            printf("Up key was pressed\n");
                        else if (ev.value == KEY_RELEASE)
                            printf("Up key was released\n");
                        break;
                    case DOWN:
                        if (ev.value == KEY_PRESS)
                            printf("Down key was pressed\n");
                        else if (ev.value == KEY_RELEASE)
                            printf("Down key was released\n");
                        break;
                    case OK:
                        if (ev.value == KEY_PRESS)
                            printf("OK key was pressed\n");
                        else if (ev.value == KEY_RELEASE)
                            printf("OK key was released\n");
                        break;
                    case CANCEL:
                        if (ev.value == KEY_PRESS)
                            printf("Cancel key was pressed\n");
                        else if (ev.value == KEY_RELEASE)
                            printf("Cancel key was released\n");
                        break;
                    default:
                        break;
                }
            }
        }

    }

    close(fd);
    return EXIT_SUCCESS;
}

