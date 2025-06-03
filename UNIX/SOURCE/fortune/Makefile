# $Header: Makefile,v 1.3 86/09/16 17:27:19 arnold Exp $
#
# Makefile for "fortune" program.  See READ_ME for details.
#
FORTUNES=	scene obscene
CFILES=		fortune.c strfile.h strfile.c rnd.c unstr.c
SOURCE=		$(CFILES) $(FORTUNES)
TFILES=		Troff.mac Troff.sed Do_troff
SFILES=		scene.sp.ok obscene.sp.ok jargon
MISC=		READ_ME fortune.6 strfile.8 Makefile Notes Rot13 Shar_split :trfix
LIBDIR=		/opt/games
BINDIR=		/opt/bin
USER=		daemon			# owner of fortune system
DEFS=					# BSD systems
#DEFS=		-DNO_RANDOM -DREGCMP	# System V systems
CFLAGS=		-g -O3 -DSYSV $(DEFS) -DFORTFILE=\"$(LIBDIR)/fortunes.dat\" -DJARGFILE=\"$(LIBDIR)/jargon.txt\"
#LIBS=		-lcompat		# 4.4BSD systems
#LIBS=					# 4.[0123]BSD systems
#LIBS=		-lPW			# System V systems
LIBS	=	-lregex -L/usr/ucblib -lucb		# System V.4 systems
SFLAGS=		-r
ARF=		fortune.ar
TARF=		/dev/rmt8
SHARF=		fort.shar
SHARFLAGS=	-vmpX
TDEV=
TROFF=		psroff $(TDEV)
MANDIR	=	/opt/man

# .DEFAULT:
# 	cat $<.?? > $<
#	rm -f $<.??
#	chmod -r $<

all: fortune strfile unstr fortunes.dat obscenities.dat

fortune: fortune.o rnd.o
	$(CC) $(CFLAGS) -o fortune fortune.o rnd.o $(LIBS)

strfile: strfile.o rnd.o
	$(CC) $(CFLAGS) -o strfile strfile.o rnd.o $(LIBS)

unstr: unstr.o
	$(CC) $(CFLAGS) -o unstr unstr.o $(LIBS)

fortune.o strfile.o unstr.o: strfile.h

fortunes.dat: fortunes strfile
	./strfile $(SFLAGS) fortunes

obscenities.dat: obscenities strfile
	./strfile $(SFLAGS) obscenities

fortunes: $(FORTUNES) jargon.msg adventure
	cp scene fortunes
	chmod u+w fortunes
	cat jargon.msg >> fortunes
	cat adventure >> fortunes
	echo "%-" >> fortunes
	echo "kludge at end" >> fortunes

obscenities: $(FORTUNES) jargon.msg adventure
	cp scene obscenities
	cat obscene >> obscenities
	chmod u+w obscenities
	cat jargon.msg >> obscenities
	cat adventure >> obscenities
	echo "%%" >> obscenities
	cat obscene >> obscenities
	echo "%-" >> obscenities
	echo "kludge at end" >> obscenities

lint:
	lint -hxb $(DEFS) fortune.c rnd.c 2>&1 > fortune.lint
	lint -hxb $(DEFS) strfile.c rnd.c 2>&1 > strfile.lint
	lint -hxb $(DEFS) unstr.c 2>&1 > unstr.lint

tags: $(CFILES)
	ctags $(CFILES)

install-both: all jargon install.data additional.obscenities
	cp -p fortune jargon strfile $(BINDIR)/
#	strip $(BINDIR)/fortune
	chown $(USER) $(BINDIR)/fortune
	chmod 4711 $(BINDIR)/fortune
	if [ ! -d $(MANDIR)/man6 ]; then \
	  mkdir -p $(MANDIR)/man6; \
        fi
	cp -p fortune.6 $(MANDIR)/man6
	if [ ! -d $(MANDIR)/man8 ]; then \
	  mkdir -p $(MANDIR)/man8; \
        fi
	cp -p strfile.8 $(MANDIR)/man8

install install.data: all fortunes.dat
	cp fortunes.dat jargon.txt $(LIBDIR)/
	chown $(USER) $(LIBDIR)/fortunes.dat
	chmod 600 $(LIBDIR)/fortunes.dat
	chown $(USER) $(LIBDIR)/jargon.txt
	chmod 600 $(LIBDIR)/jargon.txt

install.obscenities: all obscenities.dat
	cp obscenities.dat $(LIBDIR)/fortunes.dat
	chown $(USER) $(LIBDIR)/fortunes.dat
	chmod 600 $(LIBDIR)/fortunes.dat

additional.obscenities: all obscenities.dat
	cp obscenities.dat $(LIBDIR)/obscenities.dat
	chown $(USER) $(LIBDIR)/obscenities.dat
	chmod 600 $(LIBDIR)/obscenities.dat

ar:
	rm -f $(ARF)
	ar cqv $(ARF) $(MISC) $(CFILES) $(SFILES) $(TFILES) $(FORTUNES)

tar:
	tar crvbf 20 $(TARF) $(ARF) $(MISC) $(CFILES) $(SFILES) $(TFILES) $(FORTUNES)

shar:
	./Rot13 obscene > obscene.r13
	-rm -f $(SHARF)* scene.?? obscene.r13.??
	shar $(SHARFLAGS) $(MISC) $(SFILES) $(TFILES) > $(SHARF).01
	shar $(SHARFLAGS) $(CFILES) > $(SHARF).02
	./Shar_split obscene.r13 $(SHARF) 3 "$(SHARFLAGS)" "sed -f :trfix"
	./Shar_split scene $(SHARF) 5 "$(SHARFLAGS)"

shar.clean:
	-rm -f $(SHARF)* scene.?? obscene.r13 obscene.r13.?? .obscene.com

spell:	scene.sp obscene.sp

Spell.hlist: scene.sp.ok obscene.sp.ok
	cat scene.sp.ok obscene.sp.ok | spellin /usr/dict/hlista > Spell.hlist

scene.sp: Spell.hlist scene
	spell -d Spell.hlist scene > scene.sp

obscene.sp: Spell.hlist obscene
	spell -d Spell.hlist obscene > obscene.sp

done_spell:
	sort -u -o scene.sp.ok scene.sp.ok scene.sp
	sort -u -o obscene.sp.ok obscene.sp.ok obscene.sp

troff: troff.scene troff.obscene

troff.scene:
	./Do_troff scene $(TROFF)

troff.obscene:
	./Do_troff obscene $(TROFF)

clean:
	rm -f fortune *.o *~ core* all jargon fortunes obscenities fortunes.dat obscenities.dat

tooclean: sort.clean shar.clean
	rm -f fortune fortunes obscenities fortunes.dat obscenities.dat fortunes.tar
	rm -f strfile unstr ? core *.o Spell.hlist *.sp

sort: sort.scene sort.obscene

sort.scene: strfile unstr
	strfile -oi scene
	mv scene Oscene
	unstr -o scene

sort.obscene: strfile unstr
	strfile -oi obscene
	mv obscene Oobscene
	unstr -o obscene

sort.clean:
	rm -f Oscene Oobscene scene.dat obscenities.dat
