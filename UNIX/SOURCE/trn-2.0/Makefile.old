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

CC = gcc
rnbin = /opt/bin
rnlib = /opt/lib/news
mansrc = /opt/man/cat1
manext = 1
mtmansrc = /opt/man/cat8
mtmanext = 8
CFLAGS = -O3 -g
LDFLAGS = 
NDIRC = 
NDIRO = 

libs =  -ltermlib    -lm 
mlibs =     
mthreading=mthreads mt.check
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

c = $(c1) $(c2) $(c3) $(c4) $(c5) $(c6)

mtc1 = mthreads.c mt-read.c mt-process.c mt-write.c mt-misc.c threads.c $(NDIRC)
mtc = $(mtc1) $(mtc2)

tmc = tmpthread.c tm-read.c tm-process.c tm-write.c mt-misc.c threads.c $(mtc2)

obj1 = addng.o art.o artio.o artsrch.o autosub.o backpage.o bits.o cheat.o
obj2 = decode.o final.o head.o help.o init.o intrp.o kfile.o last.o $(NDIRO)
obj3 = ng.o ngdata.o ngsrch.o ngstuff.o only.o rcln.o rcstuff.o respond.o rn.o
obj4 = rthreads.o rt-rn.o rt-select.o search.o sw.o term.o threads.o util.o
obj5 = unship.o uudecode.o

obj = $(obj1) $(obj2) $(obj3) $(obj4) $(obj5) $(obj6)

mto1 = mthreads.o mt-read.o mt-process.o mt-write.o mt-misc.o
mto2 = threads.o getdate.o $(NDIRO)
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



# if a .h file depends on another .h file...
$(h):
	touch $@

install: $(public) $(private) $(manpages)
# won't work with csh
	export PATH || exit 1
	- mv $(rnbin)/trn $(rnbin)/trn.old
	- if test `pwd` != $(rnbin); then cp $(public) $(rnbin); fi
	- if test `pwd` != $(rnbin); then cd $(rnbin); chmod 755 $(public); strip trn tmpthread ; fi
	- ./makedir `./filexp $(rnlib)`
	- cd `./filexp $(rnlib)`; mv mthreads mthreads.old
	- if test `pwd` != `./filexp $(rnlib)`; then cp INIT $(private) `./filexp $(rnlib)`; fi
	- if test ! -f `./filexp $(rnlib)/newsnews`; then cp newsnews `./filexp $(rnlib)`; fi
	cd `./filexp $(rnlib)`; chmod 755 $(private); chmod 644 INIT newsnews
	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/mthreads ; fi
	- if test `pwd` != $(mansrc); then \
for page in $(manpages); do \
cp $$page $(mansrc)/`basename $$page .1`.$(manext); \
done; \
fi
	- if test `pwd` != $(mtmansrc); then \
cp mthreads.8 $(mtmansrc)/mthreads.$(mtmanext); \
fi

install-sysv: $(public) $(private) $(manpages)
# won't work with csh
	export PATH || exit 1
	- mv $(rnbin)/trn $(rnbin)/trn.old
	- if test `pwd` != $(rnbin); then cp $(public) $(rnbin); fi
	- if test `pwd` != $(rnbin); then cd $(rnbin); chmod 755 $(public); strip trn tmpthread ; fi
	- ./makedir `./filexp $(rnlib)`
	- cd `./filexp $(rnlib)`; mv mthreads mthreads.old
	- if test `pwd` != `./filexp $(rnlib)`; then cp INIT $(private) `./filexp $(rnlib)`; fi
	- if test ! -f `./filexp $(rnlib)/newsnews`; then cp newsnews `./filexp $(rnlib)`; fi
	cd `./filexp $(rnlib)`; chmod 755 $(private); chmod 644 INIT newsnews
	- if test `pwd` != `./filexp $(rnlib)`; then strip `./filexp $(rnlib)`/mthreads ; fi
	- if test `pwd` != $(mansrc); then \
for page in $(manpages); do \
nroff -man $$page > $(mansrc)/$$page; \
done; \
fi
	- if test `pwd` != $(mtmansrc); then \
nroff -man mthreads.8 > $(mtmansrc)/mthreads.8; \
fi

clean:
	rm -f *.o

