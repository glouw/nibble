CC = gcc -std=c23
CFLAGS = -fsanitize=address,undefined -Og -Wall -Wextra -Wpedantic
FRONTEND = lang
SRC = main.c
TEMP = out.ll
EXE = a.out
BACKEND = clang -g

test: all
	time ./$(FRONTEND) > $(TEMP) && time $(BACKEND) $(TEMP) && time valgrind --leak-check=full -s ./$(EXE)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(FRONTEND)

clean:
	rm -f $(FRONTEND)
	rm -f $(EXE)
	rm -f $(TEMP)
