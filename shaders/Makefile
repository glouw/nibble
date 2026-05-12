CC = ../nibble
ASM = clang -O1
LDFLAGS = -lSDL2 -lm -lpthread
TIME = time
X=0
Y=0
XRES=640
YRES=480

all: creation art

creation: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(LDFLAGS) -o $@ && ./$@ $(X) $(Y) $(XRES) $(YRES)

art: force
	$(TIME) $(CC) $@.n > $@.ll && $(TIME) $(ASM) $@.ll $(LDFLAGS) -o $@ && ./$@ $(X) $(Y) $(XRES) $(YRES)

clean:
	rm -f creation.ll
	rm -f creation
	rm -f art.ll
	rm -f art

force:
