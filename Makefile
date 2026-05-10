CC = clang -std=c23
CFLAGS = -fsanitize=address,undefined -g -Wall -Wextra -Wpedantic
BIN = nibble
SRC = main.c

all: nibble
	make -C test
	make -C game
	make -C shaders

nibble: force
	$(TIME) $(CC) $(CFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN)
	make -C test clean
	make -C game clean
	make -C shaders clean

force:
