CC = clang -std=c23
CFLAGS = -fsanitize=address,undefined -g
WFLAGS = -Wall -Wextra -Wpedantic
BIN = nibble
SRC = main.c

all: nibble
	make -C test
	make -C chickens X=0 Y=0 XRES=640 YRES=480 &
	make -C shaders creation X=640 Y=0 XRES=640 YRES=480 &
	make -C shaders art X=1280 Y=0 XRES=640 YRES=480 &
	make -C tree X=0 Y=480 XRES=640 YRES=480 &
	make -C midi X=640 Y=480 XRES=640 YRES=480

nibble: force
	$(CC) $(CFLAGS) $(WFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN)
	make -C test clean
	make -C chickens clean
	make -C shaders clean
	make -C tree clean
	make -C midi clean

force:
