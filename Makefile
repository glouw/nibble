test: lang
	./lang tests/0.e > out.ll && clang out.ll && ./a.out

lang: main.c
	gcc -Wall -Wextra -Wpedantic -std=c23 main.c -o lang
