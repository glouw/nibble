CC = ../nibble
ASM = clang -O3
LDFLAGS = -lSDL2 -lm -lpthread
TIME = time

all: creation art

creation: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(LDFLAGS) -o $@ && ./$@

art: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(LDFLAGS) -o $@ && ./$@

clean:
	rm -f creation.ll
	rm -f creation
	rm -f art.ll
	rm -f art

force:
