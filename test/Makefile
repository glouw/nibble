CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang
BIN = test

all:
	$(CC) $(SRC) > $(TEMP) && $(ASM) $(TEMP) -o $(BIN) && ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
