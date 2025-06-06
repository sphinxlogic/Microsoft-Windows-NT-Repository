
# @(#): Makefile.SH,v 5.1 1992/10/04 00:46:45 syd Exp $
#
#  Makefile for the Elm system utilities
#
#         (C) Copyright 1986,1987, by Dave Taylor
#         (C) Copyright 1988-1992, USENET Community Trust
#
# Bug reports, patches, comments, suggestions should be sent to:
#
#	Syd Weinstein - elm@DSI.COM
#			dsinc!elm
#
# $Log: Makefile.SH,v $
# Revision 5.1  1992/10/04  00:46:45  syd
# Initial checkin as of 2.4 Release at PL0
#
# 
#
# Variables
#	Variables established by Configure
CC		=	gcc
CCFLAGS		=	-fpcc-struct-return -traditional 
CHMOD		=	chmod
CP		=	cp
DEST		=	/opt/bin
ECHO		=	echo
LFLAGS		=	-g 
LIB		=	/opt/lib/elm
LIB2		= 	-lnls
LIBS		=	-ltermlib 
LINT		=	/bin/lint
LN		=	ln
MAKE		=	make
MV		=	mv
OPTIMIZE	=	-O3 -g
RM		= 	rm -f
TOUCH		=	touch
#	Variables you may want to manually edit
#		If you want debug logging then you'll
#		want to uncomment the following.
#DEBUG		=	-DDEBUG

#	Other general variables
BIN		=	../bin
CFLAGS		=	$(CCFLAGS) $(OPTIMIZE) -I$(INCLDIR) $(DEBUG) $(DACSNET) 
INCLDIR		=	../hdrs
LINTFLAGS	=	-I$(INCLDIR)
SHELL		=	/bin/sh

#	Lists
#		List of installed programs - excludes wnewmail and nfrm,
#		which are handled separately
INSTALL_LIST	=	$(DEST)/answer		\
			$(DEST)/arepdaemon	\
			$(DEST)/autoreply	\
			$(DEST)/checkalias	\
			$(DEST)/fastmail	\
			$(DEST)/frm		\
			$(DEST)/listalias	\
			$(DEST)/messages	\
			$(DEST)/newalias	\
			$(DEST)/newmail		\
			$(DEST)/printmail	\
			$(DEST)/readmsg

#		List of remotely install programs
REMOTE_LIST	=	$(REMOTE)$(DEST)/answer		\
			$(REMOTE)$(DEST)/arepdaemon	\
			$(REMOTE)$(DEST)/autoreply	\
			$(REMOTE)$(DEST)/checkalias	\
			$(REMOTE)$(DEST)/fastmail	\
			$(REMOTE)$(DEST)/frm		\
			$(REMOTE)$(DEST)/listalias	\
			$(REMOTE)$(DEST)/messages	\
			$(REMOTE)$(DEST)/newalias	\
			$(REMOTE)$(DEST)/newmail	\
			$(REMOTE)$(DEST)/printmail	\
			$(REMOTE)$(DEST)/readmsg

#		List of programs in bin directory
BINARY_LIST	=	$(BIN)/answer		\
			$(BIN)/arepdaemon	\
			$(BIN)/autoreply	\
			$(BIN)/checkalias	\
			$(BIN)/fastmail		\
			$(BIN)/frm		\
			$(BIN)/listalias	\
			$(BIN)/messages		\
			$(BIN)/newalias		\
			$(BIN)/newmail		\
			$(BIN)/printmail	\
			$(BIN)/readmsg

#		List of programs to $(LINT) - only C programs
LINT_LIST	=	answer_lint	\
			arepdaemon_lint	\
			autoreply_lint	\
			fastmail_lint	\
			frm_lint	\
			listalias_lint	\
			newalias_lint	\
			newmail_lint	\
			readmsg_lint

#	List of all object files in all util programs (used in parallel makes)
UTIL_OBJ	=	answer.o		\
			arepdaem.o 		\
			autoreply.o		\
			fastmail.o		\
			from.o			\
			listalias.o		\
			newalias.o		\
			newmail.o		\
			readmsg.o

#	Lists of source and object files for each C program
ANSWER_SRC	=	answer.c
ANSWER_OBJ	=	answer.o
AREPDAEMON_SRC	=	arepdaem.c
AREPDAEMON_OBJ	=	arepdaem.o
AUTOREPLY_SRC	=	autoreply.c
AUTOREPLY_OBJ	=	autoreply.o
FASTMAIL_SRC	=	fastmail.c
FASTMAIL_OBJ	=	fastmail.o
FRM_SRC		=	from.c
FRM_OBJ		=	from.o
LISTALIAS_SRC	=	listalias.c
LISTALIAS_OBJ	=	listalias.o
NEWALIAS_SRC	=	newalias.c
NEWALIAS_OBJ	=	newalias.o
NEWMAIL_SRC	=	newmail.c
NEWMAIL_OBJ	=	newmail.o
READMSG_SRC	=	readmsg.c
READMSG_OBJ	=	readmsg.o

