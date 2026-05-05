CC = gcc -std=c23
CFLAGS = -Wall -Wextra -Wpedantic
BIN = nibble
SRC = main.c

all:
	$(TIME) $(CC) $(CFLAGS) $(SRC) -o $(BIN)
	make -C test
	make -C game

clean:
	rm -f $(BIN)
	make -C test clean
	make -C game clean
