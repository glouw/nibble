CC = gcc -std=c23
CFLAGS = #-fsanitize=address,undefined -Og -g -Wall -Wextra -Wpedantic
FRONTEND = lang
SRC = main.c
TEMP = out.ll
EXE = a.out
BACKEND = clang -g
TEST = test.n

test: all
	time ./$(FRONTEND) $(TEST) > $(TEMP) && time $(BACKEND) $(TEMP) && time valgrind --leak-check=full -s ./$(EXE)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(FRONTEND)

clean:
	rm -f $(FRONTEND)
	rm -f $(EXE)
	rm -f $(TEMP)
