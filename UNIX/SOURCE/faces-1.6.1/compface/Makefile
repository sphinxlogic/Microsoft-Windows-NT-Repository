#
#  Compface - 48x48x1 image compression and decompression
#
#  Copyright (c) James Ashton - Sydney University - June 1990.
#
#  Written 11th November 1989.
#
#  Permission is given to distribute these sources, as long as the
#  copyright messages are not removed, and no monies are exchanged. 
#
#  No responsibility is taken for any errors on inaccuracies inherent
#  either to the comments or the code of this program, but if reported
#  to me, then an attempt will be made to fix them.

# SYSV is expected to be overridden by the calling Makefile.
#--------------------------------------------------------------------------
#  If you are running on a Unix System V machine, then you should uncomment
#  the next definition.
#
#SYSV           = -DSYSV32
#--------------------------------------------------------------------------
# BINDIR, LIBDIR, and MANDIR are expected to be overridden by the
# calling Makefile

BINDIR		= /opt/bin
LIBDIR		= /opt/lib
MANDIR		= /opt/man/cat1

NAME		= compface
UNNAME		= uncompface
EXECUTABLE	= $(BINDIR)/$(NAME)
UNEXECUTABLE	= $(BINDIR)/$(UNNAME)
LIBNAME		= lib$(NAME).a
LIBRARY		= $(LIBDIR)/$(LIBNAME)
MAN1DIR		= $(MANDIR)/cat1
MAN3DIR		= $(MANDIR)/cat3
OBJECTS		= arith.o file.o compress.o gen.o uncompface.o
SOURCES		= compface.c uncompface.o arith.c file.c compress.c gen.c \
		  cmain.c uncmain.c
HDRS		= compface.h data.h
OTHERS		= README $(NAME).1 $(NAME).3 Makefile

CC		= gcc
CDEFS		= $(SYSV)
CCOMP		= -g -O3
CFLAGS		= $(CDEFS) $(CCOMP)

all:		$(NAME) $(UNNAME)

$(NAME) :	cmain.o compface.o $(LIBNAME)
		$(CC) $(CFLAGS) -o $(NAME) cmain.o compface.o $(LIBNAME)

$(UNNAME) :	uncmain.o $(LIBNAME)
		$(CC) $(CFLAGS) -o $(UNNAME) uncmain.o $(LIBNAME)

$(LIBNAME) :	$(OBJECTS)
		ar rc $(LIBNAME) $(OBJECTS)

lint :
		lint -abchx $(SOURCES)

clean :
		rm -f *.o *.a *.sh core a.out $(NAME) $(UNNAME)

install :	$(NAME) $(UNNAME) $(LIBNAME)
		-cp $(NAME) $(EXECUTABLE)
		strip $(EXECUTABLE)
		-cp $(UNNAME) $(UNEXECUTABLE)
		strip $(UNEXECUTABLE)
		-cp $(NAME).1 $(MAN1DIR)
		rm -f $(MAN1DIR)/$(UNNAME).1
		-ln $(MAN1DIR)/$(NAME).1 $(MAN1DIR)/$(UNNAME).1
		-cp $(LIBNAME) $(LIBRARY)
		-cp $(NAME).3 $(MAN3DIR)
		rm -f $(MAN3DIR)/$(UNNAME).3
		-ln $(MAN3DIR)/$(NAME).3 $(MAN3DIR)/$(UNNAME).3

install-sysv:	$(NAME) $(UNNAME) $(LIBNAME)
		-cp $(NAME) $(EXECUTABLE)
		strip $(EXECUTABLE)
		-cp $(UNNAME) $(UNEXECUTABLE)
		strip $(UNEXECUTABLE)
		nroff -man $(NAME).1 >$(MAN1DIR)/$(NAME).1
		rm -f $(MAN1DIR)/$(UNNAME).1
		-ln $(MAN1DIR)/$(NAME).1 $(MAN1DIR)/$(UNNAME).1
		if [ -d $(LIBDIR) ] ; then \
			true ; \
		else \
			mkdir $(LIBDIR) ; \
		fi
		-cp $(LIBNAME) $(LIBRARY)
		nroff -man $(NAME).3 > $(MAN3DIR)/$(NAME).3
		rm -f $(MAN3DIR)/$(UNNAME).3
		-ln $(MAN3DIR)/$(NAME).3 $(MAN3DIR)/$(UNNAME).3

shar :
		shar.script $(OTHERS) $(HDRS) $(SOURCES) > $(NAME).sh
		compress $(NAME).sh

arith.o:	arith.c compface.h data.h
cmain.o:	cmain.c compface.h data.h
compface.o:	compface.c compface.h data.h
compress.o:	compress.c compface.h data.h
file.o:		file.c compface.h data.h
gen.o:		gen.c compface.h data.h
uncmain.o:	uncmain.c compface.h data.h
uncompface.o:	uncompface.c compface.h data.h
