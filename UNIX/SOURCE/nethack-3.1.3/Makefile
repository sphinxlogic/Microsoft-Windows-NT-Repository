#	NetHack Makefile.
#	SCCS Id: @(#)Makefile.top	3.1	92/01/05

# newer makes predefine $(MAKE) to 'make' and do smarter processing of
# recursive make calls if $(MAKE) is used
# these makes allow $(MAKE) to be overridden by the environment if someone
# wants to (or has to) use something other than the standard make, so we do
# not want to unconditionally set $(MAKE) here
#
# unfortunately, some older makes do not predefine $(MAKE); if you have one of
# these, uncomment the following line
# (you will know that you have one if you get complaints about unable to
# execute things like 'data' and 'rumors')
# MAKE = make

# make NetHack
GAME     = nethack
GAMEUID  = games
GAMEGRP  = bin

# Permissions - some places use setgid instead of setuid, for instance
# See also the option "SECURE" in include/config.h
GAMEPERM = 04755
FILEPERM = 0644
EXEPERM  = 0755
DIRPERM  = 0755

# GAMEDIR also appears in config.h as "HACKDIR".
#
# note that 'make install' believes in creating a nice tidy GAMEDIR for
# installation, free of debris from previous NetHack versions --
# therefore there should not be anything in GAMEDIR that you want to keep
# (if there is, you'll have to do the installation by hand or modify the
# instructions)
GAMEDIR  = /opt/games/lib/$(GAME)dir
SHELLDIR = /opt/games

DATHELP = help hh cmdhelp history opthelp wizhelp
VARDAT = data oracles options quest.dat rumors

SPEC_LEVS = asmodeus.lev baalz.lev bigroom.lev castle.lev fakewiz?.lev \
	juiblex.lev knox.lev medusa-?.lev mine_end.lev minefill.lev \
	minetown.lev oracle.lev orcus.lev sanctum.lev tower?.lev valley.lev \
	wizard?.lev astral.lev air.lev earth.lev fire.lev water.lev
QUEST_LEVS = ?-goal.lev ?-fill?.lev ?-locate.lev ?-start.lev

SCRIPT = dungeon
DAT = $(DATHELP) $(VARDAT) $(SPEC_LEVS) $(QUEST_LEVS) $(SCRIPT) license

$(GAME):
	( cd src ; $(MAKE) )

all:	$(GAME) Guidebook $(VARDAT) dungeon spec_levs
	@echo "Done."

Guidebook:
	( cd doc ; $(MAKE) Guidebook )

manpages:
	( cd doc ; $(MAKE) manpages )

data:
	( cd dat ; $(MAKE) data )

rumors:
	( cd dat ; $(MAKE) rumors )

oracles:
	( cd dat ; $(MAKE) oracles )

#	Note: options should have already been made with make, but...
options:
	( cd dat ; $(MAKE) options )

quest.dat:
	( cd dat ; $(MAKE) quest.dat )

spec_levs:
	( cd util ; $(MAKE) lev_comp )
	( cd dat ; $(MAKE) spec_levs )
	( cd dat ; $(MAKE) quest_levs )
	
dungeon:
	( cd util ; $(MAKE) dgn_comp )
	( cd dat ; $(MAKE) dungeon )
	
update: $(GAME) $(VARDAT) dungeon spec_levs
#	(don't yank the old version out from under people who're playing it)
	-mv $(GAMEDIR)/$(GAME) $(GAMEDIR)/$(GAME).old
#	quest.dat is also kept open and has the same problems over NFS
	-mv $(GAMEDIR)/quest.dat $(GAMEDIR)/quest.dat.old
# copy over new versions of the game files
	( cd dat ; cp $(DAT) $(GAMEDIR) )
	cp src/$(GAME) $(GAMEDIR)
	-rm -f $(SHELLDIR)/$(GAME)
	sed -e 's;/usr/games/lib/nethackdir;$(GAMEDIR);' \
		-e 's;HACKDIR/nethack;HACKDIR/$(GAME);' \
		< sys/unix/nethack.sh \
		> $(SHELLDIR)/$(GAME)
# correct the permissions on the new versions
	-( cd $(GAMEDIR) ; chown $(GAMEUID) $(DAT) $(GAME) ; \
			chgrp $(GAMEGRP) $(DAT) $(GAME) ; \
			chmod $(FILEPERM) $(DAT) )
	-chown $(GAMEUID) $(SHELLDIR)/$(GAME)
	chgrp $(GAMEGRP) $(SHELLDIR)/$(GAME)
	chmod $(EXEPERM) $(SHELLDIR)/$(GAME)
	chmod $(GAMEPERM) $(GAMEDIR)/$(GAME)
# touch time-sensitive files
	-touch -c $(GAMEDIR)/bones* $(GAMEDIR)/?lock* $(GAMEDIR)/wizard*
	-touch -c $(GAMEDIR)/save/*
	touch $(GAMEDIR)/perm $(GAMEDIR)/record
# and a reminder
	@echo You may also want to install the man pages via the doc Makefile.

install: $(GAME) $(VARDAT) dungeon spec_levs
# set up the directories
	-mkdir $(SHELLDIR)
	-rm -rf $(GAMEDIR)
	-mkdir $(GAMEDIR) $(GAMEDIR)/save
# create some files
	touch $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
# copy over the game files
	( cd dat ; cp $(DAT) $(GAMEDIR) )
	cp src/$(GAME) $(GAMEDIR)
	-rm -f $(SHELLDIR)/$(GAME)
	sed -e 's;/usr/games/lib/nethackdir;$(GAMEDIR);' \
		-e 's;HACKDIR/nethack;HACKDIR/$(GAME);' \
		< sys/unix/nethack.sh \
		> $(SHELLDIR)/$(GAME)
# set up the permissions
	-( cd $(GAMEDIR) ; chown $(GAMEUID) . * $(SHELLDIR)/$(GAME) )
	( cd $(GAMEDIR) ; chgrp $(GAMEGRP) . * $(SHELLDIR)/$(GAME) )
	chmod $(EXEPERM) $(SHELLDIR)/$(GAME)
	chmod $(FILEPERM) $(GAMEDIR)/*
	chmod $(DIRPERM) $(GAMEDIR) $(GAMEDIR)/save
	chmod $(GAMEPERM) $(GAMEDIR)/$(GAME)
# and a reminder
	@echo You may also want to reinstall the man pages via the doc Makefile.

# 'make clean' removes all the .o files, but leaves around all the executables
# and compiled data files
clean:
	( cd src ; $(MAKE) clean )
	( cd util ; $(MAKE) clean )

# 'make spotless' returns the source tree to near-distribution condition.
# it removes .o files, executables, and compiled data files
spotless:
	( cd src ; $(MAKE) spotless )
	( cd util ; $(MAKE) spotless )
	( cd dat ; $(MAKE) spotless )
	( cd doc ; $(MAKE) spotless )
