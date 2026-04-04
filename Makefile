all:
	gcc -g -fsanitize=address,undefined -Wall -Wextra -Wpedantic -std=c2y main.c -o lang

clean:
	rm -f lang
	rm -f a.out
	rm -f out.ll
