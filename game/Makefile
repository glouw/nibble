CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang
BIN = game
LDFLAGS = -lSDL2 -lm

all:
	$(CC) $(SRC) > $(TEMP) && $(ASM) $(TEMP) $(LDFLAGS) -o $(BIN) && ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
