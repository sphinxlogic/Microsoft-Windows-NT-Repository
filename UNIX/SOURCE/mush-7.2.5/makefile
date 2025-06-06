# Mush makefile for system V.  Note: SIGRET should return void for normal
# sys-v, but Att PC users should *not* have it defined.  See the README!!
#
HDRS1= mush.h config.h
HDRS2= strings.h options.h
HDRS3= bindings.h glob.h
HDRS4= version.h pop.h
SRCS1= commands.c dates.c execute.c expr.c folders.c \
	hdrs.c init.c loop.c mail.c main.c misc.c msgs.c pick.c \
	print.c setopts.c signals.c sort.c viewopts.c options.c lock.c
SRCS2= bind.c curs_io.c curses.c file.c strings.c macros.c \
	addrs.c malloc.c glob.c command2.c pop.c pmush.c xcreat.c

OBJS1= commands.o dates.o execute.o expr.o folders.o \
	hdrs.o init.o loop.o mail.o main.o misc.o msgs.o pick.o \
	print.o setopts.o signals.o sort.o viewopts.o options.o lock.o
OBJS2= bind.o curs_io.o curses.o file.o strings.o macros.o \
	addrs.o malloc.o glob.o command2.o pop.o pmush.o xcreat.o

HELP= README README-7.0 README-7.1 README-7.2.0 README-7.2.2 \
	README-7.2.4 mush.1 cmd_help Mushrc Mailrc Gnurc \
	sample.mushrc advanced.mushrc digestify

# Sun OS systems who wish to compile with sys-v options:
# CC= /usr/5bin/cc
# CFLAGS= 	-O -DSYSV -DCURSES -DUSG -DDIRECTORY
# LIBS= -L/usr/5lib -lcurses

# IRIX 3.2 systems (SGI Iris workstations) should add -DDIRECTORY to CFLAGS
# SCO UNIX 3.2 should add -DDIRECTORY -DSELECT and should avoid library -lx
# System V Release 4 (SunOS 5?) should add -DSVR4 -DDIRECTORY to CFLAGS and
#  should replace -lPW with -lgen

CFLAGS= 	-O3 -g  -DSYSV -DUSG -DCURSES -DREGCMP -DSIGRET=void -DSVR4 -DDIRECTORY
LDFLAGS=
LIBS= 		-lcurses -lgen 
OTHERLIBS=
# Use some variant of this one if you #define MMDF in config.h
#OTHERLIBS=/usr/src/mmdf/lib/libmmdf.a
PROG=		mush

$(PROG): $(OBJS1) $(OBJS2)
	@echo loading...
	@$(CC) $(LDFLAGS) $(OBJS1) $(OBJS2) -o $(PROG) $(LIBS) $(OTHERLIBS)

$(OBJS1): $(HDRS1) $(HDRS2)
$(OBJS2): $(HDRS1) $(HDRS2) $(HDRS3)
loop.o: version.h

BINDIR= /opt/bin
LIBDIR= /etc/mail/mush
MRCDIR= /etc/mail/mush
MANDIR= /opt/man/cat1
MANEXT= 1

install: mush
	cp mush $(BINDIR)
	strip $(BINDIR)/mush
	chmod 0755 $(BINDIR)/mush
	cp mush.1 $(MANDIR)/mush.$(MANEXT)
	chmod 0644 $(MANDIR)/mush.$(MANEXT)
	cp cmd_help $(LIBDIR)
	chmod 0644 $(LIBDIR)/cmd_help
	cp Mushrc $(MRCDIR)/Mushrc
	chmod 0644 $(MRCDIR)/Mushrc

install-sysv: mush
	- mkdirhier /etc/mail/mush;
	cp mush $(BINDIR)
	strip $(BINDIR)/mush
	chmod 0755 $(BINDIR)/mush
	nroff -man mush.1 $(MANDIR)/mush.$(MANEXT)
	chmod 0644 $(MANDIR)/mush.$(MANEXT)
	cp cmd_help $(LIBDIR)
	chmod 0644 $(LIBDIR)/cmd_help
	cp Mushrc $(MRCDIR)/Mushrc
	chmod 0644 $(MRCDIR)/Mushrc

clean:
	rm -f *.o *~ core $(PROG)