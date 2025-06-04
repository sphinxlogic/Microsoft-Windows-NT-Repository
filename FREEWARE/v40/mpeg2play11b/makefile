# Makefile for mpeg2play

# Copyright (C) 1994, MPEG Software Simulation Group. All Rights Reserved.

#
# Disclaimer of Warranty
#
# These software programs are available to the user without any license fee or
# royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
# any and all warranties, whether express, implied, or statuary, including any
# implied warranties or merchantability or of fitness for a particular
# purpose.  In no event shall the copyright-holder be liable for any
# incidental, punitive, or consequential damages of any kind whatsoever
# arising from the use of these programs.
#
# This disclaimer of warranty extends to the user of these programs and user's
# customers, employees, agents, transferees, successors, and assigns.
#
# The MPEG Software Simulation Group does not represent or warrant that the
# programs furnished hereunder are free of infringement of any third-party
# patents.
#
# Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
# are subject to royalty fees to patent holders.  Many of these patents are
# general enough such that they are unavoidable regardless of implementation
# design.
#
#

LIBS = -lX11

# uncomment the following two lines if you want to use shared memory
# (faster display if server and client run on the same machine)

USE_SHMEM = -DSH_MEM
LIBS = -lXext -lX11

# uncomment the following line to activate calculation of decoding speed
# (frames per second) and frame rate control (-fn option)

USE_TIME = -DUSE_TIME

# if your X11 include files / libraries are in a non standard location:
# set INCLUDEDIR to -I followed by the appropriate include file path and
# set LIBRARYDIR to -L followed by the appropriate library path and

#INCLUDEDIR = -I/usr/include
#LIBRARYDIR = -L/usr/lib

# select one of the following CC CFLAGS settings

#
# GNU gcc
#
CC = gcc
CFLAGS = -O2 $(USE_DISP) $(USE_SHMEM) $(USE_TIME) $(INCLUDEDIR)

#
# SPARCworks acc
#
#CC = acc
#CFLAGS = -fast $(USE_DISP) $(USE_SHMEM) $(USE_TIME) $(INCLUDEDIR)

#
# SUN cc
#
#CC = cc
#CFLAGS = -O3 -DNON_ANSI_COMPILER $(USE_SHMEM) $(USE_TIME) $(INCLUDEDIR)

OBJ = mpeg2dec.o getpic.o motion.o getvlc.o gethdr.o getblk.o getbits.o recon.o spatscal.o idct.o display.o

all: mpeg2play

clean:
	rm -f *.o *% core mpeg2play

mpeg2play: $(OBJ)
	$(CC) $(CFLAGS) $(LIBRARYDIR) -o mpeg2play $(OBJ) $(LIBS)

getbits.o: config.h global.h mpeg2dec.h
getblk.o: config.h global.h mpeg2dec.h
gethdr.o: config.h global.h mpeg2dec.h
getpic.o: config.h global.h mpeg2dec.h
getvlc.o: config.h global.h mpeg2dec.h getvlc.h
idct.o: config.h
motion.o: config.h global.h mpeg2dec.h
mpeg2dec.o: config.h global.h mpeg2dec.h
recon.o: config.h global.h mpeg2dec.h
spatscal.o: config.h global.h mpeg2dec.h
display.o: config.h global.h mpeg2dec.h
