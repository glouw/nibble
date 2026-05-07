CC = ../nibble
ASM = clang
ASMFLAGS = -O2 -march=native -ffast-math
LDFLAGS = -lSDL2 -lm -lpthread
TIME = time

all: creation art

creation: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(ASMFLAGS) $(LDFLAGS) -o $@ && ./$@

art: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(ASMFLAGS) $(LDFLAGS) -o $@ && ./$@

clean:
	rm -f creation.ll
	rm -f creation
	rm -f art.ll
	rm -f art

force:
