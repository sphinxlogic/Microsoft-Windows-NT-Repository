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

case "$d_symlink" in
*define*) sln='ln -s' ;;
*) sln='ln';;
esac

echo "Extracting Makefile (with variable substitutions)"
cat >Makefile <<!GROK!THIS!
#
# $Id: Makefile.SH,v 5.1 1992/10/03 22:12:17 syd Exp $
#
#  Makefile for the entire ELM mail system
#
#         (C) Copyright 1986,1987, by Dave Taylor
#         (C) Copyright 1988-1992, USENET Community Trust
#
#  $Log: Makefile.SH,v $
# Revision 5.1  1992/10/03  22:12:17  syd
# Initial checkin as of 2.4 Release at PL0
#
# 
#

#	Variables established by Configure
CAT		=	$cat
GREP		=	$grep
ECHO		=	$echo
MAKE		=	$make
PR		=	$pr
SHAR		=	$shar -s 60000
SLEEP		=	$sleep
TAR		=	$tar
TAPE		=	$rmttape

!GROK!THIS!
cat >>Makefile <<'!NO!SUBS!'
#	Other variables
SHELL			=	/bin/sh
SOURCE_FILES		=	`$(GREP) -v 'doc/' MANIFEST`
DISTRIBUTED_FILES	=	`$(CAT) MANIFEST`


# Targets
#	Give default target first and alone
default_target:	all

#	Targets that are simply executed in each subordinate makefile as is
all install uninstall lint clean:
		cd lib; $(MAKE) -$(MAKEFLAGS) $@
		cd src; $(MAKE) -$(MAKEFLAGS) $@
		cd utils; $(MAKE) -$(MAKEFLAGS) $@
		cd filter; $(MAKE) -$(MAKEFLAGS) $@
		cd doc; $(MAKE) -$(MAKEFLAGS) $@

#	Targets that apply to all elm at once
listing:
		@$(ECHO) "Making source listing in file LISTING."
		@$(PR) -n -f $(SOURCE_FILES) > LISTING

#	This is the only target that gets installed even if not out-of-date
#	with respect the files from which it is installed.
rmt-install:	rmt-defined
		@($(ECHO) "Warning: This assumes you have already installed";\
		  $(ECHO) "ELM on the remote machine <$(REMOTE)>. If this is";\
		  $(ECHO) "not the case, interrupt this process quickly!";\
		  $(SLEEP) 10;)
		cd lib; $(MAKE) -$(MAKEFLAGS) REMOTE=$(REMOTE) $@
		cd src; $(MAKE) -$(MAKEFLAGS) REMOTE=$(REMOTE) $@
		cd filter; $(MAKE) -$(MAKEFLAGS) REMOTE=$(REMOTE) $@
		cd utils; $(MAKE) -$(MAKEFLAGS) REMOTE=$(REMOTE) $@
		cd doc; $(MAKE) -$(MAKEFLAGS) REMOTE=$(REMOTE) $@

rmt-defined:
	@(if [ "$(REMOTE)" = "" ];\
	  then\
	    $(ECHO) "You need to define 'REMOTE' as the remote file system";\
	    $(ECHO) "for this particular command. The easiest way to do this";\
	    $(ECHO) "to type:";\
	    $(ECHO) "        make REMOTE=<remote file system> rmt-install";\
	    exit 1;\
	fi);

shar:
		$(SHAR) $(DISTRIBUTED_FILES)

tar:
		$(TAR) cvf $(TAPE) $(DISTRIBUTED_FILES)

#	Targets that are really in subordinate make files
documentation:
	cd doc; $(MAKE) -$(MAKEFLAGS) $@

elm:
	cd lib; $(MAKE) -$(MAKEFLAGS) all
	cd src; $(MAKE) -$(MAKEFLAGS) all

#	The dummy dependency here prevents make from thinking the target is the
#	filter directory that sits in the current directory, rather than
#	an abstract target.
filter: _filter

_filter:
	cd filter; $(MAKE) -$(MAKEFLAGS) all

#	The dummy dependency here prevents make from thinking the target is the
#	utils directory that sits in the current directory, rather than
#	an abstract target.
utils: _utils

_utils:
	cd utils; $(MAKE) -$(MAKEFLAGS) all

!NO!SUBS!
