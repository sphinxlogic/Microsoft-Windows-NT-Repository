# Makefile for XC
SHELL=/bin/sh

all \
xc \
install \
install.suid \
install.scripts \
xc.man \
install.man \
clean \
shar \
tah \
taz \
lha \
shark:			Makefile.xc xc.h
				make $(MFLAGS) -f Makefile.xc $@

Makefile.xc:	Makefile.in Configure
				./Configure

xc.h:			xc.h.in Configure
				./Configure
