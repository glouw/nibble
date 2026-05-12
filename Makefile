CC = clang -std=c23
CFLAGS = -fsanitize=address,undefined -g
WFLAGS = -Wall -Wextra -Wpedantic
BIN = nibble
SRC = main.c

all: nibble
	make -C test
	make -C game 			 X=0   Y=0   XRES=640 YRES=480 &
	make -C shaders creation X=640 Y=0   XRES=640 YRES=480 &
	make -C shaders art	     X=0   Y=480 XRES=640 YRES=480 &
	make -C tree 			 X=640 Y=480 XRES=640 YRES=480

nibble: force
	$(CC) $(CFLAGS) $(WFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN)
	make -C test clean
	make -C game clean
	make -C shaders clean
	make -C tree clean

force:
