case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
    fi
    . ./config.sh
    ;;
esac
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac

case "$d_usenls" in
define)	mcsrc="mcprt.c mcprtlib.c msgcat.c"
	mcobj="mcprt.o mcprtlib.o msgcat.o"
	;;
*)	mcsrc= mcobj= ;;
esac

case "$d_strstr" in
define)	strsrc= strobj= ;;
*)	strsrc="strstr.c"
	strobj="strstr.o"
	;;
esac

echo "Extracting lib/Makefile (with variable substitutions)"
cat >Makefile <<!GROK!THIS!
# $Id: Makefile.SH,v 5.3 1992/11/07 20:07:23 syd Exp $
#
#  Makefile for the ELM mail program.
#
#	Copyright (c) 1986,1987 Dave Taylor
#	Copyright (c) 1988-1992 USENET Community Trust
#
# Bug reports, patches, comments, suggestions should be sent to:
#
#	Syd Weinstein - elm@DSI.COM
#			dsinc!elm
#
# $Log: Makefile.SH,v $
# Revision 5.3  1992/11/07  20:07:23  syd
# Add header_cmp routine
# From: Syd
#
# Revision 5.2  1992/10/27  01:43:40  syd
# Move posix_signal to lib directory
# From: tom@osf.org
#
# Revision 5.1  1992/10/03  22:41:36  syd
# Initial checkin as of 2.4 Release at PL0
#
#
#
# Variables
#	Variables established by Configure
AR		=	ar
CC		=	$cc
CCFLAGS		=	$ccflags $xencf
CHMOD		=	$chmod
LINT		=	$lint
OPTIMIZE	=	$optimize
RANLIB		=	$ranlib
RM		= 	$rm -f
TOUCH		=	$touch
TAGS		=	ctags

MCSRC		=	$mcsrc
MCOBJ		=	$mcobj

STRSRC		=	$strsrc
STROBJ		=	$strobj

!GROK!THIS!

cat >>Makefile <<'!NO!SUBS!'

#	Variables you may want to manually edit
#		If you want debug logging then you'll
#		want to uncomment the following.
#DEBUG		=	-DDEBUG

#		If you're on an ACSnet system (Australia) then
#		you'll want to uncomment the following.
# DACSNET	=	-DACSNET

#	Other general variables
CFLAGS		=	$(CCFLAGS) $(OPTIMIZE) -I$(INCLDIR) $(DEBUG) $(DACSNET) 
INCLDIR		=	../hdrs
LINTFLAGS	=	-I$(INCLDIR)
SHELL		=	/bin/sh

# 	Lists
LIB_SRC		=	add_site.c	\
			addrmchusr.c	\
			mk_aliases.c	\
			mk_lockname.c	\
			can_access.c	\
			can_open.c	\
			chloc.c		\
			errno.c		\
			expand.c	\
			figadrssee.c	\
			gcos_name.c	\
			getaddrfrm.c	\
			getarpdate.c	\
			getfullnam.c	\
			header_cmp.c	\
			in_list.c	\
			in_string.c	\
			istrcmp.c	\
			ldstate.c	\
			len_next.c	\
			mail_gets.c	\
			$(MCSRC)	\
			move_left.c	\
			ndbz.c		\
			okay_addr.c	\
			opt_utils.c	\
			parsarpwho.c	\
			posixsig.c	\
			putenv.c	\
			qstrings.c	\
			remfirstwd.c	\
			reverse.c	\
			shiftlower.c	\
			strincmp.c	\
			striparens.c	\
			$(STRSRC)	\
			tail_of.c	\
			validname.c

LIB_OBJ		=	add_site.o	\
			addrmchusr.o	\
			mk_aliases.o	\
			mk_lockname.o	\
			can_access.o	\
			can_open.o	\
			chloc.o		\
			errno.o		\
			expand.o	\
			figadrssee.o	\
			gcos_name.o	\
			getaddrfrm.o	\
			getarpdate.o	\
			getfullnam.o	\
			header_cmp.o	\
			in_list.o	\
			in_string.o	\
			istrcmp.o	\
			ldstate.o	\
			len_next.o	\
			mail_gets.o	\
			$(MCOBJ)	\
			move_left.o	\
			ndbz.o		\
			okay_addr.o	\
			opt_utils.o	\
			parsarpwho.o	\
			posixsig.o	\
			putenv.o	\
			qstrings.o	\
			remfirstwd.o	\
			reverse.o	\
			shiftlower.o	\
			strincmp.o	\
			striparens.o	\
			$(STROBJ)	\
			tail_of.o	\
			validname.o

all:			libutil.a

install:		libutil.a

uninstall:		libutil.a

libutil.a:		$(LIB_OBJ)
			$(AR) r $@ $?
			$(RANLIB) $@

lint:		
		$(LINT) $(LINTFLAGS) $(LIB_SRC) > LINT.OUT

tags:
		$(TAGS) $(LIB_SRC)

clean:		
		$(RM) $(LIB_OBJ) libutil.a

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

#	Dependencies of C object files
add_site.o:	$(INCLDIR)/headers.h
addrmchusr.o:	$(INCLDIR)/headers.h
mk_aliases.o:	$(INCLDIR)/headers.h $(INCLDIR)/ndbz.h $(INCLDIR)/s_newalias.h
mk_lockname.o:	$(INCLDIR)/headers.h
can_access.o:	$(INCLDIR)/headers.h
can_open.o:	$(INCLDIR)/headers.h
chloc.o:	$(INCLDIR)/headers.h
errno.o:	$(INCLDIR)/headers.h
expand.o:	$(INCLDIR)/defs.h $(INCLDIR)/s_elmrc.h
figadrssee.o:	$(INCLDIR)/headers.h
gcos_name.o:	$(INCLDIR)/headers.h
getaddrfrm.o:	$(INCLDIR)/headers.h
getarpdate.o:	$(INCLDIR)/headers.h
getfullnam.o:	$(INCLDIR)/headers.h
header_cmp.o:	$(INCLDIR)/headers.h
in_list.o:	$(INCLDIR)/headers.h
in_string.o:	$(INCLDIR)/headers.h
istrcmp.o:	$(INCLDIR)/headers.h
ldstate.o:	$(INCLDIR)/defs.h
mcprt.o:	$(INCLDIR)/mcprt.h $(INCLDIR)/mcprtlib.h
mcprtlib.o:	$(INCLDIR)/mcprtlib.h $(INCLDIR)/nl_types.h
move_left.o:	$(INCLDIR)/headers.h
msgcat.o:	$(INCLDIR)/msgcat.h $(INCLDIR)/nl_types.h
ndbz.o:		$(INCLDIR)/headers.h $(INCLDIR)/ndbz.h
okay_addr.o:	$(INCLDIR)/headers.h
opt_utils.o:	$(INCLDIR)/headers.h $(INCLDIR)/s_error.h
parsarpwho.o:	$(INCLDIR)/headers.h
posixsig.o:	$(INCLDIR)/headers.h
putenv.o:	$(INCLDIR)/headers.h
remfirstwd.o:	$(INCLDIR)/headers.h
reverse.o:	$(INCLDIR)/headers.h
shiftlower.o:	$(INCLDIR)/headers.h
strincmp.o:	$(INCLDIR)/headers.h
striparens.o:	$(INCLDIR)/headers.h
strstr.o:	$(INCLDIR)/headers.h
tail_of.o:	$(INCLDIR)/headers.h
validname.o:	$(INCLDIR)/defs.h

!NO!SUBS!
