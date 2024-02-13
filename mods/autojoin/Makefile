CC=gcc
CFLAGS=-g -std=gnu99 -lconfig -fPIC -I../../lib
OBJ=../autojoin.so

main:
	$(CC) -shared -o $(OBJ) $(CFLAGS) ./autojoin.c
	@echo "All Done!"