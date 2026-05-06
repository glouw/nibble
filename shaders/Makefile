CC = ../nibble
SRC = creation.n
TEMP = out.ll
ASM = clang -O3 -march=native
BIN = creation
LDFLAGS = -lSDL2 -lm

all:
	$(CC) $(SRC) > $(TEMP) && $(ASM) $(TEMP) $(LDFLAGS) -o $(BIN) && ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
