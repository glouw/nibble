CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang
BIN = test
ASAN = valgrind

all:
	$(CC) $(SRC) > $(TEMP) && $(ASM) $(TEMP) -o $(BIN) && $(ASAN) ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