# Standard targets
all:			objects $(BINARY_LIST)

#	This unusual target enables highly efficial compilation of object files
#	on systems that have the parallel make feature.
objects:		$& $(UTIL_OBJ)

install:		$(INSTALL_LIST) $(DEST)/wnewmail $(DEST)/nfrm

uninstall:		
			$(RM) $(INSTALL_LIST) $(DEST)/wnewmail $(DEST)/nfrm

#	This is the only target that gets installed even if not out-of-date
#	with respect the files from which it is installed.
rmt-install:		rmt-defined
			-$(MV) $(DEST)/answer $(DEST)/answer.old
			-$(MV) $(DEST)/arepdaemon $(DEST)/arepdaemon.old
			-$(MV) $(DEST)/autoreply $(DEST)/autoreply.old
			-$(MV) $(DEST)/checkalias $(DEST)/checkalias.old
			-$(MV) $(DEST)/fastmail $(DEST)/fastmail.old
			-$(MV) $(DEST)/frm $(DEST)/frm.old
			-$(MV) $(DEST)/nfrm $(DEST)/nfrm.old
			-$(MV) $(DEST)/listalias $(DEST)/listalias.old
			-$(MV) $(DEST)/messages $(DEST)/messages.old
			-$(MV) $(DEST)/newalias $(DEST)/newalias.old
			-$(MV) $(DEST)/newmail $(DEST)/newmail.old
			-$(MV) $(DEST)/printmail $(DEST)/printmail.old
			-$(MV) $(DEST)/readmsg $(DEST)/readmsg.old
			-$(MV) $(DEST)/wnewmail $(DEST)/wnewmail.old
			-$(RM) $(DEST)/answer.old
			-$(RM) $(DEST)/arepdaemon.old
			-$(RM) $(DEST)/autoreply.old
			-$(RM) $(DEST)/checkalias.old
			-$(RM) $(DEST)/fastmail.old
			-$(RM) $(DEST)/frm.old
			-$(RM) $(DEST)/nfrm.old
			-$(RM) $(DEST)/listalias.old
			-$(RM) $(DEST)/messages.old
			-$(RM) $(DEST)/newalias.old
			-$(RM) $(DEST)/newmail.old
			-$(RM) $(DEST)/printmail.old
			-$(RM) $(DEST)/readmsg.old
			-$(RM) $(DEST)/wnewmail.old
			$(CP) $(REMOTE_LIST) $(DEST)
			$(LN) $(DEST)/frm $(DEST)/nfrm
			$(LN) $(DEST)/newmail $(DEST)/wnewmail
			$(CHMOD) a+rx $(INSTALL_LIST)

rmt-defined:
	@(if [ "$(REMOTE)" = "" ];\
	  then\
	    $(ECHO) "You need to define 'REMOTE' as the remote file system";\
	    $(ECHO) "for this particular command. The easiest way to do this";\
	    $(ECHO) "to type:";\
	    $(ECHO) "        make REMOTE=<remote file system> rmt-install";\
	    exit 1;\
	  fi);

#	This rule allows us to put lint output for each program on the
#	same file, but make sure we start off fresh each time.
lint:			
			$(RM) LINT.OUT; $(MAKE) -$(MAKEFLAGS) $(LINT_LIST)

clean:			
			$(RM) $(UTIL_OBJ) $(BINARY_LIST)

# Dependencies and rules
#	Dependencies and rules for compiling and linting C programs
.PRECIOUS:		$(INCLDIR)/defs.h $(INCLDIR)/elm.h \
			$(INCLDIR)/headers.h ../lib/libutil.a

