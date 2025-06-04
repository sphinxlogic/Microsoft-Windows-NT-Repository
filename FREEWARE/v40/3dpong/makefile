# makefile for 3dpong

# Math 222 Final Project
# Object spinner

# by Bill Kendrick
# December 9, 1997 - December 15, 1997


# Makefile user-definable variables

CC=gcc
XLIB=-L/usr/X11/lib -lX11
# XLIB=-L/usr/openwin/lib -lX11
MATHLIB=-lm


# Makefile other variables

OBJECTS=3dpong.o window.o connect.o hints.o visual.o gc.o color.o randnum.o \
	text.o


# Makefile commands: 

all:	3dpong
	chmod 755 go handball sounds
	chmod 644 *.c *.h makefile sounds/*.au

clean:
	rm *.o


# Application:

3dpong:	$(OBJECTS)
	$(CC)	$(OBJECTS) $(XLIB) $(MATHLIB) -o 3dpong
	chmod 755 3dpong
	strip 3dpong


# Application object:

3dpong.o:	3dpong.c window.h connect.h hints.h visual.h gc.h color.h \
		randnum.h text.h
	$(CC)	3dpong.c -c


# Library objects:

window.o:	window.c window.h
	$(CC)	window.c -c

connect.o:	connect.c connect.h
	$(CC)	connect.c -c

hints.o:	hints.c hints.h
	$(CC)	hints.c -c

visual.o:	visual.c visual.h
	$(CC)	visual.c -c

gc.o:	gc.c gc.h
	$(CC)	gc.c -c

color.o:	color.c color.h
	$(CC)	color.c -c

randnum.o:	randnum.c randnum.h
	$(CC)	randnum.c -c

text.o:		text.c text.h
	$(CC)	text.c -c
