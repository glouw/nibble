CC = ../nibble
SRC = main.n
TEMP = out.ll
ASM = clang
BIN = test
ASAN = valgrind
TIME = time

all:
	$(TIME) $(CC) $(SRC) > $(TEMP) && $(TIME) $(ASM) $(TEMP) -o $(BIN) && $(ASAN) ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
