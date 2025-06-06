#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.src	3.1	93/01/25

# newer makes predefine $(MAKE) to 'make' and do smarter processing of
# recursive make calls if $(MAKE) is used
# these makes allow $(MAKE) to be overridden by the environment if someone
# wants to (or has to) use something other than the standard make, so we do
# not want to unconditionally set $(MAKE) here
#
# unfortunately, some older makes do not predefine $(MAKE); if you have one of
# these, uncomment the following line
# (you will know that you have one if you get complaints about being unable
# to find 'makedefs')
# MAKE = make

# This makefile replaces the previous Makefile.unix, Makefile.xenix,
# Makefile.3B2, Makefile.att, and Makefile.tos.
# Set SYSTEM to one of:
#	'Sysunix'	-- generic UNIX
#	'Sys3B2'	-- AT&T 3B2, 3B5, etc.
#	'Sysatt'	-- AT&T UNIXPC, 7300, 3B1
#	'SysV-AT'	-- Microport 286 UNIX (put -DDUMB in CFLAGS)
#	'Systos'	-- Atari
SYSTEM = Sysunix

#
# Make sure that your bourne shell is specified here, as you have to spawn
# some of the commands (eg. depend) in bourne shell for them to work.
#
# For Systos users compiling on the ST, you'll either need a bourne shell
# clone or you'll need to do make depend, etc. by hand. In either case,
# the line below probably needs changing
SHELL=/bin/sh

# Pick the SYSSRC and SYSOBJ lines corresponding to your desired operating
# system.
#
# for UNIX systems
SYSSRC = ../sys/unix/ioctl.c ../sys/unix/unixmain.c ../sys/unix/unixtty.c \
	../sys/unix/unixunix.c
SYSOBJ = ioctl.o unixmain.o unixtty.o unixunix.o
#
# for Systos
# SYSSRC = ../sys/atari/tos.c ../sys/share/pcmain.c ../sys/share/pcsys.c \
#	../sys/share/pctty.c ../sys/share/pcunix.c
# SYSOBJ = tos.o pcmain.o pcsys.o pctty.o pcunix.o


# if you are using gcc as your compiler:
#	uncomment the CC definition below if it's not in your environment
#	if you get setcgtty() warnings during execution, you are feeding gcc
#		a non-ANSI <sys/ioctl.h> -- either run fixincludes on it or use
#		-traditional in CFLAGS
# CC = gcc
#
#	For Bull DPX/2 systems at B.O.S. 2.0 or higher use the following:
#
# CC = gcc -ansi -D_BULL_SOURCE -D_XOPEN_SOURCE -D_POSIX_SOURCE
# 
#	If you are using GCC 2.2.2 or higher on a DPX/2, just use:
#
# CC = gcc -ansi
#
# if you're debugging and want gcc to check as much as possible, use:
# CC = gcc -W -Wimplicit -Wreturn-type -Wunused -Wformat -Wswitch -Wshadow -Wcast-qual -Wwrite-strings -DGCC_WARN

# flags may have to be changed as required
# flags for 286 Xenix:
# CFLAGS = -Ml2t16 -O -LARGE -I../include
# LFLAGS = -Ml -F 4000 -SEG 512

# flags for 286 Microport SysV-AT
# CFLAGS = -DDUMB -Ml -I../include
# LFLAGS = -Ml

# flags for Atari gcc
# CFLAGS = -O -I../include
# LFLAGS = -s

# flags for AIX 3.1 cc on IBM RS/6000 to define
# a suitable subset of standard libraries
# (note that there is more info regarding the "-qchars=signed"
# switch in file Install.unx note 8)
# CFLAGS = -D_NO_PROTO -D_XOPEN_SOURCE -O -I../include -qchars=signed

# flags for A/UX 2.01 using native cc or c89
# gcc predefines AUX so that's not needed there
# Remember to use -lcurses for WINLIB below !
# CFLAGS = -ZS -D_POSIX_SOURCE -O -I../include -DAUX

# flags for debugging:
# CFLAGS = -g -I../include

CFLAGS = -g -I../include
LFLAGS =


# Set the WINSRC, WINOBJ, and WINLIB lines to correspond to your desired
# combination of windowing systems.  Also set windowing systems in config.h.
#
# files for a straight tty port using no native windowing system
WINTTYSRC = ../win/tty/getline.c ../win/tty/termcap.c ../win/tty/topl.c \
	../win/tty/wintty.c
