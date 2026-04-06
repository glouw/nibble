CC = gcc -std=c23
CFLAGS = -Og -fsanitize=address,undefined -Wall -Wextra -Wpedantic
FRONTEND = lang
SRC = main.c
TEMP = out.ll
EXE = a.out
BACKEND = clang -O3

test: all
	./$(FRONTEND) > $(TEMP) && $(BACKEND) $(TEMP) && ./$(EXE)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(FRONTEND)

clean:
	rm -f $(FRONTEND)
	rm -f $(EXE)
	rm -f $(TEMP)
