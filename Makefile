.SUFFIXES: .asm .d

VPATH = xprintf/src
SRCS = main.c midi.c file.c snd.c xprintf.c
CC = cmoc
CFLAGS = -I$(VPATH)
LDFLAGS = -ma.map -slinkerscript -L/usr/local/share/cmoc/lib -fraw \
	-lcmoc-crt-usim -lcmoc-std-usim

all: $(SRCS:.c=.d) a.out mid2l3

mid2l3: mid2l3.cpp
	c++ $< -o $@

a.out: crt0.o $(SRCS:.c=.o) linkerscript
	lwlink $(LDFLAGS) -o$@ crt0.o $(SRCS:.c=.o)
	f9dasm -6309 -offset c000 $@ > $(@:.out=.lst)

.c.o:
	$(CC) -i $(CFLAGS) -c $<

.c.d:
	$(CC) --deps-only $(CFLAGS) $<

.asm.o:
	lwasm -l$(<:.asm=.lst) -fobj -o$@ $<

clean:
	rm -f a.{map,out} *.{d,lst,o,s} mid2l3

ifneq ($(MAKECMDGOALS),clean)
-include $(SRCS:.c=.d)
endif
