case $CONFIG in
    '') . ./config.sh ;;
esac
echo "Extracting Makefile (with variable substitutions)"
$cat >Makefile <<!GROK!THIS!
# $Id: Makefile.SH,v 1.2 92/01/11 16:26:58 usenet Exp $
#
# This software is Copyright 1991 by Stan Barber. 
#
# Permission is hereby granted to copy, reproduce, redistribute or otherwise
# use this software as long as: there is no monetary profit gained
# specifically from the use or reproduction of this software, it is not
# sold, rented, traded or otherwise marketed, and this copyright notice is
# included prominently in any copy made. 
#
# The author make no claims as to the fitness or correctness of this software
# for any use whatsoever, and it is provided as is. Any use of this software
# is at the user's own risk. 
#
# $Log:	Makefile.SH,v $
# Revision 1.2  92/01/11  16:26:58  usenet
# Lots of bug fixes:
# 
#    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
#    o    Enhanced the article-reading code to not remember our side-trip
#         to the end of the group between thread selections (to fix '-').
#    o    Extended trrn's handling of new articles (we fetch the active file
#         more consistently, and listen to what the GROUP command tells us).
#    o    Enhanced the thread selector to mention when new articles have
#         cropped up since the last visit to the selector.
#    o    Changed strftime to use size_t and added a check for size_t in
#         Configure to make sure it is defined.  Also made it a bit more
#         portable by using gettimeofday() and timezone() in some instances.
#    o    Fixed a problem with the "total" structure not getting zero'ed in
#         mthreads (causing bogus 'E'rrors on null groups).
#    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
#    o    Fixed a problem with using N and Y with the newgroup code and then
#         trying to use the 'a' command.
#    o    Fixed an instance where having trrn get ahead of the active file
#         might declare a group as being reset when it wasn't.
#    o    Reorganized the checks for Apollo's C library to make sure it doesn't
#         find the Domain OS version.
#    o    Added a check for Xenix 386's C library.
#    o    Made the Configure PATH more portable.
#    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
#    o    Fixed the insert-my-subject-before-my-sibling code to not do this
#         when the sibling's subject is the same as the parent.
#    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
#    o    Twiddled the mthreads.8 manpage.
#    o    mthreads.8 is now installed and the destination is prompted for in
#         Configure, since it might be different from the .1 destination.
#    o    Fixed a typo in newsnews.SH and the README.
# 
# Revision 4.4.2.1  1991/12/01  18:05:42  sob
# Changed clientlib to be built in the rn tree instead of the nntp tree.
#
# Revision 4.4.1.1  1991/09/25  19:38:08  sob
# Changed some install directives
#
# Revision 4.4  1991/09/09  20:18:23  sob
# release 4.4
#
#
# 

CC = $cc
rnbin = $rnbin
rnlib = $rnlib
mansrc = $mansrc
manext = $manext
#MTmtmansrc = $mtmansrc
#MTmtmanext = $mtmanext
#NNTPNNTPDIR = $NNTPSRC
#NNTPNNTPINC = $rrninclude
CFLAGS = $cflags
LDFLAGS = $posix$iandd
NDIRC = $ndirc
NDIRO = $ndiro

libs = $ndirlib $termlib $jobslib $xnxlib $socketlib -lm $sharedclib
mlibs = $ndirlib $jobslib $xnxlib $socketlib $sharedclib
!GROK!THIS!
$cat >>Makefile <<'!NO!SUBS!'
#NNTPnntp=getactive
#MTmthreading=mthreads mt.check
public = trn tmpthread newsetup newsgroups Pnews Rnmail
private = norm.saver mbox.saver makedir\
	filexp Pnews.header $(mthreading) $(nntp)
manpages = trn.1 Pnews.1 Rnmail.1 newsetup.1 newsgroups.1
util = Makefile makedepend newsnews


h1 = addng.h art.h artio.h artsrch.h autosub.h backpage.h bits.h cheat.h 
h2 = common.h decode.h final.h head.h help.h init.h intrp.h kfile.h last.h
h3 = ng.h ngdata.h ngsrch.h ngstuff.h only.h rcln.h rcstuff.h
h4 = respond.h rn.h search.h sw.h term.h util.h

h = $(h1) $(h2) $(h3) $(h4)

