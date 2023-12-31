CC = /home/addison/personal/hackathon-2023/armv5te-toolchain/.build/arm-unknown-linux-uclibcgnueabi/buildtools/bin/arm-unknown-linux-uclibcgnueabi-gcc
CFLAGS += -march=armv5te
CFLAGS += -Wl,--unresolved-symbols=ignore-all

SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,output/%,$(SRCS))
OBJS = $(patsubst %.c,%.o,$(SRCS))
ETHAN_SRCS = $(wildcard ethan/*.c)
ETHAN_PROGS = $(patsubst ethan/%.c,ethan/%,$(ETHAN_SRCS))

.PHONY: install scp all clean ethan

output/snake: snake.c engine.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

output/space: space.c engine.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

output/space_too: space_too.c engine.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

output/tetris: tetris.c engine.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

output/remember: remember.c engine.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

output/menu: menu.c engine.o space.o snake.o tetris.o space_too.o remember.o
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

ethan/%: ethan/%.c
	$(CC) $(CFLAGS) -o $@ $< -L/home/addison/personal/hackathon-2023 -lfb

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: output/snake output/space output/tetris output/space_too output/remember output/menu

scp: install
	scp output/* root@192.168.75.140:/var/mnt/storage.usb/
	scp output/* im7200:/var/mnt/storage.usb/

ethan:
	rm -rf ethan/*
	scp root@192.168.75.140:/var/mnt/storage.usb/ethan/* ethan/
	make $(ETHAN_PROGS)
	scp ethan/* root@192.168.75.140:/var/mnt/storage.usb/ethan/

all: compile-ethan install scp

clean:
	rm -rf *.o
