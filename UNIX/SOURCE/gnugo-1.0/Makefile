# program to play the game of Go (Wei-Chi)
# Makefile completed by Greg Lehey, LEMIS, 11 May 1993
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT6DIR=cat6
INFODIR=/opt/info

SRC = count.c countlib.c endgame.c eval.c exambord.c findcolr.c \
      findnext.c findopen.c findpatn.c findsavr.c findwinr.c \
      fioe.c genmove.c getij.c getmove.c initmark.c main.c matchpat.c \
      opening.c openregn.c random.c seed.c sethand.c \
      showbord.c showinst.c suicide.c

PRG = gnugo

OBJ = $(SRC:.c=.o)

CFLAGS = -g  -O3

$(PRG) : $(OBJ)
	$(LINK.c) $(OBJ) -o $@

matchpat.o : patterns.c


install:
	cp gnugo $(BINDIR)
	mkdirhier $(MANDIR)/$(CAT6DIR)
	cat  Documentation > $(MANDIR)/$(CAT6DIR)/gnugo.1

clean:
	rm -f gnugo *.o *~