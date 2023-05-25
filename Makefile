#CC = arm-linux-gnueabi-gcc # or whichever cross-compiler you have installed
CC = /home/addison/personal/hackathon-2023/armv5te-toolchain/.build/arm-unknown-linux-uclibcgnueabi/buildtools/bin/arm-unknown-linux-uclibcgnueabi-gcc
CFLAGS = -march=armv5te
TARGET = output/buttons

.PHONY: install scp all

install:
	$(CC) $(CFLAGS) -o $(TARGET) buttons.c

scp: install
	scp output/* root@192.168.75.140:/var/mnt/storage.usb/
	scp output/* im7200:/var/mnt/storage.usb/

all: install scp
