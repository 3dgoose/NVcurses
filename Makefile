CC = gcc # Change to tcc for lighter binaries
CFLAGS = -std=c99 -lnvidia-ml -lncurses

all:
	$(CC) main.c $(CFLAGS) -o nvcare
