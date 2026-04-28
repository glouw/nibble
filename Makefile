CC = gcc -std=c23
CFLAGS = -fsanitize=address,undefined -Og -g -Wall -Wextra -Wpedantic
FRONTEND = lang
SRC = main.c
TEMP = out.ll
EXE = a.out
BACKEND = clang -O2
TEST = test.n
GAME = game.n
VALGRIND = valgrind
TIME = time
GAME_FLAGS = -lSDL2

test: all
	$(TIME) ./$(FRONTEND) $(TEST) > $(TEMP) && $(TIME) $(BACKEND) $(TEMP) && $(TIME) $(VALGRIND) ./$(EXE)

game: all
	$(TIME) ./$(FRONTEND) $(GAME) > $(TEMP) && $(TIME) $(BACKEND) $(TEMP) $(GAME_FLAGS) && ./$(EXE)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(FRONTEND)

clean:
	rm -f $(FRONTEND)
	rm -f $(EXE)
	rm -f $(TEMP)
