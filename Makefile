CC=gcc
CFLAGS=-g  -std=gnu99 -c -lconfig -ldl -I./lib
BINFLAGS=-g -rdynamic -ldl -lconfig 
SRC=./src
OBJ=./build
OBJECTS=$(OBJ)/*.o
EXEC=xbot

.PHONY: mods

main:
	@rm -rf build
	@mkdir build
	$(CC) $(CFLAGS) $(SRC)/main.c   -o $(OBJ)/main.o
	$(CC) $(CFLAGS) $(SRC)/irc.c    -o $(OBJ)/irc.o
	$(CC) $(CFLAGS) $(SRC)/util.c   -o $(OBJ)/util.o
	$(CC) $(CFLAGS) $(SRC)/events.c -o $(OBJ)/events.o
	$(CC) $(CFLAGS) $(SRC)/module.c -o $(OBJ)/module.o
	$(CC) -o $(EXEC) $(OBJECTS) $(BINFLAGS)
	@echo "All Done!"

mods:
	$(MAKE) -C mods/test

clean:
	@rm -rf build $(EXEC)