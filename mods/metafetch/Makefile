CC=gcc
CFLAGS=-g -std=gnu99 -fPIC -I../../lib
LDFLAGS=-lcurl
SRC=./metafetch.c
OBJ=./mods/metafetch.so

main: $(OBJ)
	@echo "All Done!"

$(OBJ): $(SRC)
	mkdir -p mods
	$(CC) -shared -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ)
