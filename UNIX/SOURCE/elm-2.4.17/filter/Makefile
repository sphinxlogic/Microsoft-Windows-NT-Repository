#
# @(#): Makefile.SH,v 5.2 1992/11/15 01:40:43 syd Exp $
#  Makefile for the Elm system filter program
#
#         (C) Copyright 1986,1987, by Dave Taylor
#         (C) Copyright 1988-1992, USENET Community Trust
#
# Bug reports, patches, comments, suggestions should be sent to:
#
#	Syd Weinstein - elm@DSI.COM
#			dsinc!elm
#
#  $Log: Makefile.SH,v $
# Revision 5.2  1992/11/15  01:40:43  syd
# Add regexp processing to filter.
# Add execc operator
# From: Jan Djarv <Jan.Djarv@sa.erisoft.se>
#
# Revision 5.1  1992/10/03  22:18:09  syd
# Initial checkin as of 2.4 Release at PL0
#
#
# Variables
#	Variables established by Configure
CC		=	gcc
CCFLAGS		=	-fpcc-struct-return -traditional 
CHGRP		=	chgrp
CHMOD		=	chmod
CP		=	cp
DEST		=	/opt/bin
ECHO		=	echo
LFLAGS		=	-g 
LIB		=	/opt/lib/elm
LIB2		= 	-lnls
LIBS		=	-ltermlib 
LINT		=	/bin/lint
MAILGRP		=	uucp
MAILERMODE	=	2755
MAKE		=	make
MV		=	mv
OPTIMIZE	=	-O3 -g
RM		= 	rm -f
TOUCH		=	touch

#	Variables you may want to manually edit
#		If you want debug logging then you'll
#		want to uncomment the following.
#DEBUG		=	-DDEBUG

#	Other variables
BIN		=	../bin
INCLDIR		=	../hdrs
CFLAGS		=	$(CCFLAGS) $(OPTIMIZE) -I$(INCLDIR) $(DEBUG) $(DACSNET) 
LINTFLAGS	=	-I$(INCLDIR)
SHELL		=	/bin/sh

# Definitions of variables
FILTER_SRC	=	actions.c	\
			buffer.c	\
			filter.c	\
			lock.c		\
			parse.c		\
			regexp.c	\
			rules.c		\
			summarize.c	\
			utils.c

FILTER_OBJ	=	actions.o	\
			buffer.o	\
			filter.o	\
			lock.o		\
			parse.o		\
			regexp.o	\
			rules.o		\
			summarize.o	\
			utils.o		\
			../lib/libutil.a

# Standard targets
all:		$(BIN)/filter
		
install:	$(DEST)/filter

uninstall:	
		$(RM) $(DEST)/filter

#	This is the only target that gets installed even if not out-of-date
#	with respect the files from which it is installed.
rmt-install:	rmt-defined
		-$(MV) $(DEST)/filter $(DEST)/filter.old
		-$(RM) $(DEST)/filter.old
		$(CP) $(REMOTE)$(DEST)/filter $(DEST)/filter
		$(CHGRP) $(MAILGRP) $(DEST)/filter
		$(CHMOD) $(MAILERMODE) $(DEST)/filter

rmt-defined:
	@(if [ "$(REMOTE)" = "" ];\
	  then\
	    $(ECHO) "You need to define 'REMOTE' as the remote file system";\
	    $(ECHO) "for this particular command. The easiest way to do this";\
	    $(ECHO) "to type:";\
	    $(ECHO) "        make REMOTE=<remote file system> rmt-install";\
	    exit 1;\
	  fi);

lint:		
		$(LINT) $(LINTFLAGS) $(FILTER_SRC) > LINT.OUT

clean:		
		$(RM) $(FILTER_OBJ) $(BIN)/filter

# Dependencies and rules
#	Dependencies of header files upon other header files they include
.PRECIOUS:		$(INCLDIR)/defs.h $(INCLDIR)/elm.h $(INCLDIR)/headers.h

$(INCLDIR)/defs.h:	$(INCLDIR)/../config.h $(INCLDIR)/sysdefs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

$(INCLDIR)/elm.h:	$(INCLDIR)/curses.h $(INCLDIR)/defs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

$(INCLDIR)/headers.h:	$(INCLDIR)/curses.h $(INCLDIR)/defs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

#	Dependencies and rules for C object files
actions.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
filter.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
lock.o:			$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
parse.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
regexp.o:		$(INCLDIR)/defs.h $(INCLDIR)/regexp.h
rules.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
summarize.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h
utils.o:		$(INCLDIR)/defs.h $(INCLDIR)/filter.h $(INCLDIR)/s_filter.h

#	Dependencies and rules for compiling programs
$(BIN)/filter:	$& $(FILTER_OBJ)
		$(CC) $(LFLAGS) -o $@ $(FILTER_OBJ) $(LIB2)

#	Dependencies and rules for installing programs from bin directory
$(DEST)/filter:		$(BIN)/filter
			-$(MV) $(DEST)/filter $(DEST)/filter.old
			-$(RM) $(DEST)/filter.old
			$(CP) $? $@
			$(CHGRP) $(MAILGRP) $@
			$(CHMOD) $(MAILERMODE) $@
