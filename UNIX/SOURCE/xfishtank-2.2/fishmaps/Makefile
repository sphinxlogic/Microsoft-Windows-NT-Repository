# Makefile for fish bitmaps 
# Greg Lehey, LEMIS, 3 May 1993
CC=gcc
CFLAGS=-g -O3
LFLAGS=-g
NROFF=nroff
BINDIR=/opt/bin
LIBDIR=/opt/lib
MANDIR=/opt/man
CAT1DIR=cat1

all: libfish.a

libfish.a: collaris.o QueenAngel.o f02.o f08.o manta.o discus2.o blueking.o f03.o f09.o newf1.o discus3.o blueking2.o f04.o f10.o shark1.o eel.o six_barred.o butfish.o f05.o f11.o f00.o teeth.o cichlid1.o f06.o f12.o cichlid4.o f01.o f07.o f13.o
	ar r libfish.a collaris.o QueenAngel.o f02.o f08.o manta.o discus2.o blueking.o f03.o f09.o newf1.o discus3.o blueking2.o f04.o f10.o shark1.o eel.o six_barred.o butfish.o f05.o f11.o f00.o teeth.o cichlid1.o f06.o f12.o cichlid4.o f01.o f07.o f13.o


.h.o:
	$(CC) $(CFLAGS) -xc -c $<
	