WINTTYOBJ = getline.o termcap.o topl.o wintty.o
#
# files for an X11 port
WINX11SRC = ../win/X11/Window.c ../win/X11/dialogs.c ../win/X11/winX.c \
	../win/X11/winmap.c  ../win/X11/winmenu.c ../win/X11/winmesg.c \
	../win/X11/winmisc.c ../win/X11/winstat.c ../win/X11/wintext.c \
	../win/X11/winval.c
WINX11OBJ = Window.o dialogs.o winX.o winmap.o winmenu.o winmesg.o \
	winmisc.o winstat.o wintext.o winval.o
#
# 
WINSRC = $(WINTTYSRC)
WINOBJ = $(WINTTYOBJ) $(WINX11OBJ)

# on some systems the termcap library is in -ltermcap or -lcurses
# on 386 Xenix, the -ltermlib tputs() seems not to work; use -lcurses instead
# Sysatt uses shared library in lieu of this option
# Systos needs -lcurses16 if you use -mshort
# AIX 3.1 on RS/6000 likes -lcurses if TERMINFO defined in config.h
# and -ltermcap otherwise
# Linux uses -ltermcap
#
# libraries for tty ports
# WINTTYLIB = -ltermcap
# WINTTYLIB = -lcurses
# WINTTYLIB = -lcurses16
WINTTYLIB = -ltermlib
#
# libraries for X11
WINX11LIB = -lXaw -lXmu -lXext -lXt -lX11
# WINX11LIB = -lXaw -lXmu -lXt -lX11
#
#
WINLIB = $(WINTTYLIB) $(WINX11LIB)

# any other strange libraries your system needs (for Sysunix only -- the more
# specialized targets should already be right)
#
# on HP-UX, the malloc(3x) routines in libmalloc.a seem to align things
# better than the malloc(3) ones in libc.a
# LIBS = -lmalloc
#
# DPX/2's also use the malloc(3x) routines.  In addition, if you are building
# for X11, you must include libinet.a.
# LIBS = -lmalloc -linet
#
# Linux NetHack uses some bsd style ioctl functions, thus it is necessary to
# use the bsd libs.
# LIBS = -lbsd
LIBS = -lsocket -lnsl

# make NetHack
GAME     = nethack

# if you defined RANDOM in unixconf.h/tosconf.h since your system did not come
# with a reasonable random number generator
# RANDOBJ = random.o
RANDOBJ =



# ----------------------------------------
#
# Nothing below this line should have to be changed.
#
# Other things that have to be reconfigured are in config.h,
# {unixconf.h, pcconf.h, tosconf.h}, and possibly system.h

MAKEDEFS = ../util/makedefs

# all .c that are part of the main NetHack program and are not operating- or
# windowing-system specific
HACKCSRC = allmain.c alloc.c apply.c artifact.c attrib.c ball.c bones.c\
	   botl.c cmd.c dbridge.c decl.c detect.c display.c do.c do_name.c\
	   do_wear.c dog.c dogmove.c dokick.c dothrow.c drawing.c dungeon.c\
	   eat.c end.c engrave.c exper.c explode.c extralev.c files.c\
	   fountain.c hack.c hacklib.c invent.c lock.c mail.c makemon.c\
	   mcastu.c mhitm.c mhitu.c minion.c mklev.c mkmap.c mkmaze.c\
	   mkobj.c mkroom.c mon.c mondata.c monmove.c monst.c mplayer.c\
	   mthrowu.c muse.c music.c o_init.c objects.c objnam.c options.c\
	   pager.c pickup.c pline.c polyself.c potion.c pray.c priest.c\
	   quest.c questpgr.c read.c rect.c restore.c rip.c rnd.c rumors.c\
	   save.c shk.c shknam.c sit.c sounds.c sp_lev.c spell.c steal.c\
	   timeout.c topten.c track.c trap.c u_init.c uhitm.c vault.c\
	   version.c vision.c weapon.c were.c wield.c windows.c wizard.c\
	   worm.c worn.c write.c zap.c

