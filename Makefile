all:
	gcc -g -fsanitize=address,undefined -Wall -Wextra -Wpedantic -std=c23 main.c -o lang

clean:
	rm -f lang
	rm -f a.out
	rm -f out.ll
