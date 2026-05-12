CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang -O1
BIN = chickens
LDFLAGS = -lSDL2 -lm
TIME = time
X=0
Y=0
XRES=640
YRES=480

all:
	$(TIME) $(CC) $(SRC) > $(TEMP) && $(TIME) $(ASM) $(TEMP) $(LDFLAGS) -o $(BIN) && ./$(BIN) $(X) $(Y) $(XRES) $(YRES)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
