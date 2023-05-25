CC = /home/addison/personal/hackathon-2023/armv5te-toolchain/.build/arm-unknown-linux-uclibcgnueabi/buildtools/bin/arm-unknown-linux-uclibcgnueabi-gcc
CFLAGS += -march=armv5te
CFLAGS += -Wl,--unresolved-symbols=ignore-all

SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,output/%,$(SRCS))
OBJS = $(patsubst %.c,%.o,$(SRCS))
ETHAN_SRCS = $(wildcard ethan/*.c)
ETHAN_PROGS = $(patsubst ethan/%.c,ethan/%,$(ETHAN_SRCS))

.PHONY: install scp all clean ethan

output/game: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -L/home/addison/personal/hackathon-2023 -lfb

ethan/%: ethan/%.c
	$(CC) $(CFLAGS) -o $@ $< -L/home/addison/personal/hackathon-2023 -lfb

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(PROGS)

scp: install
	scp output/* root@192.168.75.140:/var/mnt/storage.usb/
	scp output/* im7200:/var/mnt/storage.usb/

ethan:
	rm -rf ethan/*
	scp root@192.168.75.140:/var/mnt/storage.usb/ethan/* ethan/
	make $(ETHAN_PROGS)
	scp ethan/* root@192.168.75.140:/var/mnt/storage.usb/ethan/

all: compile-ethan install scp
