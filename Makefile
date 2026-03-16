test: lang
	./lang tests/0.e > out.ll && clang -O3 out.ll && ./a.out && objdump -M intel -d a.out > out.asm

lang: main.c
	gcc -Wall -Wextra -Wpedantic -std=c23 main.c -o lang

clean:
	rm a.out
	rm lang
	rm out.ll
	rm out.asm