realclean:
	rm -f trn mthreads tmpthread *.o core $(addedbyconf) 

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
# If this runs make out of memory, delete /usr/include lines.
addng.o: /usr/include/ctype.h
addng.o: /usr/include/errno.h
addng.o: /usr/include/fcntl.h
addng.o: /usr/include/signal.h
addng.o: /usr/include/stdio.h
addng.o: /usr/include/sys/cred.h
addng.o: /usr/include/sys/errno.h
addng.o: /usr/include/sys/fcntl.h
addng.o: /usr/include/sys/ioctl.h
addng.o: /usr/include/sys/poll.h
addng.o: /usr/include/sys/ptem.h
addng.o: /usr/include/sys/select.h
addng.o: /usr/include/sys/signal.h
addng.o: /usr/include/sys/stat.h
addng.o: /usr/include/sys/stream.h
addng.o: /usr/include/sys/strmdep.h
addng.o: /usr/include/sys/termio.h
addng.o: /usr/include/sys/termios.h
addng.o: /usr/include/sys/time.h
addng.o: /usr/include/sys/ttydev.h
addng.o: /usr/include/sys/types.h
addng.o: /usr/include/sys/vnode.h
addng.o: /usr/include/termio.h
addng.o: /usr/include/time.h
addng.o: EXTERN.h
addng.o: INTERN.h
addng.o: addng.c
addng.o: addng.h
addng.o: common.h
addng.o: config.h
addng.o: final.h
addng.o: intrp.h
addng.o: last.h
addng.o: ngdata.h
addng.o: only.h
addng.o: rcstuff.h
addng.o: rn.h
addng.o: search.h
addng.o: util.h
art.o: /usr/include/ctype.h
art.o: /usr/include/errno.h
art.o: /usr/include/fcntl.h
art.o: /usr/include/signal.h
art.o: /usr/include/stdio.h
art.o: /usr/include/sys/cred.h
art.o: /usr/include/sys/errno.h
art.o: /usr/include/sys/fcntl.h
art.o: /usr/include/sys/ioctl.h
art.o: /usr/include/sys/poll.h
art.o: /usr/include/sys/ptem.h
art.o: /usr/include/sys/select.h
art.o: /usr/include/sys/signal.h
art.o: /usr/include/sys/stat.h
art.o: /usr/include/sys/stream.h
art.o: /usr/include/sys/strmdep.h
art.o: /usr/include/sys/termio.h
art.o: /usr/include/sys/termios.h
art.o: /usr/include/sys/time.h
art.o: /usr/include/sys/ttydev.h
art.o: /usr/include/sys/types.h
art.o: /usr/include/sys/vnode.h
art.o: /usr/include/termio.h
art.o: /usr/include/time.h
art.o: EXTERN.h
art.o: INTERN.h
art.o: art.c
art.o: art.h
art.o: artio.h
art.o: artstate.h
art.o: backpage.h
art.o: bits.h
art.o: cheat.h
art.o: common.h
art.o: config.h
art.o: final.h
art.o: head.h
art.o: help.h
art.o: intrp.h
art.o: ng.h
art.o: ngdata.h
art.o: ngstuff.h
art.o: rcstuff.h
art.o: rn.h
art.o: rthreads.h
art.o: search.h
art.o: sw.h
art.o: term.h
art.o: threads.h
art.o: util.h
artio.o: /usr/include/ctype.h
artio.o: /usr/include/errno.h
artio.o: /usr/include/fcntl.h
artio.o: /usr/include/signal.h
artio.o: /usr/include/stdio.h
artio.o: /usr/include/sys/cred.h
artio.o: /usr/include/sys/errno.h
artio.o: /usr/include/sys/fcntl.h
artio.o: /usr/include/sys/ioctl.h
artio.o: /usr/include/sys/poll.h
artio.o: /usr/include/sys/ptem.h
artio.o: /usr/include/sys/select.h
artio.o: /usr/include/sys/signal.h
artio.o: /usr/include/sys/stat.h
artio.o: /usr/include/sys/stream.h
artio.o: /usr/include/sys/strmdep.h
artio.o: /usr/include/sys/termio.h
artio.o: /usr/include/sys/termios.h
artio.o: /usr/include/sys/time.h
artio.o: /usr/include/sys/ttydev.h
artio.o: /usr/include/sys/types.h
artio.o: /usr/include/sys/vnode.h
artio.o: /usr/include/termio.h
artio.o: /usr/include/time.h
artio.o: EXTERN.h
artio.o: INTERN.h
artio.o: artio.c
artio.o: artio.h
artio.o: common.h
artio.o: config.h
artsrch.o: /usr/include/ctype.h
artsrch.o: /usr/include/errno.h
artsrch.o: /usr/include/fcntl.h
artsrch.o: /usr/include/signal.h
artsrch.o: /usr/include/stdio.h
artsrch.o: /usr/include/sys/cred.h
artsrch.o: /usr/include/sys/errno.h
artsrch.o: /usr/include/sys/fcntl.h
artsrch.o: /usr/include/sys/ioctl.h
artsrch.o: /usr/include/sys/poll.h
artsrch.o: /usr/include/sys/ptem.h
artsrch.o: /usr/include/sys/select.h
artsrch.o: /usr/include/sys/signal.h
artsrch.o: /usr/include/sys/stat.h
artsrch.o: /usr/include/sys/stream.h
artsrch.o: /usr/include/sys/strmdep.h
artsrch.o: /usr/include/sys/termio.h
artsrch.o: /usr/include/sys/termios.h
artsrch.o: /usr/include/sys/time.h
artsrch.o: /usr/include/sys/ttydev.h
artsrch.o: /usr/include/sys/types.h
artsrch.o: /usr/include/sys/vnode.h
artsrch.o: /usr/include/termio.h
artsrch.o: /usr/include/time.h
artsrch.o: EXTERN.h
artsrch.o: INTERN.h
artsrch.o: artio.h
artsrch.o: artsrch.c
artsrch.o: artsrch.h
artsrch.o: bits.h
artsrch.o: cheat.h
artsrch.o: common.h
artsrch.o: config.h
artsrch.o: final.h
artsrch.o: head.h
artsrch.o: intrp.h
artsrch.o: kfile.h
artsrch.o: ng.h
artsrch.o: ngdata.h
artsrch.o: rthreads.h
artsrch.o: search.h
artsrch.o: term.h
artsrch.o: threads.h
artsrch.o: util.h
autosub.o: /usr/include/ctype.h
autosub.o: /usr/include/errno.h
autosub.o: /usr/include/fcntl.h
autosub.o: /usr/include/signal.h
autosub.o: /usr/include/stdio.h
autosub.o: /usr/include/sys/cred.h
autosub.o: /usr/include/sys/errno.h
autosub.o: /usr/include/sys/fcntl.h
autosub.o: /usr/include/sys/ioctl.h
autosub.o: /usr/include/sys/poll.h
autosub.o: /usr/include/sys/ptem.h
autosub.o: /usr/include/sys/select.h
autosub.o: /usr/include/sys/signal.h
autosub.o: /usr/include/sys/stat.h
autosub.o: /usr/include/sys/stream.h
autosub.o: /usr/include/sys/strmdep.h
autosub.o: /usr/include/sys/termio.h
autosub.o: /usr/include/sys/termios.h
autosub.o: /usr/include/sys/time.h
autosub.o: /usr/include/sys/ttydev.h
autosub.o: /usr/include/sys/types.h
autosub.o: /usr/include/sys/vnode.h
autosub.o: /usr/include/termio.h
autosub.o: /usr/include/time.h
autosub.o: EXTERN.h
autosub.o: INTERN.h
autosub.o: autosub.c
autosub.o: autosub.h
autosub.o: common.h
autosub.o: config.h
autosub.o: ngsrch.h
autosub.o: search.h
autosub.o: util.h
backpage.o: /usr/include/ctype.h
backpage.o: /usr/include/errno.h
backpage.o: /usr/include/fcntl.h
backpage.o: /usr/include/signal.h
backpage.o: /usr/include/stdio.h
backpage.o: /usr/include/sys/cred.h
backpage.o: /usr/include/sys/errno.h
backpage.o: /usr/include/sys/fcntl.h
backpage.o: /usr/include/sys/ioctl.h
backpage.o: /usr/include/sys/poll.h
backpage.o: /usr/include/sys/ptem.h
backpage.o: /usr/include/sys/select.h
backpage.o: /usr/include/sys/signal.h
backpage.o: /usr/include/sys/stat.h
backpage.o: /usr/include/sys/stream.h
backpage.o: /usr/include/sys/strmdep.h
backpage.o: /usr/include/sys/termio.h
backpage.o: /usr/include/sys/termios.h
backpage.o: /usr/include/sys/time.h
backpage.o: /usr/include/sys/ttydev.h
backpage.o: /usr/include/sys/types.h
backpage.o: /usr/include/sys/vnode.h
backpage.o: /usr/include/termio.h
backpage.o: /usr/include/time.h
backpage.o: EXTERN.h
backpage.o: INTERN.h
backpage.o: backpage.c
backpage.o: backpage.h
backpage.o: common.h
backpage.o: config.h
backpage.o: final.h
backpage.o: intrp.h
bits.o: /usr/include/ctype.h
bits.o: /usr/include/errno.h
bits.o: /usr/include/fcntl.h
bits.o: /usr/include/signal.h
bits.o: /usr/include/stdio.h
bits.o: /usr/include/sys/cred.h
bits.o: /usr/include/sys/errno.h
bits.o: /usr/include/sys/fcntl.h
bits.o: /usr/include/sys/ioctl.h
bits.o: /usr/include/sys/poll.h
bits.o: /usr/include/sys/ptem.h
bits.o: /usr/include/sys/select.h
bits.o: /usr/include/sys/signal.h
bits.o: /usr/include/sys/stat.h
bits.o: /usr/include/sys/stream.h
bits.o: /usr/include/sys/strmdep.h
bits.o: /usr/include/sys/termio.h
bits.o: /usr/include/sys/termios.h
bits.o: /usr/include/sys/time.h
bits.o: /usr/include/sys/ttydev.h
bits.o: /usr/include/sys/types.h
bits.o: /usr/include/sys/vnode.h
bits.o: /usr/include/termio.h
bits.o: /usr/include/time.h
bits.o: EXTERN.h
bits.o: INTERN.h
bits.o: artio.h
bits.o: bits.c
bits.o: bits.h
bits.o: cheat.h
bits.o: common.h
bits.o: config.h
bits.o: final.h
bits.o: head.h
bits.o: intrp.h
bits.o: kfile.h
bits.o: ng.h
bits.o: ngdata.h
bits.o: rcln.h
bits.o: rcstuff.h
bits.o: rn.h
bits.o: rthreads.h
bits.o: threads.h
bits.o: util.h
cheat.o: /usr/include/ctype.h
cheat.o: /usr/include/errno.h
cheat.o: /usr/include/fcntl.h
cheat.o: /usr/include/signal.h
cheat.o: /usr/include/stdio.h
cheat.o: /usr/include/sys/cred.h
cheat.o: /usr/include/sys/errno.h
cheat.o: /usr/include/sys/fcntl.h
cheat.o: /usr/include/sys/ioctl.h
cheat.o: /usr/include/sys/poll.h
cheat.o: /usr/include/sys/ptem.h
cheat.o: /usr/include/sys/select.h
cheat.o: /usr/include/sys/signal.h
cheat.o: /usr/include/sys/stat.h
cheat.o: /usr/include/sys/stream.h
cheat.o: /usr/include/sys/strmdep.h
cheat.o: /usr/include/sys/termio.h
cheat.o: /usr/include/sys/termios.h
cheat.o: /usr/include/sys/time.h
cheat.o: /usr/include/sys/ttydev.h
cheat.o: /usr/include/sys/types.h
cheat.o: /usr/include/sys/vnode.h
cheat.o: /usr/include/termio.h
cheat.o: /usr/include/time.h
cheat.o: EXTERN.h
cheat.o: INTERN.h
cheat.o: artio.h
cheat.o: artsrch.h
cheat.o: bits.h
cheat.o: cheat.c
cheat.o: cheat.h
cheat.o: common.h
cheat.o: config.h
cheat.o: head.h
cheat.o: intrp.h
cheat.o: ng.h
cheat.o: search.h
cheat.o: term.h
decode.o: /usr/include/ctype.h
decode.o: /usr/include/errno.h
decode.o: /usr/include/fcntl.h
decode.o: /usr/include/signal.h
decode.o: /usr/include/stdio.h
decode.o: /usr/include/sys/cred.h
decode.o: /usr/include/sys/errno.h
decode.o: /usr/include/sys/fcntl.h
decode.o: /usr/include/sys/ioctl.h
decode.o: /usr/include/sys/poll.h
decode.o: /usr/include/sys/ptem.h
decode.o: /usr/include/sys/select.h
decode.o: /usr/include/sys/signal.h
decode.o: /usr/include/sys/stat.h
decode.o: /usr/include/sys/stream.h
decode.o: /usr/include/sys/strmdep.h
decode.o: /usr/include/sys/termio.h
decode.o: /usr/include/sys/termios.h
decode.o: /usr/include/sys/time.h
decode.o: /usr/include/sys/ttydev.h
decode.o: /usr/include/sys/types.h
decode.o: /usr/include/sys/vnode.h
decode.o: /usr/include/termio.h
decode.o: /usr/include/time.h
decode.o: EXTERN.h
decode.o: INTERN.h
decode.o: common.h
decode.o: config.h
decode.o: decode.c
decode.o: decode.h
final.o: /usr/include/ctype.h
final.o: /usr/include/errno.h
final.o: /usr/include/fcntl.h
final.o: /usr/include/signal.h
final.o: /usr/include/stdio.h
final.o: /usr/include/sys/cred.h
final.o: /usr/include/sys/errno.h
final.o: /usr/include/sys/fcntl.h
final.o: /usr/include/sys/ioctl.h
final.o: /usr/include/sys/poll.h
final.o: /usr/include/sys/ptem.h
final.o: /usr/include/sys/select.h
final.o: /usr/include/sys/signal.h
final.o: /usr/include/sys/stat.h
final.o: /usr/include/sys/stream.h
final.o: /usr/include/sys/strmdep.h
final.o: /usr/include/sys/termio.h
final.o: /usr/include/sys/termios.h
final.o: /usr/include/sys/time.h
final.o: /usr/include/sys/ttydev.h
final.o: /usr/include/sys/types.h
final.o: /usr/include/sys/vnode.h
final.o: /usr/include/termio.h
final.o: /usr/include/time.h
final.o: EXTERN.h
final.o: INTERN.h
final.o: artio.h
final.o: bits.h
final.o: common.h
final.o: config.h
final.o: final.c
final.o: final.h
final.o: init.h
final.o: last.h
final.o: ng.h
final.o: ngdata.h
final.o: rcstuff.h
final.o: term.h
final.o: util.h
getactive.o: /usr/include/signal.h
getactive.o: /usr/include/stdio.h
getactive.o: /usr/include/sys/signal.h
getactive.o: INTERN.h
getactive.o: config.h
getactive.o: getactive.c
head.o: /usr/include/ctype.h
head.o: /usr/include/errno.h
head.o: /usr/include/fcntl.h
head.o: /usr/include/signal.h
head.o: /usr/include/stdio.h
head.o: /usr/include/sys/cred.h
head.o: /usr/include/sys/errno.h
head.o: /usr/include/sys/fcntl.h
head.o: /usr/include/sys/ioctl.h
head.o: /usr/include/sys/poll.h
head.o: /usr/include/sys/ptem.h
head.o: /usr/include/sys/select.h
head.o: /usr/include/sys/signal.h
head.o: /usr/include/sys/stat.h
head.o: /usr/include/sys/stream.h
head.o: /usr/include/sys/strmdep.h
head.o: /usr/include/sys/termio.h
head.o: /usr/include/sys/termios.h
head.o: /usr/include/sys/time.h
head.o: /usr/include/sys/ttydev.h
head.o: /usr/include/sys/types.h
head.o: /usr/include/sys/vnode.h
head.o: /usr/include/termio.h
head.o: /usr/include/time.h
head.o: EXTERN.h
head.o: INTERN.h
head.o: artio.h
head.o: bits.h
head.o: common.h
head.o: config.h
head.o: head.c
head.o: head.h
head.o: util.h
help.o: /usr/include/ctype.h
help.o: /usr/include/errno.h
help.o: /usr/include/fcntl.h
help.o: /usr/include/signal.h
help.o: /usr/include/stdio.h
help.o: /usr/include/sys/cred.h
help.o: /usr/include/sys/errno.h
help.o: /usr/include/sys/fcntl.h
help.o: /usr/include/sys/ioctl.h
help.o: /usr/include/sys/poll.h
help.o: /usr/include/sys/ptem.h
help.o: /usr/include/sys/select.h
help.o: /usr/include/sys/signal.h
help.o: /usr/include/sys/stat.h
help.o: /usr/include/sys/stream.h
help.o: /usr/include/sys/strmdep.h
help.o: /usr/include/sys/termio.h
help.o: /usr/include/sys/termios.h
help.o: /usr/include/sys/time.h
help.o: /usr/include/sys/ttydev.h
help.o: /usr/include/sys/types.h
help.o: /usr/include/sys/vnode.h
help.o: /usr/include/termio.h
help.o: /usr/include/time.h
help.o: EXTERN.h
help.o: INTERN.h
help.o: common.h
help.o: config.h
help.o: help.c
help.o: help.h
help.o: rn.h
help.o: term.h
init.o: /usr/include/ctype.h
init.o: /usr/include/errno.h
init.o: /usr/include/fcntl.h
init.o: /usr/include/signal.h
init.o: /usr/include/stdio.h
init.o: /usr/include/sys/cred.h
init.o: /usr/include/sys/errno.h
init.o: /usr/include/sys/fcntl.h
init.o: /usr/include/sys/ioctl.h
init.o: /usr/include/sys/poll.h
init.o: /usr/include/sys/ptem.h
init.o: /usr/include/sys/select.h
init.o: /usr/include/sys/signal.h
init.o: /usr/include/sys/stat.h
init.o: /usr/include/sys/stream.h
init.o: /usr/include/sys/strmdep.h
init.o: /usr/include/sys/termio.h
init.o: /usr/include/sys/termios.h
init.o: /usr/include/sys/time.h
init.o: /usr/include/sys/ttydev.h
init.o: /usr/include/sys/types.h
init.o: /usr/include/sys/vnode.h
init.o: /usr/include/termio.h
init.o: /usr/include/time.h
init.o: EXTERN.h
init.o: INTERN.h
init.o: addng.h
init.o: art.h
init.o: artio.h
init.o: artsrch.h
init.o: backpage.h
init.o: bits.h
init.o: cheat.h
init.o: common.h
init.o: config.h
init.o: decode.h
init.o: final.h
init.o: head.h
init.o: help.h
init.o: init.c
init.o: init.h
init.o: intrp.h
init.o: kfile.h
init.o: last.h
init.o: ng.h
init.o: ngdata.h
init.o: ngsrch.h
init.o: ngstuff.h
init.o: only.h
init.o: rcln.h
init.o: rcstuff.h
init.o: respond.h
init.o: rn.h
init.o: rthreads.h
init.o: search.h
init.o: sw.h
init.o: term.h
init.o: threads.h
init.o: util.h
intrp.o: /usr/include/ctype.h
intrp.o: /usr/include/errno.h
intrp.o: /usr/include/fcntl.h
intrp.o: /usr/include/signal.h
intrp.o: /usr/include/stdio.h
intrp.o: /usr/include/sys/cred.h
intrp.o: /usr/include/sys/errno.h
intrp.o: /usr/include/sys/fcntl.h
intrp.o: /usr/include/sys/ioctl.h
intrp.o: /usr/include/sys/poll.h
intrp.o: /usr/include/sys/ptem.h
intrp.o: /usr/include/sys/select.h
intrp.o: /usr/include/sys/signal.h
intrp.o: /usr/include/sys/stat.h
intrp.o: /usr/include/sys/stream.h
intrp.o: /usr/include/sys/strmdep.h
intrp.o: /usr/include/sys/termio.h
intrp.o: /usr/include/sys/termios.h
intrp.o: /usr/include/sys/time.h
intrp.o: /usr/include/sys/ttydev.h
intrp.o: /usr/include/sys/types.h
intrp.o: /usr/include/sys/vnode.h
intrp.o: /usr/include/termio.h
intrp.o: /usr/include/time.h
intrp.o: EXTERN.h
intrp.o: INTERN.h
intrp.o: artio.h
intrp.o: artsrch.h
intrp.o: bits.h
intrp.o: common.h
intrp.o: config.h
intrp.o: final.h
intrp.o: head.h
intrp.o: intrp.c
intrp.o: intrp.h
intrp.o: ng.h
intrp.o: rcstuff.h
intrp.o: respond.h
intrp.o: rn.h
intrp.o: rthreads.h
intrp.o: search.h
intrp.o: term.h
intrp.o: threads.h
intrp.o: util.h
kfile.o: /usr/include/ctype.h
kfile.o: /usr/include/errno.h
kfile.o: /usr/include/fcntl.h
kfile.o: /usr/include/signal.h
kfile.o: /usr/include/stdio.h
kfile.o: /usr/include/sys/cred.h
kfile.o: /usr/include/sys/errno.h
kfile.o: /usr/include/sys/fcntl.h
kfile.o: /usr/include/sys/ioctl.h
kfile.o: /usr/include/sys/poll.h
kfile.o: /usr/include/sys/ptem.h
kfile.o: /usr/include/sys/select.h
kfile.o: /usr/include/sys/signal.h
kfile.o: /usr/include/sys/stat.h
kfile.o: /usr/include/sys/stream.h
kfile.o: /usr/include/sys/strmdep.h
kfile.o: /usr/include/sys/termio.h
kfile.o: /usr/include/sys/termios.h
kfile.o: /usr/include/sys/time.h
kfile.o: /usr/include/sys/ttydev.h
kfile.o: /usr/include/sys/types.h
kfile.o: /usr/include/sys/vnode.h
kfile.o: /usr/include/termio.h
kfile.o: /usr/include/time.h
kfile.o: EXTERN.h
kfile.o: INTERN.h
kfile.o: artsrch.h
kfile.o: bits.h
kfile.o: common.h
kfile.o: config.h
kfile.o: intrp.h
kfile.o: kfile.c
kfile.o: kfile.h
kfile.o: ng.h
kfile.o: ngstuff.h
kfile.o: rcstuff.h
kfile.o: rn.h
kfile.o: rthreads.h
kfile.o: search.h
kfile.o: term.h
kfile.o: threads.h
kfile.o: util.h
last.o: /usr/include/ctype.h
last.o: /usr/include/errno.h
last.o: /usr/include/fcntl.h
last.o: /usr/include/signal.h
last.o: /usr/include/stdio.h
last.o: /usr/include/sys/cred.h
last.o: /usr/include/sys/errno.h
last.o: /usr/include/sys/fcntl.h
last.o: /usr/include/sys/ioctl.h
last.o: /usr/include/sys/poll.h
last.o: /usr/include/sys/ptem.h
last.o: /usr/include/sys/select.h
last.o: /usr/include/sys/signal.h
last.o: /usr/include/sys/stat.h
last.o: /usr/include/sys/stream.h
last.o: /usr/include/sys/strmdep.h
last.o: /usr/include/sys/termio.h
last.o: /usr/include/sys/termios.h
last.o: /usr/include/sys/time.h
last.o: /usr/include/sys/ttydev.h
last.o: /usr/include/sys/types.h
last.o: /usr/include/sys/vnode.h
last.o: /usr/include/termio.h
last.o: /usr/include/time.h
last.o: EXTERN.h
last.o: INTERN.h
last.o: common.h
last.o: config.h
last.o: intrp.h
last.o: last.c
last.o: last.h
last.o: rn.h
last.o: util.h
mt-misc.o: /usr/include/ctype.h
mt-misc.o: /usr/include/errno.h
mt-misc.o: /usr/include/fcntl.h
mt-misc.o: /usr/include/signal.h
mt-misc.o: /usr/include/stdio.h
mt-misc.o: /usr/include/sys/cred.h
mt-misc.o: /usr/include/sys/errno.h
mt-misc.o: /usr/include/sys/fcntl.h
mt-misc.o: /usr/include/sys/ioctl.h
mt-misc.o: /usr/include/sys/poll.h
mt-misc.o: /usr/include/sys/ptem.h
mt-misc.o: /usr/include/sys/select.h
mt-misc.o: /usr/include/sys/signal.h
mt-misc.o: /usr/include/sys/stat.h
mt-misc.o: /usr/include/sys/stream.h
mt-misc.o: /usr/include/sys/strmdep.h
mt-misc.o: /usr/include/sys/termio.h
mt-misc.o: /usr/include/sys/termios.h
mt-misc.o: /usr/include/sys/time.h
mt-misc.o: /usr/include/sys/ttydev.h
mt-misc.o: /usr/include/sys/types.h
mt-misc.o: /usr/include/sys/vnode.h
mt-misc.o: /usr/include/termio.h
mt-misc.o: /usr/include/time.h
mt-misc.o: EXTERN.h
mt-misc.o: common.h
mt-misc.o: config.h
mt-misc.o: mt-misc.c
mt-misc.o: mthreads.h
mt-misc.o: threads.h
mt-process.o: /usr/include/ctype.h
mt-process.o: /usr/include/dirent.h
mt-process.o: /usr/include/errno.h
mt-process.o: /usr/include/fcntl.h
mt-process.o: /usr/include/signal.h
mt-process.o: /usr/include/stdio.h
mt-process.o: /usr/include/sys/cred.h
mt-process.o: /usr/include/sys/dirent.h
mt-process.o: /usr/include/sys/errno.h
mt-process.o: /usr/include/sys/fcntl.h
mt-process.o: /usr/include/sys/ioctl.h
mt-process.o: /usr/include/sys/poll.h
mt-process.o: /usr/include/sys/ptem.h
mt-process.o: /usr/include/sys/select.h
mt-process.o: /usr/include/sys/signal.h
mt-process.o: /usr/include/sys/stat.h
mt-process.o: /usr/include/sys/stream.h
mt-process.o: /usr/include/sys/strmdep.h
mt-process.o: /usr/include/sys/termio.h
mt-process.o: /usr/include/sys/termios.h
mt-process.o: /usr/include/sys/time.h
mt-process.o: /usr/include/sys/ttydev.h
mt-process.o: /usr/include/sys/types.h
mt-process.o: /usr/include/sys/vnode.h
mt-process.o: /usr/include/termio.h
mt-process.o: /usr/include/time.h
mt-process.o: EXTERN.h
mt-process.o: INTERN.h
mt-process.o: bits.h
mt-process.o: common.h
mt-process.o: config.h
mt-process.o: mt-process.c
mt-process.o: mthreads.h
mt-process.o: ndir.h
mt-process.o: threads.h
mt-read.o: /usr/include/ctype.h
mt-read.o: /usr/include/errno.h
mt-read.o: /usr/include/fcntl.h
mt-read.o: /usr/include/signal.h
mt-read.o: /usr/include/stdio.h
mt-read.o: /usr/include/sys/cred.h
mt-read.o: /usr/include/sys/errno.h
mt-read.o: /usr/include/sys/fcntl.h
mt-read.o: /usr/include/sys/ioctl.h
mt-read.o: /usr/include/sys/poll.h
mt-read.o: /usr/include/sys/ptem.h
mt-read.o: /usr/include/sys/select.h
mt-read.o: /usr/include/sys/signal.h
mt-read.o: /usr/include/sys/stat.h
mt-read.o: /usr/include/sys/stream.h
mt-read.o: /usr/include/sys/strmdep.h
mt-read.o: /usr/include/sys/termio.h
mt-read.o: /usr/include/sys/termios.h
mt-read.o: /usr/include/sys/time.h
mt-read.o: /usr/include/sys/ttydev.h
mt-read.o: /usr/include/sys/types.h
mt-read.o: /usr/include/sys/vnode.h
mt-read.o: /usr/include/termio.h
mt-read.o: /usr/include/time.h
mt-read.o: EXTERN.h
mt-read.o: common.h
mt-read.o: config.h
mt-read.o: mt-read.c
mt-read.o: mthreads.h
mt-read.o: threads.h
mt-write.o: /usr/include/ctype.h
mt-write.o: /usr/include/errno.h
mt-write.o: /usr/include/fcntl.h
mt-write.o: /usr/include/signal.h
mt-write.o: /usr/include/stdio.h
mt-write.o: /usr/include/sys/cred.h
mt-write.o: /usr/include/sys/errno.h
mt-write.o: /usr/include/sys/fcntl.h
mt-write.o: /usr/include/sys/ioctl.h
mt-write.o: /usr/include/sys/poll.h
mt-write.o: /usr/include/sys/ptem.h
mt-write.o: /usr/include/sys/select.h
mt-write.o: /usr/include/sys/signal.h
mt-write.o: /usr/include/sys/stat.h
mt-write.o: /usr/include/sys/stream.h
mt-write.o: /usr/include/sys/strmdep.h
mt-write.o: /usr/include/sys/termio.h
mt-write.o: /usr/include/sys/termios.h
mt-write.o: /usr/include/sys/time.h
mt-write.o: /usr/include/sys/ttydev.h
mt-write.o: /usr/include/sys/types.h
mt-write.o: /usr/include/sys/vnode.h
mt-write.o: /usr/include/termio.h
mt-write.o: /usr/include/time.h
mt-write.o: EXTERN.h
mt-write.o: common.h
mt-write.o: config.h
mt-write.o: mt-write.c
mt-write.o: mthreads.h
mt-write.o: threads.h
mthreads.o: /usr/include/ctype.h
mthreads.o: /usr/include/errno.h
mthreads.o: /usr/include/fcntl.h
mthreads.o: /usr/include/signal.h
mthreads.o: /usr/include/stdio.h
mthreads.o: /usr/include/sys/cred.h
mthreads.o: /usr/include/sys/errno.h
mthreads.o: /usr/include/sys/fcntl.h
mthreads.o: /usr/include/sys/ioctl.h
mthreads.o: /usr/include/sys/poll.h
mthreads.o: /usr/include/sys/ptem.h
mthreads.o: /usr/include/sys/select.h
mthreads.o: /usr/include/sys/signal.h
mthreads.o: /usr/include/sys/stat.h
mthreads.o: /usr/include/sys/stream.h
mthreads.o: /usr/include/sys/strmdep.h
mthreads.o: /usr/include/sys/termio.h
mthreads.o: /usr/include/sys/termios.h
mthreads.o: /usr/include/sys/time.h
mthreads.o: /usr/include/sys/ttydev.h
mthreads.o: /usr/include/sys/types.h
mthreads.o: /usr/include/sys/vnode.h
mthreads.o: /usr/include/termio.h
mthreads.o: /usr/include/time.h
mthreads.o: EXTERN.h
mthreads.o: INTERN.h
mthreads.o: common.h
mthreads.o: config.h
mthreads.o: mthreads.c
mthreads.o: mthreads.h
mthreads.o: threads.h
ndir.o: /usr/include/ctype.h
ndir.o: /usr/include/dirent.h
ndir.o: /usr/include/errno.h
ndir.o: /usr/include/fcntl.h
ndir.o: /usr/include/signal.h
ndir.o: /usr/include/stdio.h
ndir.o: /usr/include/sys/cred.h
ndir.o: /usr/include/sys/dirent.h
ndir.o: /usr/include/sys/errno.h
ndir.o: /usr/include/sys/fcntl.h
ndir.o: /usr/include/sys/ioctl.h
ndir.o: /usr/include/sys/poll.h
ndir.o: /usr/include/sys/ptem.h
ndir.o: /usr/include/sys/select.h
ndir.o: /usr/include/sys/signal.h
ndir.o: /usr/include/sys/stat.h
ndir.o: /usr/include/sys/stream.h
ndir.o: /usr/include/sys/strmdep.h
ndir.o: /usr/include/sys/termio.h
ndir.o: /usr/include/sys/termios.h
ndir.o: /usr/include/sys/time.h
ndir.o: /usr/include/sys/ttydev.h
ndir.o: /usr/include/sys/types.h
ndir.o: /usr/include/sys/vnode.h
ndir.o: /usr/include/termio.h
ndir.o: /usr/include/time.h
ndir.o: EXTERN.h
ndir.o: INTERN.h
ndir.o: common.h
ndir.o: config.h
ndir.o: ndir.c
ndir.o: ndir.h
ng.o: /usr/include/ctype.h
ng.o: /usr/include/errno.h
ng.o: /usr/include/fcntl.h
ng.o: /usr/include/signal.h
ng.o: /usr/include/stdio.h
ng.o: /usr/include/sys/cred.h
ng.o: /usr/include/sys/errno.h
ng.o: /usr/include/sys/fcntl.h
ng.o: /usr/include/sys/ioctl.h
ng.o: /usr/include/sys/poll.h
ng.o: /usr/include/sys/ptem.h
ng.o: /usr/include/sys/select.h
ng.o: /usr/include/sys/signal.h
ng.o: /usr/include/sys/stat.h
ng.o: /usr/include/sys/stream.h
ng.o: /usr/include/sys/strmdep.h
ng.o: /usr/include/sys/termio.h
ng.o: /usr/include/sys/termios.h
ng.o: /usr/include/sys/time.h
ng.o: /usr/include/sys/ttydev.h
ng.o: /usr/include/sys/types.h
ng.o: /usr/include/sys/vnode.h
ng.o: /usr/include/termio.h
ng.o: /usr/include/time.h
ng.o: EXTERN.h
ng.o: INTERN.h
ng.o: art.h
ng.o: artio.h
ng.o: artsrch.h
ng.o: artstate.h
ng.o: backpage.h
ng.o: bits.h
ng.o: cheat.h
ng.o: common.h
ng.o: config.h
ng.o: decode.h
ng.o: final.h
ng.o: head.h
ng.o: help.h
ng.o: intrp.h
ng.o: kfile.h
ng.o: last.h
ng.o: ng.c
ng.o: ng.h
ng.o: ngdata.h
ng.o: ngstuff.h
ng.o: rcln.h
ng.o: rcstuff.h
ng.o: respond.h
ng.o: rn.h
ng.o: rthreads.h
ng.o: search.h
ng.o: term.h
ng.o: threads.h
ng.o: util.h
ngdata.o: /usr/include/ctype.h
ngdata.o: /usr/include/dirent.h
ngdata.o: /usr/include/errno.h
ngdata.o: /usr/include/fcntl.h
ngdata.o: /usr/include/signal.h
ngdata.o: /usr/include/stdio.h
ngdata.o: /usr/include/sys/cred.h
ngdata.o: /usr/include/sys/dirent.h
ngdata.o: /usr/include/sys/errno.h
ngdata.o: /usr/include/sys/fcntl.h
ngdata.o: /usr/include/sys/ioctl.h
ngdata.o: /usr/include/sys/poll.h
ngdata.o: /usr/include/sys/ptem.h
ngdata.o: /usr/include/sys/select.h
ngdata.o: /usr/include/sys/signal.h
ngdata.o: /usr/include/sys/stat.h
ngdata.o: /usr/include/sys/stream.h
ngdata.o: /usr/include/sys/strmdep.h
ngdata.o: /usr/include/sys/termio.h
ngdata.o: /usr/include/sys/termios.h
ngdata.o: /usr/include/sys/time.h
ngdata.o: /usr/include/sys/ttydev.h
ngdata.o: /usr/include/sys/types.h
ngdata.o: /usr/include/sys/vnode.h
ngdata.o: /usr/include/termio.h
ngdata.o: /usr/include/time.h
ngdata.o: EXTERN.h
ngdata.o: INTERN.h
ngdata.o: common.h
ngdata.o: config.h
ngdata.o: final.h
ngdata.o: intrp.h
ngdata.o: ndir.h
ngdata.o: ng.h
ngdata.o: ngdata.c
ngdata.o: ngdata.h
ngdata.o: rcln.h
ngdata.o: rcstuff.h
ngdata.o: rn.h
ngdata.o: util.h
ngsrch.o: /usr/include/ctype.h
ngsrch.o: /usr/include/errno.h
ngsrch.o: /usr/include/fcntl.h
ngsrch.o: /usr/include/signal.h
ngsrch.o: /usr/include/stdio.h
ngsrch.o: /usr/include/sys/cred.h
ngsrch.o: /usr/include/sys/errno.h
ngsrch.o: /usr/include/sys/fcntl.h
ngsrch.o: /usr/include/sys/ioctl.h
ngsrch.o: /usr/include/sys/poll.h
ngsrch.o: /usr/include/sys/ptem.h
ngsrch.o: /usr/include/sys/select.h
ngsrch.o: /usr/include/sys/signal.h
ngsrch.o: /usr/include/sys/stat.h
ngsrch.o: /usr/include/sys/stream.h
ngsrch.o: /usr/include/sys/strmdep.h
ngsrch.o: /usr/include/sys/termio.h
ngsrch.o: /usr/include/sys/termios.h
ngsrch.o: /usr/include/sys/time.h
ngsrch.o: /usr/include/sys/ttydev.h
ngsrch.o: /usr/include/sys/types.h
ngsrch.o: /usr/include/sys/vnode.h
ngsrch.o: /usr/include/termio.h
ngsrch.o: /usr/include/time.h
ngsrch.o: EXTERN.h
ngsrch.o: INTERN.h
ngsrch.o: common.h
ngsrch.o: config.h
ngsrch.o: final.h
ngsrch.o: ngsrch.c
ngsrch.o: ngsrch.h
ngsrch.o: rcln.h
ngsrch.o: rcstuff.h
ngsrch.o: rn.h
ngsrch.o: search.h
ngsrch.o: term.h
ngsrch.o: util.h
ngstuff.o: /usr/include/ctype.h
ngstuff.o: /usr/include/errno.h
ngstuff.o: /usr/include/fcntl.h
ngstuff.o: /usr/include/signal.h
ngstuff.o: /usr/include/stdio.h
ngstuff.o: /usr/include/sys/cred.h
ngstuff.o: /usr/include/sys/errno.h
ngstuff.o: /usr/include/sys/fcntl.h
ngstuff.o: /usr/include/sys/ioctl.h
ngstuff.o: /usr/include/sys/poll.h
ngstuff.o: /usr/include/sys/ptem.h
ngstuff.o: /usr/include/sys/select.h
ngstuff.o: /usr/include/sys/signal.h
ngstuff.o: /usr/include/sys/stat.h
ngstuff.o: /usr/include/sys/stream.h
ngstuff.o: /usr/include/sys/strmdep.h
ngstuff.o: /usr/include/sys/termio.h
ngstuff.o: /usr/include/sys/termios.h
ngstuff.o: /usr/include/sys/time.h
ngstuff.o: /usr/include/sys/ttydev.h
ngstuff.o: /usr/include/sys/types.h
ngstuff.o: /usr/include/sys/vnode.h
ngstuff.o: /usr/include/termio.h
ngstuff.o: /usr/include/time.h
ngstuff.o: EXTERN.h
ngstuff.o: INTERN.h
ngstuff.o: bits.h
ngstuff.o: cheat.h
ngstuff.o: common.h
ngstuff.o: config.h
ngstuff.o: decode.h
ngstuff.o: final.h
ngstuff.o: head.h
ngstuff.o: intrp.h
ngstuff.o: ng.h
ngstuff.o: ngstuff.c
ngstuff.o: ngstuff.h
ngstuff.o: rcstuff.h
ngstuff.o: rn.h
ngstuff.o: rthreads.h
ngstuff.o: sw.h
ngstuff.o: term.h
ngstuff.o: threads.h
ngstuff.o: util.h
only.o: /usr/include/ctype.h
only.o: /usr/include/errno.h
only.o: /usr/include/fcntl.h
only.o: /usr/include/signal.h
only.o: /usr/include/stdio.h
only.o: /usr/include/sys/cred.h
only.o: /usr/include/sys/errno.h
only.o: /usr/include/sys/fcntl.h
only.o: /usr/include/sys/ioctl.h
only.o: /usr/include/sys/poll.h
only.o: /usr/include/sys/ptem.h
only.o: /usr/include/sys/select.h
only.o: /usr/include/sys/signal.h
only.o: /usr/include/sys/stat.h
only.o: /usr/include/sys/stream.h
only.o: /usr/include/sys/strmdep.h
only.o: /usr/include/sys/termio.h
only.o: /usr/include/sys/termios.h
only.o: /usr/include/sys/time.h
only.o: /usr/include/sys/ttydev.h
only.o: /usr/include/sys/types.h
only.o: /usr/include/sys/vnode.h
only.o: /usr/include/termio.h
only.o: /usr/include/time.h
only.o: EXTERN.h
only.o: INTERN.h
only.o: common.h
only.o: config.h
only.o: final.h
only.o: ngsrch.h
only.o: only.c
only.o: only.h
only.o: search.h
only.o: util.h
rcln.o: /usr/include/ctype.h
rcln.o: /usr/include/errno.h
rcln.o: /usr/include/fcntl.h
rcln.o: /usr/include/signal.h
rcln.o: /usr/include/stdio.h
rcln.o: /usr/include/sys/cred.h
rcln.o: /usr/include/sys/errno.h
rcln.o: /usr/include/sys/fcntl.h
rcln.o: /usr/include/sys/ioctl.h
rcln.o: /usr/include/sys/poll.h
rcln.o: /usr/include/sys/ptem.h
rcln.o: /usr/include/sys/select.h
rcln.o: /usr/include/sys/signal.h
rcln.o: /usr/include/sys/stat.h
rcln.o: /usr/include/sys/stream.h
rcln.o: /usr/include/sys/strmdep.h
rcln.o: /usr/include/sys/termio.h
rcln.o: /usr/include/sys/termios.h
rcln.o: /usr/include/sys/time.h
rcln.o: /usr/include/sys/ttydev.h
rcln.o: /usr/include/sys/types.h
rcln.o: /usr/include/sys/vnode.h
rcln.o: /usr/include/termio.h
rcln.o: /usr/include/time.h
rcln.o: EXTERN.h
rcln.o: INTERN.h
rcln.o: common.h
rcln.o: config.h
rcln.o: ngdata.h
rcln.o: rcln.c
rcln.o: rcln.h
rcln.o: rcstuff.h
rcln.o: util.h
rcstuff.o: /usr/include/ctype.h
rcstuff.o: /usr/include/errno.h
rcstuff.o: /usr/include/fcntl.h
rcstuff.o: /usr/include/signal.h
rcstuff.o: /usr/include/stdio.h
rcstuff.o: /usr/include/sys/cred.h
rcstuff.o: /usr/include/sys/errno.h
rcstuff.o: /usr/include/sys/fcntl.h
rcstuff.o: /usr/include/sys/ioctl.h
rcstuff.o: /usr/include/sys/poll.h
rcstuff.o: /usr/include/sys/ptem.h
rcstuff.o: /usr/include/sys/select.h
rcstuff.o: /usr/include/sys/signal.h
rcstuff.o: /usr/include/sys/stat.h
rcstuff.o: /usr/include/sys/stream.h
rcstuff.o: /usr/include/sys/strmdep.h
rcstuff.o: /usr/include/sys/termio.h
rcstuff.o: /usr/include/sys/termios.h
rcstuff.o: /usr/include/sys/time.h
rcstuff.o: /usr/include/sys/ttydev.h
rcstuff.o: /usr/include/sys/types.h
rcstuff.o: /usr/include/sys/vnode.h
rcstuff.o: /usr/include/termio.h
rcstuff.o: /usr/include/time.h
rcstuff.o: EXTERN.h
rcstuff.o: INTERN.h
rcstuff.o: autosub.h
rcstuff.o: common.h
rcstuff.o: config.h
rcstuff.o: final.h
rcstuff.o: intrp.h
rcstuff.o: ngdata.h
rcstuff.o: only.h
rcstuff.o: rcln.h
rcstuff.o: rcstuff.c
rcstuff.o: rcstuff.h
rcstuff.o: rn.h
rcstuff.o: search.h
rcstuff.o: term.h
rcstuff.o: util.h
respond.o: /usr/include/ctype.h
respond.o: /usr/include/errno.h
respond.o: /usr/include/fcntl.h
respond.o: /usr/include/signal.h
respond.o: /usr/include/stdio.h
respond.o: /usr/include/sys/cred.h
respond.o: /usr/include/sys/errno.h
respond.o: /usr/include/sys/fcntl.h
respond.o: /usr/include/sys/ioctl.h
respond.o: /usr/include/sys/poll.h
respond.o: /usr/include/sys/ptem.h
respond.o: /usr/include/sys/select.h
respond.o: /usr/include/sys/signal.h
respond.o: /usr/include/sys/stat.h
respond.o: /usr/include/sys/stream.h
respond.o: /usr/include/sys/strmdep.h
respond.o: /usr/include/sys/termio.h
respond.o: /usr/include/sys/termios.h
respond.o: /usr/include/sys/time.h
respond.o: /usr/include/sys/ttydev.h
respond.o: /usr/include/sys/types.h
respond.o: /usr/include/sys/vnode.h
respond.o: /usr/include/termio.h
respond.o: /usr/include/time.h
respond.o: EXTERN.h
respond.o: INTERN.h
respond.o: artio.h
respond.o: bits.h
respond.o: common.h
respond.o: config.h
respond.o: decode.h
respond.o: final.h
respond.o: head.h
respond.o: intrp.h
respond.o: ng.h
respond.o: respond.c
respond.o: respond.h
respond.o: rn.h
respond.o: term.h
respond.o: util.h
rn.o: /usr/include/ctype.h
rn.o: /usr/include/errno.h
rn.o: /usr/include/fcntl.h
rn.o: /usr/include/signal.h
rn.o: /usr/include/stdio.h
rn.o: /usr/include/sys/cred.h
rn.o: /usr/include/sys/errno.h
rn.o: /usr/include/sys/fcntl.h
rn.o: /usr/include/sys/ioctl.h
rn.o: /usr/include/sys/poll.h
rn.o: /usr/include/sys/ptem.h
rn.o: /usr/include/sys/select.h
rn.o: /usr/include/sys/signal.h
rn.o: /usr/include/sys/stat.h
rn.o: /usr/include/sys/stream.h
rn.o: /usr/include/sys/strmdep.h
rn.o: /usr/include/sys/termio.h
rn.o: /usr/include/sys/termios.h
rn.o: /usr/include/sys/time.h
rn.o: /usr/include/sys/ttydev.h
rn.o: /usr/include/sys/types.h
rn.o: /usr/include/sys/vnode.h
rn.o: /usr/include/termio.h
rn.o: /usr/include/time.h
rn.o: EXTERN.h
rn.o: INTERN.h
rn.o: addng.h
rn.o: common.h
rn.o: config.h
rn.o: final.h
rn.o: help.h
rn.o: init.h
rn.o: intrp.h
rn.o: last.h
rn.o: ng.h
rn.o: ngdata.h
rn.o: ngsrch.h
rn.o: only.h
rn.o: rcln.h
rn.o: rcstuff.h
rn.o: rn.c
rn.o: rn.h
rn.o: search.h
rn.o: sw.h
rn.o: term.h
rn.o: util.h
rt-rn.o: /usr/include/ctype.h
rt-rn.o: /usr/include/errno.h
rt-rn.o: /usr/include/fcntl.h
rt-rn.o: /usr/include/signal.h
rt-rn.o: /usr/include/stdio.h
rt-rn.o: /usr/include/sys/cred.h
rt-rn.o: /usr/include/sys/errno.h
rt-rn.o: /usr/include/sys/fcntl.h
rt-rn.o: /usr/include/sys/ioctl.h
rt-rn.o: /usr/include/sys/poll.h
rt-rn.o: /usr/include/sys/ptem.h
rt-rn.o: /usr/include/sys/select.h
rt-rn.o: /usr/include/sys/signal.h
rt-rn.o: /usr/include/sys/stat.h
rt-rn.o: /usr/include/sys/stream.h
rt-rn.o: /usr/include/sys/strmdep.h
rt-rn.o: /usr/include/sys/termio.h
rt-rn.o: /usr/include/sys/termios.h
rt-rn.o: /usr/include/sys/time.h
rt-rn.o: /usr/include/sys/ttydev.h
rt-rn.o: /usr/include/sys/types.h
rt-rn.o: /usr/include/sys/vnode.h
rt-rn.o: /usr/include/termio.h
rt-rn.o: /usr/include/time.h
rt-rn.o: EXTERN.h
rt-rn.o: artio.h
rt-rn.o: artstate.h
rt-rn.o: backpage.h
rt-rn.o: bits.h
rt-rn.o: common.h
rt-rn.o: config.h
rt-rn.o: final.h
rt-rn.o: ng.h
rt-rn.o: ngdata.h
rt-rn.o: rt-rn.c
rt-rn.o: rthreads.h
rt-rn.o: search.h
rt-rn.o: term.h
rt-rn.o: threads.h
rt-rn.o: util.h
rt-select.o: /usr/include/ctype.h
rt-select.o: /usr/include/errno.h
rt-select.o: /usr/include/fcntl.h
rt-select.o: /usr/include/signal.h
rt-select.o: /usr/include/stdio.h
rt-select.o: /usr/include/sys/cred.h
rt-select.o: /usr/include/sys/errno.h
rt-select.o: /usr/include/sys/fcntl.h
rt-select.o: /usr/include/sys/ioctl.h
rt-select.o: /usr/include/sys/poll.h
rt-select.o: /usr/include/sys/ptem.h
rt-select.o: /usr/include/sys/select.h
rt-select.o: /usr/include/sys/signal.h
rt-select.o: /usr/include/sys/stat.h
rt-select.o: /usr/include/sys/stream.h
rt-select.o: /usr/include/sys/strmdep.h
rt-select.o: /usr/include/sys/termio.h
rt-select.o: /usr/include/sys/termios.h
rt-select.o: /usr/include/sys/time.h
rt-select.o: /usr/include/sys/ttydev.h
rt-select.o: /usr/include/sys/types.h
rt-select.o: /usr/include/sys/vnode.h
rt-select.o: /usr/include/termio.h
rt-select.o: /usr/include/time.h
rt-select.o: EXTERN.h
rt-select.o: artsrch.h
rt-select.o: bits.h
rt-select.o: common.h
rt-select.o: config.h
rt-select.o: final.h
rt-select.o: help.h
rt-select.o: ng.h
rt-select.o: ngdata.h
rt-select.o: ngstuff.h
rt-select.o: rcstuff.h
rt-select.o: rn.h
rt-select.o: rt-select.c
rt-select.o: rthreads.h
rt-select.o: search.h
rt-select.o: term.h
rt-select.o: threads.h
rt-select.o: util.h
rthreads.o: /usr/include/ctype.h
rthreads.o: /usr/include/errno.h
rthreads.o: /usr/include/fcntl.h
rthreads.o: /usr/include/signal.h
rthreads.o: /usr/include/stdio.h
rthreads.o: /usr/include/sys/cred.h
rthreads.o: /usr/include/sys/errno.h
rthreads.o: /usr/include/sys/fcntl.h
rthreads.o: /usr/include/sys/ioctl.h
rthreads.o: /usr/include/sys/poll.h
rthreads.o: /usr/include/sys/ptem.h
rthreads.o: /usr/include/sys/select.h
rthreads.o: /usr/include/sys/signal.h
rthreads.o: /usr/include/sys/stat.h
rthreads.o: /usr/include/sys/stream.h
rthreads.o: /usr/include/sys/strmdep.h
rthreads.o: /usr/include/sys/termio.h
rthreads.o: /usr/include/sys/termios.h
rthreads.o: /usr/include/sys/time.h
rthreads.o: /usr/include/sys/ttydev.h
rthreads.o: /usr/include/sys/types.h
rthreads.o: /usr/include/sys/vnode.h
rthreads.o: /usr/include/termio.h
rthreads.o: /usr/include/time.h
rthreads.o: EXTERN.h
rthreads.o: INTERN.h
rthreads.o: bits.h
rthreads.o: common.h
rthreads.o: config.h
rthreads.o: intrp.h
rthreads.o: ngdata.h
rthreads.o: rcln.h
rthreads.o: rn.h
rthreads.o: rthreads.c
rthreads.o: rthreads.h
rthreads.o: threads.h
rthreads.o: util.h
search.o: /usr/include/ctype.h
search.o: /usr/include/errno.h
search.o: /usr/include/fcntl.h
search.o: /usr/include/signal.h
search.o: /usr/include/stdio.h
search.o: /usr/include/sys/cred.h
search.o: /usr/include/sys/errno.h
search.o: /usr/include/sys/fcntl.h
search.o: /usr/include/sys/ioctl.h
search.o: /usr/include/sys/poll.h
search.o: /usr/include/sys/ptem.h
search.o: /usr/include/sys/select.h
search.o: /usr/include/sys/signal.h
search.o: /usr/include/sys/stat.h
search.o: /usr/include/sys/stream.h
search.o: /usr/include/sys/strmdep.h
search.o: /usr/include/sys/termio.h
search.o: /usr/include/sys/termios.h
search.o: /usr/include/sys/time.h
search.o: /usr/include/sys/ttydev.h
search.o: /usr/include/sys/types.h
search.o: /usr/include/sys/vnode.h
search.o: /usr/include/termio.h
search.o: /usr/include/time.h
search.o: EXTERN.h
search.o: INTERN.h
search.o: common.h
search.o: config.h
search.o: search.c
search.o: search.h
search.o: util.h
server.o: /usr/include/ctype.h
server.o: /usr/include/errno.h
server.o: /usr/include/fcntl.h
server.o: /usr/include/signal.h
server.o: /usr/include/stdio.h
server.o: /usr/include/sys/cred.h
server.o: /usr/include/sys/errno.h
server.o: /usr/include/sys/fcntl.h
server.o: /usr/include/sys/ioctl.h
server.o: /usr/include/sys/poll.h
server.o: /usr/include/sys/ptem.h
server.o: /usr/include/sys/select.h
server.o: /usr/include/sys/signal.h
server.o: /usr/include/sys/stat.h
server.o: /usr/include/sys/stream.h
server.o: /usr/include/sys/strmdep.h
server.o: /usr/include/sys/termio.h
server.o: /usr/include/sys/termios.h
server.o: /usr/include/sys/time.h
server.o: /usr/include/sys/ttydev.h
server.o: /usr/include/sys/types.h
server.o: /usr/include/sys/vnode.h
server.o: /usr/include/termio.h
server.o: /usr/include/time.h
server.o: EXTERN.h
server.o: common.h
server.o: config.h
server.o: server.c
server.o: threads.h
sw.o: /usr/include/ctype.h
sw.o: /usr/include/errno.h
sw.o: /usr/include/fcntl.h
sw.o: /usr/include/signal.h
sw.o: /usr/include/stdio.h
sw.o: /usr/include/sys/cred.h
sw.o: /usr/include/sys/errno.h
sw.o: /usr/include/sys/fcntl.h
sw.o: /usr/include/sys/ioctl.h
sw.o: /usr/include/sys/poll.h
sw.o: /usr/include/sys/ptem.h
sw.o: /usr/include/sys/select.h
sw.o: /usr/include/sys/signal.h
sw.o: /usr/include/sys/stat.h
sw.o: /usr/include/sys/stream.h
sw.o: /usr/include/sys/strmdep.h
sw.o: /usr/include/sys/termio.h
sw.o: /usr/include/sys/termios.h
sw.o: /usr/include/sys/time.h
sw.o: /usr/include/sys/ttydev.h
sw.o: /usr/include/sys/types.h
sw.o: /usr/include/sys/vnode.h
sw.o: /usr/include/termio.h
sw.o: /usr/include/time.h
sw.o: EXTERN.h
sw.o: INTERN.h
sw.o: common.h
sw.o: config.h
sw.o: head.h
sw.o: intrp.h
sw.o: ng.h
sw.o: only.h
sw.o: search.h
sw.o: sw.c
sw.o: sw.h
sw.o: term.h
sw.o: util.h
term.o: /usr/include/ctype.h
term.o: /usr/include/errno.h
term.o: /usr/include/fcntl.h
term.o: /usr/include/signal.h
term.o: /usr/include/stdio.h
term.o: /usr/include/sys/cred.h
term.o: /usr/include/sys/errno.h
term.o: /usr/include/sys/fcntl.h
term.o: /usr/include/sys/ioctl.h
term.o: /usr/include/sys/poll.h
term.o: /usr/include/sys/ptem.h
term.o: /usr/include/sys/select.h
term.o: /usr/include/sys/signal.h
term.o: /usr/include/sys/stat.h
term.o: /usr/include/sys/stream.h
term.o: /usr/include/sys/strmdep.h
term.o: /usr/include/sys/termio.h
term.o: /usr/include/sys/termios.h
term.o: /usr/include/sys/time.h
term.o: /usr/include/sys/ttydev.h
term.o: /usr/include/sys/types.h
term.o: /usr/include/sys/vnode.h
term.o: /usr/include/termio.h
term.o: /usr/include/time.h
term.o: EXTERN.h
term.o: INTERN.h
term.o: cheat.h
term.o: common.h
term.o: config.h
term.o: final.h
term.o: help.h
term.o: intrp.h
term.o: term.c
term.o: term.h
term.o: util.h
threads.o: /usr/include/ctype.h
threads.o: /usr/include/errno.h
threads.o: /usr/include/fcntl.h
threads.o: /usr/include/signal.h
threads.o: /usr/include/stdio.h
threads.o: /usr/include/sys/cred.h
threads.o: /usr/include/sys/errno.h
threads.o: /usr/include/sys/fcntl.h
threads.o: /usr/include/sys/ioctl.h
threads.o: /usr/include/sys/poll.h
threads.o: /usr/include/sys/ptem.h
threads.o: /usr/include/sys/select.h
threads.o: /usr/include/sys/signal.h
threads.o: /usr/include/sys/stat.h
threads.o: /usr/include/sys/stream.h
threads.o: /usr/include/sys/strmdep.h
threads.o: /usr/include/sys/termio.h
threads.o: /usr/include/sys/termios.h
threads.o: /usr/include/sys/time.h
threads.o: /usr/include/sys/ttydev.h
threads.o: /usr/include/sys/types.h
threads.o: /usr/include/sys/vnode.h
threads.o: /usr/include/termio.h
threads.o: /usr/include/time.h
threads.o: EXTERN.h
threads.o: INTERN.h
threads.o: common.h
threads.o: config.h
threads.o: threads.c
threads.o: threads.h
tm-process.o: /usr/include/ctype.h
tm-process.o: /usr/include/dirent.h
tm-process.o: /usr/include/errno.h
tm-process.o: /usr/include/fcntl.h
tm-process.o: /usr/include/signal.h
tm-process.o: /usr/include/stdio.h
tm-process.o: /usr/include/sys/cred.h
tm-process.o: /usr/include/sys/dirent.h
tm-process.o: /usr/include/sys/errno.h
tm-process.o: /usr/include/sys/fcntl.h
tm-process.o: /usr/include/sys/ioctl.h
tm-process.o: /usr/include/sys/poll.h
tm-process.o: /usr/include/sys/ptem.h
tm-process.o: /usr/include/sys/select.h
tm-process.o: /usr/include/sys/signal.h
tm-process.o: /usr/include/sys/stat.h
tm-process.o: /usr/include/sys/stream.h
tm-process.o: /usr/include/sys/strmdep.h
tm-process.o: /usr/include/sys/termio.h
tm-process.o: /usr/include/sys/termios.h
tm-process.o: /usr/include/sys/time.h
tm-process.o: /usr/include/sys/ttydev.h
tm-process.o: /usr/include/sys/types.h
tm-process.o: /usr/include/sys/vnode.h
tm-process.o: /usr/include/termio.h
tm-process.o: /usr/include/time.h
tm-process.o: EXTERN.h
tm-process.o: INTERN.h
tm-process.o: bits.h
tm-process.o: common.h
tm-process.o: config.h
tm-process.o: mt-process.c
tm-process.o: mthreads.h
tm-process.o: ndir.h
tm-process.o: threads.h
tm-process.o: tm-process.c
tm-read.o: /usr/include/ctype.h
tm-read.o: /usr/include/errno.h
tm-read.o: /usr/include/fcntl.h
tm-read.o: /usr/include/signal.h
tm-read.o: /usr/include/stdio.h
tm-read.o: /usr/include/sys/cred.h
tm-read.o: /usr/include/sys/errno.h
tm-read.o: /usr/include/sys/fcntl.h
tm-read.o: /usr/include/sys/ioctl.h
tm-read.o: /usr/include/sys/poll.h
tm-read.o: /usr/include/sys/ptem.h
tm-read.o: /usr/include/sys/select.h
tm-read.o: /usr/include/sys/signal.h
tm-read.o: /usr/include/sys/stat.h
tm-read.o: /usr/include/sys/stream.h
tm-read.o: /usr/include/sys/strmdep.h
tm-read.o: /usr/include/sys/termio.h
tm-read.o: /usr/include/sys/termios.h
tm-read.o: /usr/include/sys/time.h
tm-read.o: /usr/include/sys/ttydev.h
tm-read.o: /usr/include/sys/types.h
tm-read.o: /usr/include/sys/vnode.h
tm-read.o: /usr/include/termio.h
tm-read.o: /usr/include/time.h
tm-read.o: EXTERN.h
tm-read.o: common.h
tm-read.o: config.h
tm-read.o: mt-read.c
tm-read.o: mthreads.h
tm-read.o: threads.h
tm-read.o: tm-read.c
tm-write.o: /usr/include/ctype.h
tm-write.o: /usr/include/errno.h
tm-write.o: /usr/include/fcntl.h
tm-write.o: /usr/include/signal.h
tm-write.o: /usr/include/stdio.h
tm-write.o: /usr/include/sys/cred.h
tm-write.o: /usr/include/sys/errno.h
tm-write.o: /usr/include/sys/fcntl.h
tm-write.o: /usr/include/sys/ioctl.h
tm-write.o: /usr/include/sys/poll.h
tm-write.o: /usr/include/sys/ptem.h
tm-write.o: /usr/include/sys/select.h
tm-write.o: /usr/include/sys/signal.h
tm-write.o: /usr/include/sys/stat.h
tm-write.o: /usr/include/sys/stream.h
tm-write.o: /usr/include/sys/strmdep.h
tm-write.o: /usr/include/sys/termio.h
tm-write.o: /usr/include/sys/termios.h
tm-write.o: /usr/include/sys/time.h
tm-write.o: /usr/include/sys/ttydev.h
tm-write.o: /usr/include/sys/types.h
tm-write.o: /usr/include/sys/vnode.h
tm-write.o: /usr/include/termio.h
tm-write.o: /usr/include/time.h
tm-write.o: EXTERN.h
tm-write.o: common.h
tm-write.o: config.h
tm-write.o: mt-write.c
tm-write.o: mthreads.h
tm-write.o: threads.h
tm-write.o: tm-write.c
tmpthread.o: /usr/include/ctype.h
tmpthread.o: /usr/include/errno.h
tmpthread.o: /usr/include/fcntl.h
tmpthread.o: /usr/include/signal.h
tmpthread.o: /usr/include/stdio.h
tmpthread.o: /usr/include/sys/cred.h
tmpthread.o: /usr/include/sys/errno.h
tmpthread.o: /usr/include/sys/fcntl.h
tmpthread.o: /usr/include/sys/ioctl.h
tmpthread.o: /usr/include/sys/poll.h
tmpthread.o: /usr/include/sys/ptem.h
tmpthread.o: /usr/include/sys/select.h
tmpthread.o: /usr/include/sys/signal.h
tmpthread.o: /usr/include/sys/stat.h
tmpthread.o: /usr/include/sys/stream.h
tmpthread.o: /usr/include/sys/strmdep.h
tmpthread.o: /usr/include/sys/termio.h
tmpthread.o: /usr/include/sys/termios.h
tmpthread.o: /usr/include/sys/time.h
tmpthread.o: /usr/include/sys/ttydev.h
tmpthread.o: /usr/include/sys/types.h
tmpthread.o: /usr/include/sys/vnode.h
tmpthread.o: /usr/include/termio.h
tmpthread.o: /usr/include/time.h
tmpthread.o: EXTERN.h
tmpthread.o: INTERN.h
tmpthread.o: common.h
tmpthread.o: config.h
tmpthread.o: mthreads.h
tmpthread.o: threads.h
tmpthread.o: tmpthread.c
unipatch.o: /usr/include/stdio.h
unipatch.o: unipatch.c
unship.o: /usr/include/ctype.h
unship.o: /usr/include/errno.h
unship.o: /usr/include/fcntl.h
unship.o: /usr/include/signal.h
unship.o: /usr/include/stdio.h
unship.o: /usr/include/sys/cred.h
unship.o: /usr/include/sys/errno.h
unship.o: /usr/include/sys/fcntl.h
unship.o: /usr/include/sys/ioctl.h
unship.o: /usr/include/sys/poll.h
unship.o: /usr/include/sys/ptem.h
unship.o: /usr/include/sys/select.h
unship.o: /usr/include/sys/signal.h
unship.o: /usr/include/sys/stat.h
unship.o: /usr/include/sys/stream.h
unship.o: /usr/include/sys/strmdep.h
unship.o: /usr/include/sys/termio.h
unship.o: /usr/include/sys/termios.h
unship.o: /usr/include/sys/time.h
unship.o: /usr/include/sys/ttydev.h
unship.o: /usr/include/sys/types.h
unship.o: /usr/include/sys/vnode.h
unship.o: /usr/include/termio.h
unship.o: /usr/include/time.h
unship.o: EXTERN.h
unship.o: common.h
unship.o: config.h
unship.o: decode.h
unship.o: respond.h
unship.o: unship.c
util.o: /usr/include/ctype.h
util.o: /usr/include/errno.h
util.o: /usr/include/fcntl.h
util.o: /usr/include/signal.h
util.o: /usr/include/stdio.h
util.o: /usr/include/sys/cred.h
util.o: /usr/include/sys/errno.h
util.o: /usr/include/sys/fcntl.h
util.o: /usr/include/sys/ioctl.h
util.o: /usr/include/sys/poll.h
util.o: /usr/include/sys/ptem.h
util.o: /usr/include/sys/select.h
util.o: /usr/include/sys/signal.h
util.o: /usr/include/sys/stat.h
util.o: /usr/include/sys/stream.h
util.o: /usr/include/sys/strmdep.h
util.o: /usr/include/sys/termio.h
util.o: /usr/include/sys/termios.h
util.o: /usr/include/sys/time.h
util.o: /usr/include/sys/ttydev.h
util.o: /usr/include/sys/types.h
util.o: /usr/include/sys/vnode.h
util.o: /usr/include/termio.h
util.o: /usr/include/time.h
util.o: EXTERN.h
util.o: INTERN.h
util.o: common.h
util.o: config.h
util.o: final.h
util.o: util.c
util.o: util.h
uudecode.o: /usr/include/ctype.h
uudecode.o: /usr/include/errno.h
uudecode.o: /usr/include/fcntl.h
uudecode.o: /usr/include/signal.h
uudecode.o: /usr/include/stdio.h
uudecode.o: /usr/include/sys/cred.h
uudecode.o: /usr/include/sys/errno.h
uudecode.o: /usr/include/sys/fcntl.h
uudecode.o: /usr/include/sys/ioctl.h
uudecode.o: /usr/include/sys/poll.h
uudecode.o: /usr/include/sys/ptem.h
uudecode.o: /usr/include/sys/select.h
uudecode.o: /usr/include/sys/signal.h
uudecode.o: /usr/include/sys/stat.h
uudecode.o: /usr/include/sys/stream.h
uudecode.o: /usr/include/sys/strmdep.h
uudecode.o: /usr/include/sys/termio.h
uudecode.o: /usr/include/sys/termios.h
uudecode.o: /usr/include/sys/time.h
uudecode.o: /usr/include/sys/ttydev.h
uudecode.o: /usr/include/sys/types.h
uudecode.o: /usr/include/sys/vnode.h
uudecode.o: /usr/include/termio.h
uudecode.o: /usr/include/time.h
uudecode.o: EXTERN.h
uudecode.o: common.h
uudecode.o: config.h
uudecode.o: decode.h
uudecode.o: respond.h
uudecode.o: uudecode.c
Makefile: Makefile.SH config.sh ; /bin/sh Makefile.SH
Pnews: Pnews.SH config.sh ; /bin/sh Pnews.SH
Rnmail: Rnmail.SH config.sh ; /bin/sh Rnmail.SH
config.h: config.h.SH config.sh ; /bin/sh config.h.SH
makedepend: makedepend.SH config.sh ; /bin/sh makedepend.SH
makedir: makedir.SH config.sh ; /bin/sh makedir.SH
mbox.saver: mbox.saver.SH config.sh ; /bin/sh mbox.saver.SH
mt.check: mt.check.SH config.sh ; /bin/sh mt.check.SH
ndir.h: ndir.h.SH config.sh ; /bin/sh ndir.h.SH
newsetup: newsetup.SH config.sh ; /bin/sh newsetup.SH
newsgroups: newsgroups.SH config.sh ; /bin/sh newsgroups.SH
newsnews: newsnews.SH config.sh ; /bin/sh newsnews.SH
norm.saver: norm.saver.SH config.sh ; /bin/sh norm.saver.SH
server.h: server.h.SH config.sh ; /bin/sh server.h.SH
# WARNING: Put nothing here or make depend will gobble it up!