# all operating-system-dependent .c (for dependencies and such)
SYSCSRC = ../sys/atari/tos.c ../sys/share/pcmain.c ../sys/share/pcsys.c \
	../sys/share/pctty.c ../sys/share/pcunix.c ../sys/share/random.c \
	../sys/unix/ioctl.c ../sys/unix/unixmain.c ../sys/unix/unixtty.c \
	../sys/unix/unixunix.c

# all windowing-system-dependent .c (for dependencies and such)
WINCSRC = $(WINTTYSRC) $(WINX11SRC)

# .c files for this version (for date.h)
VERSOURCES = $(HACKCSRC) $(SYSSRC) $(WINSRC) monstr.c vis_tab.c

# .c files for all UNIX versions (for lint and tags)
CSOURCES = $(HACKCSRC) $(SYSSRC) $(WINCSRC) monstr.c vis_tab.c


# all .h files except date.h, onames.h, pm.h, and vis_tab.h which would
# cause dependency loops if run through "make depend"
# and dgn_comp.h, dgn_file.h, lev_comp.h, special level & dungeon files.
#
HACKINCL = align.h amiconf.h artifact.h artilist.h attrib.h color.h config.h\
	   coord.h decl.h def_os2.h display.h dungeon.h edog.h emin.h engrave.h\
	   epri.h eshk.h extern.h flag.h func_tab.h global.h hack.h lev.h\
	   macconf.h mfndpos.h micro.h mkroom.h monattk.h mondata.h monflag.h\
	   monst.h monsym.h obj.h objclass.h os2conf.h patchlevel.h pcconf.h\
	   permonst.h prop.h rect.h rm.h sp_lev.h spell.h system.h termcap.h\
	   tosconf.h tradstdc.h trampoli.h trap.h unixconf.h vault.h vision.h\
	   vmsconf.h wintty.h winX.h winprocs.h wintype.h you.h youprop.h

HSOURCES = $(HACKINCL) date.h onames.h pm.h vis_tab.h\
		lev_comp.h dgn_comp.h dgn_file.h

# the following .o's _must_ be made before any others (for makedefs)
FIRSTOBJ = monst.o objects.o

HOBJ = $(FIRSTOBJ) allmain.o alloc.o apply.o artifact.o attrib.o ball.o\
	bones.o botl.o cmd.o dbridge.o decl.o detect.o display.o do.o\
	do_name.o do_wear.o dog.o dogmove.o dokick.o dothrow.o drawing.o\
	dungeon.o eat.o end.o engrave.o exper.o explode.o extralev.o\
	files.o fountain.o hack.o hacklib.o invent.o lock.o mail.o\
	makemon.o mcastu.o mhitm.o mhitu.o minion.o mklev.o mkmap.o\
	mkmaze.o mkobj.o mkroom.o mon.o mondata.o monmove.o monstr.o\
	mplayer.o mthrowu.o muse.o music.o o_init.o objnam.o options.o\
	pager.o pickup.o pline.o polyself.o potion.o pray.o priest.o\
	quest.o questpgr.o read.o rect.o restore.o rip.o rnd.o rumors.o\
	save.o shk.o shknam.o sit.o sounds.o sp_lev.o spell.o steal.o\
	timeout.o topten.o track.o trap.o u_init.o uhitm.o vault.o\
	vision.o vis_tab.o weapon.o were.o wield.o windows.o wizard.o\
	worm.o worn.o write.o zap.o\
	$(RANDOBJ) $(SYSOBJ) $(WINOBJ) version.o
# the .o files from the HACKCSRC, SYSSRC, and WINSRC lists

$(GAME):	$(SYSTEM)
	@echo "$(GAME) is up to date."

Sysunix:	$(HOBJ) Makefile
	@echo "Loading ..."
	$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB) $(LIBS)
	@touch Sysunix

Sys3B2:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB) -lmalloc
	@touch Sys3B2

Sysatt:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LD) $(LFLAGS) /lib/crt0s.o /lib/shlib.ifile -o $(GAME) $(HOBJ)
	@touch Sysatt

Systos:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB)
	@touch Systos

