CC = gcc -std=c23
CFLAGS = -fsanitize=address,undefined -g -Wall -Wextra -Wpedantic
BIN = nibble
SRC = main.c

all:
	$(TIME) $(CC) $(CFLAGS) $(SRC) -o $(BIN)
	make -C test
	make -C game
	make -C shaders

clean:
	rm -f $(BIN)
	make -C test clean
	make -C game clean
	make -C shaders clean
