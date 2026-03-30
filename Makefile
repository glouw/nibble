all:
	gcc -g -fsanitize=address,undefined -Wall -Wextra -Wpedantic -std=c23 main.c -o lang

clean:
	rm lang
