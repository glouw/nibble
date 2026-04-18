CC = gcc -std=c23
CFLAGS = -Og -fsanitize=address,undefined -Wall -Wextra -Wpedantic
FRONTEND = lang
SRC = main.c
TEMP = out.ll
EXE = a.out
BACKEND = clang -O2

test: all
	time ./$(FRONTEND) > $(TEMP) && time $(BACKEND) $(TEMP) && time ./$(EXE)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(FRONTEND)

clean:
	rm -f $(FRONTEND)
	rm -f $(EXE)
	rm -f $(TEMP)