SysV-AT:	DUMB.Setup $(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(WINLIB)
	@touch SysV-AT

DUMB.Setup:	../include/extern.h
	cp ../include/extern.h ../include/extern.h.BAK 
	cat ../include/extern.h | \
		sed -e '/^E\ int\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e '/[(][*]occupation[)]/b' \
			-e '/[(][*]afternmv[)]/b' \
			-e '/float_down/b' \
			-e '/done1/b' \
			-e '/identify/b' \
			-e '/Hear_again/b' \
			-e '/hangup/b' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' | \
		sed -e '/^E\ void\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' \
				>../include/extern.DUMB 
	cp ../include/extern.DUMB ../include/extern.h 
	@touch DUMB.Setup

all:	$(GAME)


#	dependencies for makedefs and its outputs, which the util
#	Makefile is responsible for keeping up to date
#

# special rules, to force update of makedefs, real dependencies should be
# below in the 'make depend' output.
monst.o:
	$(CC) $(CFLAGS) -c monst.c
	@rm -f $(MAKEDEFS)

objects.o:
	$(CC) $(CFLAGS) -c objects.c
	@rm -f $(MAKEDEFS)

# need those include files not in monst.c/objects.c dependencies below
$(MAKEDEFS): ../util/makedefs.c  ../include/artilist.h ../include/patchlevel.h \
		../include/qtext.h
	@( cd ../util ; $(MAKE) makedefs)

../include/onames.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/onames.h )
../include/pm.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/pm.h )
monstr.c: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../src/monstr.c )
../include/vis_tab.h: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../include/vis_tab.h )
vis_tab.c: $(MAKEDEFS)
	@( cd ../util ; $(MAKE) ../src/vis_tab.c )

#	date.h should be remade any time any of the source or include code
#	is modified.  Unfortunately, this would make the contents of this
#	file far more complex.  Since "hack.h" depends on most of the include
#	files, we kludge around this by making date.h dependent on hack.h,
#	even though it doesn't include this file.
#
#	hack.h depends on makedefs' output, so we know makedefs will be
#	up to date before being executed
../include/date.h:	$(VERSOURCES) ../include/hack.h
	( cd ../util ; $(MAKE) ../include/date.h )


lint:
# lint cannot have -p here because (i) capitals are meaningful:
# [Ww]izard, (ii) identifiers may coincide in the first six places:
# doweararm() versus dowearring().
# _flsbuf comes from <stdio.h>, a bug in the system libraries.
	@echo lint -axbh -DLINT ...
	@lint -axbh -I../include -DLINT $(CSOURCES) | sed '/_flsbuf/d'


tags: $(CSOURCES)
	@echo ctags -tw ...
	@ctags -tw $(CSOURCES)
	@( cd ../include ; ctags -tw $(HSOURCES) )
	@( cd ../util ; $(MAKE) tags )

clean:
	rm -f *.o

spotless: clean
	rm -f a.out core $(GAME) Sys*
	rm -f ../include/date.h ../include/onames.h ../include/pm.h
	rm -f monstr.c ../include/vis_tab.h vis_tab.c


