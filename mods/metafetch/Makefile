CC=gcc
CFLAGS=-g -std=gnu99 -I../../lib
LDFLAGS=
SRC=./metafetch.c
OBJ=./mods/metafetch.so

main: $(OBJ)

$(OBJ): $(SRC)
	$(CC) -shared -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@echo "All Done!"

clean:
	rm -f $(OBJ)
