CC=gcc
CFLAGS=-g -std=gnu99 -lconfig -fPIC -I../../lib
OBJ=../sysinfo.so

main:
	$(CC) -shared -o $(OBJ) $(CFLAGS) ./sysinfo.c
	@echo "All Done!"
