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

    fd = open(devname, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", devname, strerror(errno));
        return EXIT_FAILURE;
    }

    while (1) {
        read(fd, &ev, sizeof(struct input_event));
        
        if (ev.type == EV_KEY) {
            if (ev.value == KEY_PRESS) {
                printf("key %i was pressed\n", ev.code);
            } else if(ev.value == KEY_RELEASE) {
                printf("key %i was released\n", ev.code);
            }
        }
    }

    return EXIT_SUCCESS;
}
