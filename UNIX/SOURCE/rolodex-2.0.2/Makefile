# Makefile for rolo - UNIX generic version

# Configure for your type of terminal capability database.  
#  if you have termcap,  set TERMSTUFF= -DTERMCAP,  and LIBS= -ltermcap
#  if you have terminfo, set TERMSTUFF= -DTERMINFO, and LIBS= -lterminfo
#  if you don't have either, leave them both blank (only "clear_screen" is used)
TERMSTUFF= -DTERMINFO
LIBS= -lcurses
# This should describe the install target: install-manman for unformatted
# man pages, install-catman for formatted pages. Both targets install the
# binaries in the same manner.
INSTALL=install-catman
NROFF=nroff
#
CC = gcc
RLIB    = /opt/lib/rolo
ROLOLIB = \"$(RLIB)\"
TOOLDIR = ./toolsdir
CFLAGS = -DUNIX -DROLOLIB=$(ROLOLIB) $(TERMSTUFF) -I$(TOOLDIR) -O3
DBGFLAG = -g
SOURCES = clear.c io.c menuaux.c operatns.c options.c \
	rlist.c rolo.c search.c update.c
OBJECTS = clear.o io.o menuaux.o operatns.o options.o \
	rlist.o rolo.o search.o update.o
TOOLOBJ = $(TOOLDIR)/mem.o $(TOOLDIR)/args.o $(TOOLDIR)/ctools.o \
	$(TOOLDIR)/menu.o

BIN = /opt/bin
MS = 1
MAN = /opt/man/man$(MS)
CAT = /opt/man/cat$(MS)

all: 	rolo

rolo: $(OBJECTS)
	cd $(TOOLDIR) ; make tools ; cd ..
	$(CC) -o rolo $(DBGFLAG) $(CFLAGS) $(OBJECTS) $(TOOLOBJ) $(LIBS)

install: $(INSTALL)

install-manman:  all install-binaries man

install-catman:  all install-binaries cat

install-binaries: rolo
	rm -f $(BIN)/rolo
	mv rolo $(BIN)/rolo
	chown bin $(BIN)/rolo
	chgrp bin $(BIN)/rolo
	chmod 111 $(BIN)/rolo

	- mkdir $(RLIB)
	- rm -f $(RLIB)/*.*
	chown bin $(RLIB)
	chgrp bin $(RLIB)
	chmod 775 $(RLIB)

	cp helplib/*.* $(RLIB)
	chown bin $(RLIB)/*.*
	chgrp bin $(RLIB)/*.*
	chmod 444 $(RLIB)/*.*

man: 
	rm -f $(MAN)/rolo.1
	cp rolo.1 $(MAN)/rolo.$(MS)
	chown bin $(MAN)/rolo.$(MS)
	chgrp bin $(MAN)/rolo.$(MS)
	chmod 444 $(MAN)/rolo.$(MS)

cat: 
	rm -f $(CAT)/rolo.1
	$(NROFF) -man rolo.1  > $(CAT)/rolo.$(MS)
	chown bin $(CAT)/rolo.$(MS)
	chgrp bin $(CAT)/rolo.$(MS)
	chmod 444 $(CAT)/rolo.$(MS)

clean:
	rm -f $(OBJECTS) rolo *~ foo $(TOOLDIR)/*.o

shar:
	cshar README rolo.1 Makefile helplib > roloshar1
	cshar *.[ch] > roloshar2
	cshar toolsdir > roloshar3