c1 = addng.c art.c artio.c artsrch.c autosub.c backpage.c bits.c cheat.c
c2 = decode.c final.c head.c help.c init.c intrp.c kfile.c last.c $(NDIRC)
c3 = ng.c ngdata.c ngsrch.c ngstuff.c only.c rcln.c rcstuff.c respond.c rn.c
c4 = rthreads.c rt-rn.c rt-select.c search.c sw.c term.c threads.c util.c
c5 = unship.c uudecode.c
#NNTPc6 = server.c $(NNTPDIR)/common/clientlib.c

c = $(c1) $(c2) $(c3) $(c4) $(c5) $(c6)

mtc1 = mthreads.c mt-read.c mt-process.c mt-write.c mt-misc.c threads.c $(NDIRC)
#NNMTmtc2 = $(c6)
mtc = $(mtc1) $(mtc2)

tmc = tmpthread.c tm-read.c tm-process.c tm-write.c mt-misc.c threads.c $(mtc2)

obj1 = addng.o art.o artio.o artsrch.o autosub.o backpage.o bits.o cheat.o
obj2 = decode.o final.o head.o help.o init.o intrp.o kfile.o last.o $(NDIRO)
obj3 = ng.o ngdata.o ngsrch.o ngstuff.o only.o rcln.o rcstuff.o respond.o rn.o
obj4 = rthreads.o rt-rn.o rt-select.o search.o sw.o term.o threads.o util.o
obj5 = unship.o uudecode.o
#NNTPobj6 = server.o clientlib.o

obj = $(obj1) $(obj2) $(obj3) $(obj4) $(obj5) $(obj6)

mto1 = mthreads.o mt-read.o mt-process.o mt-write.o mt-misc.o
mto2 = threads.o getdate.o $(NDIRO)
#NNMTmto3 = $(obj6)
mtobj = $(mto1) $(mto2) $(mto3)

tmo1 = tmpthread.o tm-read.o tm-process.o tm-write.o mt-misc.o
tmo2 = threads.o getdate.o
tmobj = $(tmo1) $(tmo2) $(obj6)

lintflags = -phbvxac

add1 = Makefile.old Pnews Rnmail mt.check
add2 = bsd config.h config.sh eunice filexp 
add3 = loc makedepend makedir mbox.saver ndir.h newsetup
add4 = newsgroups newsnews norm.saver
add5 = pdp11 usg v7 ultrix sun hp-ux sgi xenix next server.h mips uts
add6 = all pyr grimble .falseactive Pnews.header s5uniq sigtest stardent

addedbyconf = $(add1) $(add2) $(add3) $(add4) $(add5) $(add6)

# grrr
SHELL = /bin/sh


.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: $(public) $(private) $(util)
	touch all

trn: $(obj)
	$(CC) $(LDFLAGS) $(obj) $(libs) -o trn

mthreads: $(mtobj)
	$(CC) $(LDFLAGS) $(mtobj) $(mlibs) -o mthreads

tmpthread: $(tmobj)
	$(CC) $(LDFLAGS) $(tmobj) $(mlibs) -o tmpthread

mtgroups: mtgroups.o mt-misc.o
	$(CC) $(LDFLAGS) mtgroups.o mt-misc.o -lcurses $(libs) -o mtgroups

#NNTPgetactive: getactive.o server.o clientlib.o
#NNTP	$(CC) $(LDFLAGS) getactive.o server.o clientlib.o -o getactive $(libs)

#NNTPclientlib.o:
#NNTP	$(CC) -c $(CFLAGS) $(NNTPINC) $(NNTPDIR)/common/clientlib.c

# if a .h file depends on another .h file...
$(h):
	touch $@

install: $(public) $(private) $(manpages)
# won't work with csh
	export PATH || exit 1
	- mv $(rnbin)/trn $(rnbin)/trn.old
#NNTP	- ln -s trn $(rnbin)/trrn
	- if test `pwd` != $(rnbin); then cp $(public) $(rnbin); fi
	- if test `pwd` != $(rnbin); then cd $(rnbin); chmod 755 $(public); strip trn tmpthread ; fi
	- ./makedir `./filexp $(rnlib)`
#MT	- cd `./filexp $(rnlib)`; mv mthreads mthreads.old
	- if test `pwd` != `./filexp $(rnlib)`; then cp INIT $(private) `./filexp $(rnlib)`; fi
