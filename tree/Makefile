CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang -O1
BIN = tree
LDFLAGS = -lSDL2 -lm
TIME = time

all:
	$(TIME) $(CC) $(SRC) > $(TEMP) && $(TIME) $(ASM) $(TEMP) $(LDFLAGS) -o $(BIN) && ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
