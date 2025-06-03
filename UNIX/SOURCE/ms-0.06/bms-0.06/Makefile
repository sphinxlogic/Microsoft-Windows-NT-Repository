#
# Makefile for bms, the batch-mode MandelSpawn client program
#
# This doesn't use Imake because it isn't an X program.
#

# uncomment this if you have GCC (not nearly as important as for mslaved)
CC=gcc

LD=$(CC)

INSTALL=/usr/ucb/install
TARGETLIB=/opt
MANLIB=$(TARGETLIB)/man/cat1

# You may need to:
#  * add -DNO_MALLOC_H to DEFINES if you don't have <malloc.h>
#  * add -DNEED_SYS_SELECT_H to DEFINES if you need to include <sys/select.h>
#     to define getdtablesize() or the fd_set type
DEFINES=

CFLAGS= -O3 -g $(DEFINES) -DSVR4

LDFLAGS=-L/usr/ucblib -lsocket -lnsl -lucb

OBJS=bms.o work.o mspawn.o

all: bms

bms: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o bms

bms.o: bms.c work.h mspawn.h ms_job.h ms_real.h
work.o: work.c work.h ms_ipc.h ms_real.h
mspawn.o: mspawn.c mspawn.h ms_ipc.h ms_job.h ms_real.h

clean:
	rm -f core bms *.o *~

# These targets added by LEMIS

doc:	bms.1
	nroff -man bms.1 >bms.0

install: bms bms.1
	install -c bms $(TARGETLIB)/bin/bms
	install -c bms.0 $(MANLIB)/bms.1

install-sysv: bms bms.1
	install -c bms $(TARGETLIB)/bin/bms
	$(NROFF) -man bms.0  > $(MANLIB)/bms.1