depend:
# For the moment we are lazy and disregard /usr/include files because
# the sources contain them conditionally. Perhaps we should use cpp.
#		( /bin/grep '^#[ 	]*include' $$i | sed -n \
#			-e 's,<\(.*\)>,"/usr/include/\1",' \
#
# extern.h is ignored, even though its declared function types may affect the
# compilation of all the .c files, since extern.h changes every time the
# type of an external function does, and we would spend all our time recompiling
# if we did not ignore it.  the risk is minimal, as lint should pick up any
# calls to the modified function that were not modified accordingly.
#
# patchlev.h is the name MS-DOS uses to refer to patchlevel.h after it gets
# through truncating the name, but it does not really exist
#
# theory of operation:
#	for each file
#		for each include line
#			pick out filename from within ""
#			delete unwanted names
#			prepend ../include/
#			squirrel away in hold space
#		at end of file
#			pick up hold space
#			change newlines to spaces
#			prepend file:
#			change .c to .o
#			add command if necessary
#
	for i in ${SYSCSRC} ${WINCSRC}; do \
		( echo '#include ""' | cat - $$i | \
		/bin/grep '^#[ 	]*include[ 	]*"' | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/\1/' \
			-e 's/patchlev.h//' \
			-e 's/.*\.h/..\/include\/&/' \
			-e H \
			-e '$$g' -e '$$s/\n/ /g' \
			-e '$$s;.*;'$$i': '$$i'&;' \
			-e '$$s;\.\./[^/]*/[^/]*/\([^.]*\)\.c;\1.o;' \
			-e '$$s;.*;&\
		$$(CC) $$(CFLAGS) -c '$$i';p' \
			>> makedep); done
	for i in ${HACKCSRC}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' $$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/\1/' \
			-e 's/patchlev.h//' \
			-e 's/.*\.h/..\/include\/&/' \
			-e H \
			-e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/'$$i': &/' \
			-e '$$s/\.c:/.o:/p' \
			>> makedep); done
	for i in ${HACKINCL}; do \
	    ( /bin/grep '^#[ 	]*include[ 	]*"' ../include/$$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/\1/' \
			-e 's/extern.h//' \
			-e 's/.*\.h/..\/include\/&/' \
			-e H \
			-e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/..\/include\/'$$i': &\
		touch ..\/include\/'$$i/p \
			>> makedep); done
	@echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@cp Makefile Makefile.bak
	ed - Makefile < eddep
	@rm -f eddep makedep
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@echo '# see make depend above' >> Makefile
	- diff Makefile.bak Makefile
	@rm -f Makefile.bak

# DO NOT DELETE THIS LINE

tos.o: ../sys/atari/tos.c  ../include/hack.h ../include/termcap.h
	$(CC) $(CFLAGS) -c ../sys/atari/tos.c
pcmain.o: ../sys/share/pcmain.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/share/pcmain.c
pcsys.o: ../sys/share/pcsys.c  ../include/hack.h ../include/wintty.h
	$(CC) $(CFLAGS) -c ../sys/share/pcsys.c
pctty.o: ../sys/share/pctty.c  ../include/hack.h ../include/wintty.h
	$(CC) $(CFLAGS) -c ../sys/share/pctty.c
pcunix.o: ../sys/share/pcunix.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/share/pcunix.c
random.o: ../sys/share/random.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/share/random.c
ioctl.o: ../sys/unix/ioctl.c  ../include/hack.h ../include/termcap.h
	$(CC) $(CFLAGS) -c ../sys/unix/ioctl.c
unixmain.o: ../sys/unix/unixmain.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/unix/unixmain.c
unixtty.o: ../sys/unix/unixtty.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/unix/unixtty.c
unixunix.o: ../sys/unix/unixunix.c  ../include/hack.h
	$(CC) $(CFLAGS) -c ../sys/unix/unixunix.c
getline.o: ../win/tty/getline.c  ../include/hack.h ../include/wintty.h ../include/func_tab.h
	$(CC) $(CFLAGS) -c ../win/tty/getline.c
termcap.o: ../win/tty/termcap.c  ../include/hack.h ../include/wintty.h ../include/termcap.h
	$(CC) $(CFLAGS) -c ../win/tty/termcap.c
topl.o: ../win/tty/topl.c  ../include/hack.h ../include/termcap.h ../include/wintty.h
	$(CC) $(CFLAGS) -c ../win/tty/topl.c
wintty.o: ../win/tty/wintty.c  ../include/hack.h ../include/termcap.h ../include/wintty.h
	$(CC) $(CFLAGS) -c ../win/tty/wintty.c
Window.o: ../win/X11/Window.c  ../include/WindowP.h ../include/config.h
	$(CC) $(CFLAGS) -c ../win/X11/Window.c
dialogs.o: ../win/X11/dialogs.c  ../include/config.h
	$(CC) $(CFLAGS) -c ../win/X11/dialogs.c
winX.o: ../win/X11/winX.c  ../include/hack.h ../include/winX.h \
		../win/X11/nh72icon ../win/X11/nh56icon ../win/X11/nh32icon
	$(CC) $(CFLAGS) -c ../win/X11/winX.c
winmap.o: ../win/X11/winmap.c  ../include/Window.h ../include/hack.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmap.c
winmenu.o: ../win/X11/winmenu.c  ../include/hack.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmenu.c
winmesg.o: ../win/X11/winmesg.c  ../include/Window.h ../include/hack.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmesg.c
winmisc.o: ../win/X11/winmisc.c  ../include/hack.h ../include/func_tab.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winmisc.c
winstat.o: ../win/X11/winstat.c  ../include/hack.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/winstat.c
wintext.o: ../win/X11/wintext.c  ../include/hack.h ../include/winX.h
	$(CC) $(CFLAGS) -c ../win/X11/wintext.c
winval.o: ../win/X11/winval.c  ../include/config.h
	$(CC) $(CFLAGS) -c ../win/X11/winval.c
allmain.o:  ../include/hack.h
alloc.o:  ../include/config.h
apply.o:  ../include/hack.h ../include/edog.h
artifact.o:  ../include/hack.h ../include/artifact.h ../include/artilist.h
attrib.o:  ../include/hack.h ../include/artifact.h
ball.o:  ../include/hack.h
bones.o:  ../include/hack.h ../include/lev.h
botl.o:  ../include/hack.h
cmd.o:  ../include/hack.h ../include/func_tab.h
dbridge.o:  ../include/hack.h
decl.o:  ../include/hack.h ../include/quest.h
detect.o:  ../include/hack.h ../include/artifact.h
display.o:  ../include/hack.h
do.o:  ../include/hack.h ../include/lev.h
do_name.o:  ../include/hack.h
do_wear.o:  ../include/hack.h
dog.o:  ../include/hack.h ../include/edog.h
dogmove.o:  ../include/hack.h ../include/mfndpos.h ../include/edog.h
dokick.o:  ../include/hack.h ../include/eshk.h
dothrow.o:  ../include/hack.h
drawing.o:  ../include/hack.h ../include/termcap.h
dungeon.o:  ../include/hack.h ../include/dgn_file.h
eat.o:  ../include/hack.h
end.o:  ../include/hack.h ../include/eshk.h
engrave.o:  ../include/hack.h ../include/lev.h
exper.o:  ../include/hack.h
explode.o:  ../include/hack.h
extralev.o:  ../include/hack.h
files.o:  ../include/hack.h
fountain.o:  ../include/hack.h
hack.o:  ../include/hack.h
hacklib.o:  ../include/hack.h
invent.o:  ../include/hack.h ../include/artifact.h
lock.o:  ../include/hack.h
mail.o:  ../include/hack.h ../include/mail.h
makemon.o:  ../include/hack.h ../include/epri.h ../include/emin.h
mcastu.o:  ../include/hack.h
mhitm.o:  ../include/hack.h ../include/artifact.h ../include/edog.h
mhitu.o:  ../include/hack.h ../include/artifact.h ../include/edog.h
minion.o:  ../include/hack.h ../include/emin.h ../include/epri.h
mklev.o:  ../include/hack.h
mkmap.o:  ../include/hack.h ../include/sp_lev.h
mkmaze.o:  ../include/hack.h ../include/sp_lev.h
mkobj.o:  ../include/hack.h ../include/artifact.h ../include/prop.h
mkroom.o:  ../include/hack.h
mon.o:  ../include/hack.h ../include/mfndpos.h ../include/edog.h
mondata.o:  ../include/hack.h ../include/eshk.h ../include/epri.h
monmove.o:  ../include/hack.h ../include/mfndpos.h ../include/artifact.h
monst.o:  ../include/config.h ../include/permonst.h ../include/monsym.h ../include/eshk.h \
		../include/vault.h ../include/epri.h ../include/color.h
mplayer.o:  ../include/hack.h
mthrowu.o:  ../include/hack.h
muse.o:  ../include/hack.h
music.o:  ../include/hack.h #interp.c
o_init.o:  ../include/hack.h
objects.o:  ../include/config.h ../include/obj.h ../include/objclass.h ../include/prop.h ../include/color.h objects.c
objnam.o:  ../include/hack.h
options.o:  ../include/config.h ../include/objclass.h ../include/flag.h ../include/hack.h ../include/termcap.h
pager.o:  ../include/hack.h
pickup.o:  ../include/hack.h
pline.o:  ../include/hack.h ../include/epri.h
polyself.o:  ../include/hack.h
potion.o:  ../include/hack.h
pray.o:  ../include/hack.h ../include/epri.h
priest.o:  ../include/hack.h ../include/mfndpos.h ../include/eshk.h ../include/epri.h ../include/emin.h
quest.o:  ../include/hack.h ../include/quest.h ../include/qtext.h
questpgr.o:  ../include/hack.h ../include/qtext.h
read.o:  ../include/hack.h
rect.o:  ../include/hack.h
restore.o:  ../include/hack.h ../include/lev.h ../include/termcap.h ../include/quest.h
rip.o:  ../include/hack.h
rnd.o:  ../include/hack.h
rumors.o:  ../include/hack.h
save.o:  ../include/hack.h ../include/lev.h ../include/quest.h
shk.o:  ../include/hack.h ../include/eshk.h
shknam.o:  ../include/hack.h ../include/eshk.h
sit.o:  ../include/hack.h ../include/artifact.h
sounds.o:  ../include/hack.h ../include/edog.h
sp_lev.o:  ../include/hack.h ../include/sp_lev.h ../include/rect.h
spell.o:  ../include/hack.h
steal.o:  ../include/hack.h
timeout.o:  ../include/hack.h
topten.o:  ../include/hack.h
track.o:  ../include/hack.h
trap.o:  ../include/hack.h
u_init.o:  ../include/hack.h
uhitm.o:  ../include/hack.h
vault.o:  ../include/hack.h ../include/vault.h
version.o:  ../include/hack.h ../include/date.h  ../include/patchlevel.h
vision.o:  ../include/hack.h ../include/vis_tab.h
weapon.o:  ../include/hack.h
were.o:  ../include/hack.h
wield.o:  ../include/hack.h
windows.o:  ../include/hack.h ../include/wintty.h
wizard.o:  ../include/hack.h ../include/qtext.h
worm.o:  ../include/hack.h ../include/lev.h
worn.o:  ../include/hack.h
write.o:  ../include/hack.h
zap.o:  ../include/hack.h
../include/amiconf.h:  ../include/micro.h ../include/pcconf.h
	touch ../include/amiconf.h
../include/config.h:  ../include/tradstdc.h ../include/global.h
	touch ../include/config.h
../include/decl.h:  ../include/quest.h ../include/spell.h ../include/color.h ../include/obj.h \
		../include/you.h ../include/onames.h ../include/pm.h
	touch ../include/decl.h
../include/display.h:  ../include/vision.h ../include/mondata.h
	touch ../include/display.h
../include/dungeon.h:  ../include/align.h
	touch ../include/dungeon.h
../include/emin.h:  ../include/dungeon.h
	touch ../include/emin.h
../include/epri.h:  ../include/dungeon.h ../include/align.h
	touch ../include/epri.h
../include/eshk.h:  ../include/dungeon.h
	touch ../include/eshk.h
../include/global.h:  ../include/coord.h ../include/vmsconf.h ../include/unixconf.h \
		../include/os2conf.h ../include/pcconf.h ../include/tosconf.h \
		../include/amiconf.h ../include/macconf.h ../include/ntconf.h
	touch ../include/global.h
../include/hack.h:  ../include/config.h ../include/dungeon.h ../include/monsym.h \
		../include/mkroom.h ../include/objclass.h ../include/decl.h \
		../include/trap.h ../include/flag.h ../include/rm.h ../include/vision.h \
		../include/display.h ../include/wintype.h ../include/engrave.h \
		../include/rect.h  ../include/winprocs.h ../include/wintty.h ../include/trampoli.h
	touch ../include/hack.h
../include/macconf.h:  ../include/system.h
	touch ../include/macconf.h
../include/mondata.h:  ../include/align.h
	touch ../include/mondata.h
../include/monst.h:  ../include/align.h
	touch ../include/monst.h
../include/os2conf.h:  ../include/micro.h ../include/system.h
	touch ../include/os2conf.h
../include/pcconf.h:  ../include/micro.h ../include/system.h
	touch ../include/pcconf.h
../include/permonst.h:  ../include/monattk.h ../include/monflag.h ../include/align.h
	touch ../include/permonst.h
../include/rm.h:  ../include/align.h
	touch ../include/rm.h
../include/sp_lev.h:  ../include/align.h
	touch ../include/sp_lev.h
../include/tosconf.h:  ../include/micro.h ../include/pcconf.h
	touch ../include/tosconf.h
../include/unixconf.h:  ../include/system.h
	touch ../include/unixconf.h
../include/vault.h:  ../include/dungeon.h
	touch ../include/vault.h
../include/vmsconf.h:  ../include/system.h
	touch ../include/vmsconf.h
../include/you.h:  ../include/align.h ../include/attrib.h ../include/monst.h ../include/youprop.h
	touch ../include/you.h
../include/youprop.h:  ../include/prop.h ../include/permonst.h ../include/mondata.h ../include/pm.h
	touch ../include/youprop.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