#NNTP	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/getactive ; fi
	- if test ! -f `./filexp $(rnlib)/newsnews`; then cp newsnews `./filexp $(rnlib)`; fi
	cd `./filexp $(rnlib)`; chmod 755 $(private); chmod 644 INIT newsnews
#MT	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/mthreads ; fi
	- if test `pwd` != $(mansrc); then \
for page in $(manpages); do \
cp $$page $(mansrc)/`basename $$page .1`.$(manext); \
done; \
#NNTPecho ".so man$(manext)/trn.$(manext)" > $(mansrc)/trrn.$(manext) ; \
fi
#MT	- if test `pwd` != $(mtmansrc); then \
#MTcp mthreads.8 $(mtmansrc)/mthreads.$(mtmanext); \
#MTfi

install-sysv: $(public) $(private) $(manpages)
# won't work with csh
	export PATH || exit 1
	- mv $(rnbin)/trn $(rnbin)/trn.old
#NNTP	- ln -s trn $(rnbin)/trrn
	- if test `pwd` != $(rnbin); then cp $(public) $(rnbin); fi
	- if test `pwd` != $(rnbin); then cd $(rnbin); chmod 755 $(public); strip trn tmpthread ; fi
	- ./makedir `./filexp $(rnlib)`
#MT	- cd `./filexp $(rnlib)`; mv mthreads mthreads.old
	- if test `pwd` != `./filexp $(rnlib)`; then cp INIT $(private) `./filexp $(rnlib)`; fi
#NNTP	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/getactive ; fi
	- if test ! -f `./filexp $(rnlib)/newsnews`; then cp newsnews `./filexp $(rnlib)`; fi
	cd `./filexp $(rnlib)`; chmod 755 $(private); chmod 644 INIT newsnews
#MT	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/mthreads ; fi
	- if test `pwd` != $(mansrc); then \
for page in $(manpages); do \
nroff -man $$page|compress > $(mansrc)/`basename $$page .1`.$(manext).Z; \
done; \
#NNTPecho ".so man$(manext)/trn.$(manext)" > $(mansrc)/trrn.$(manext) ; \
fi
#MT	- if test `pwd` != $(mtmansrc); then \
#MTnroff -man mthreads.8|compress > $(mtmansrc)/mthreads.$(mtmanext).Z; \
#MTfi

clean:
	rm -f *.o

realclean:
	rm -f trn mthreads tmpthread *.o core $(addedbyconf) 
#NNTP	rm -f clientlib.o getactive

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint: lint_trn lint_mt lint_tm

lint_trn:
	lint $(lintflags) $(defs) $(c) > trn.fuzz

lint_mt:
	lint $(lintflags) $(mtc) > mt.fuzz

lint_tm:
	lint $(lintflags) $(tmc) > tt.fuzz

sabertrn: $(c)
	#load $(c) $(libs)

depend: config.h Makefile
	./makedepend

# AUTOMATICALLY GENERATED MAKE DEPENDENCIES--PUT NOTHING BELOW THIS LINE
config.h: config.h.SH config.sh ; sh config.h.SH
Makefile: Makefile.SH config.sh ; sh Makefile.SH
$(obj):
	@ echo "You haven't done a "'"make depend" yet!'; exit 1
!NO!SUBS!
case "$isrrn" in
define) if $test "$serverspool " != " " ; then
		sed < Makefile -e '/^#NNTP/s/^#NNTP//
				   /^#MT/s/^#MT//
				   /^#NNMT/d' > Makefile.new
	else if $test "$xthread " != "define " ; then
		sed < Makefile -e '/^#NNTP/s/^#NNTP//
				   /^#MT/s/^#MT//
				   /^#NNMT/s/^#NNMT//' > Makefile.new
	else
		sed < Makefile -e '/^#NNTP/s/^#NNTP//
				   /^#MT/d
				   /^#NNMT/s/^#NNMT//' > Makefile.new
	fi ; fi
	;;
*)	sed < Makefile -e '/^#NNTP/d
			   /^#MT/s/^#MT//
			   /^#NNMT/d' > Makefile.new
	;;
esac
mv Makefile.new Makefile
$eunicefix Makefile
