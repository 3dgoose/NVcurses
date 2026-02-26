CC = tcc

all:
	$(CC) main.c -lnvidia-ml -o nvcurses
	$(CC) test.c -lncurses -o test
