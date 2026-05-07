CC = ../nibble
SRC = creation.n
TEMP = out.ll
ASM = clang
ASMFLAGS = -O2 -march=native -ffast-math
BIN = creation
LDFLAGS = -lSDL2 -lm -lpthread
TIME = time

all:
	$(TIME) $(CC) $(SRC) > $(TEMP) && $(TIME) $(ASM) $(TEMP) $(ASMFLAGS) $(LDFLAGS) -o $(BIN) && ./$(BIN)

clean:
	rm -f $(TEMP)
	rm -f $(BIN)