$(BIN)/answer:		$(ANSWER_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(ANSWER_OBJ) ../lib/libutil.a $(LIB2)

answer_lint:		$(ANSWER_SRC)
			$(LINT) $(LINTFLAGS) $(ANSWER_SRC) >> LINT.OUT

$(BIN)/arepdaemon:	$(AREPDAEMON_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(AREPDAEMON_OBJ) ../lib/libutil.a $(LIB2)

arepdaemon_lint:	$(AREPDAEMON_SRC)
			$(LINT) $(LINTFLAGS) $(AREPDAEMON_SRC) >> LINT.OUT

$(BIN)/autoreply:	$(AUTOREPLY_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(AUTOREPLY_OBJ) ../lib/libutil.a $(LIB2)

autoreply_lint:		$(AUTOREPLY_SRC)
			$(LINT) $(LINTFLAGS) $(AUTOREPLY_SRC) >> LINT.OUT

$(BIN)/fastmail:	$(FASTMAIL_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(FASTMAIL_OBJ) ../lib/libutil.a $(LIB2)

fastmail_lint:		$(FASTMAIL_SRC)
			$(LINT) $(LINTFLAGS) $(FASTMAIL_SRC) >> LINT.OUT

$(BIN)/frm:		$(FRM_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(FRM_OBJ) ../lib/libutil.a $(LIB2)

frm_lint:		$(FRM_SRC)
			$(LINT) $(LINTFLAGS) $(FRM_SRC) >> LINT.OUT

$(BIN)/listalias:	$(LISTALIAS_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(LISTALIAS_OBJ) ../lib/libutil.a $(LIB2)

listalias_lint:		$(LISTALIAS_SRC)
			$(LINT) $(LINTFLAGS) $(LISTALIAS_SRC) >> LINT.OUT

$(BIN)/newalias:	$(NEWALIAS_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(NEWALIAS_OBJ) ../lib/libutil.a $(LIB2)

newalias_lint:		$(NEWALIAS_SRC)
			$(LINT) $(LINTFLAGS) $(NEWALIAS_SRC) >> LINT.OUT

$(BIN)/newmail:		$(NEWMAIL_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(NEWMAIL_OBJ) ../lib/libutil.a $(LIB2)

newmail_lint:		$(NEWMAIL_SRC)
			$(LINT) $(LINTFLAGS) $(NEWMAIL_SRC) >> LINT.OUT

$(BIN)/readmsg:		$(READMSG_OBJ) ../lib/libutil.a
			$(CC) $(LFLAGS) -o $@ $(READMSG_OBJ) ../lib/libutil.a $(LIB2)

readmsg_lint:		$(READMSG_SRC)
			$(LINT) $(LINTFLAGS) $(READMSG_SRC) >> LINT.OUT


#	Rules to make shell scripts in bin directory
$(BIN)/checkalias:	checkalias
			$(CP) $? $@
			$(CHMOD) u+w,a+rx $@

$(BIN)/messages:	messages
			$(CP) $? $@
			$(CHMOD) u+w,a+rx $@

$(BIN)/printmail:	printmail
			$(CP) $? $@
			$(CHMOD) u+w,a+rx $@

#	Dependencies of header files upon other header files they include
$(INCLDIR)/defs.h:	$(INCLDIR)/../config.h $(INCLDIR)/sysdefs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

$(INCLDIR)/elmutil.h:	$(INCLDIR)/curses.h $(INCLDIR)/defs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

$(INCLDIR)/headers.h:	$(INCLDIR)/curses.h $(INCLDIR)/defs.h
			$(CHMOD) u+w $@
			$(TOUCH) $@

#	Dependencies of C object files
answer.o:	$(INCLDIR)/ndbz.h $(INCLDIR)/elmutil.h $(INCLDIR)/s_answer.h
arepdaem.o:	$(INCLDIR)/elmutil.h
autoreply.o:	$(INCLDIR)/elmutil.h
fastmail.o:	$(INCLDIR)/patchlevel.h $(INCLDIR)/elmutil.h $(INCLDIR)/s_fastmail.h
from.o:		$(INCLDIR)/elmutil.h $(INCLDIR)/s_from.h
listalias.o:	$(INCLDIR)/elmutil.h $(INCLDIR)/sysdefs.h $(INCLDIR)/s_newalias.h
newalias.o:	$(INCLDIR)/elmutil.h $(INCLDIR)/sysdefs.h $(INCLDIR)/s_newalias.h
newmail.o:	$(INCLDIR)/elmutil.h $(INCLDIR)/s_newmail.h
readmsg.o:	$(INCLDIR)/elmutil.h $(INCLDIR)/s_readmsg.h

#	Dependencies and rules for installing programs from bin directory
$(DEST)/answer:		$(BIN)/answer
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/arepdaemon:	$(BIN)/arepdaemon
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/autoreply:	$(BIN)/autoreply
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@
			$(CHMOD) u+s $@

$(DEST)/checkalias:	$(BIN)/checkalias
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+rx $@

$(DEST)/fastmail:	$(BIN)/fastmail
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/frm:		$(BIN)/frm
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/nfrm:		$(DEST)/frm
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(RM) $@
			$(LN) $? $@

$(DEST)/listalias:	$(BIN)/listalias
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/messages:	$(BIN)/messages
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+rx $@

$(DEST)/newalias:	$(BIN)/newalias
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/newmail:	$(BIN)/newmail
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(RM) $@
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/printmail:	$(BIN)/printmail
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+rx $@

$(DEST)/readmsg:	$(BIN)/readmsg
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(CP) $? $@
			$(CHMOD) a+x $@

$(DEST)/wnewmail:	$(DEST)/newmail
			-$(MV) $@ $@.old
			-$(RM) $@.old
			$(RM) $@
			$(LN) $? $@

