# Makefile for "expect" by Don Libes, NIST
VERSION = \"3.24.1\"
# Requires following version of Tcl or later
TCL = 6.4

# The following group of lines isolate things you are likely to want to change

# Tcl include files.  (If you haven't installed Tcl yet, read the README file).
TCLHDIR = /opt/lib/tcl$(TCL)/include
# Tcl library
TCLLIB = /opt/lib/libtcl.a
# If your Tcl is a Sun (maybe others) shared library, uncomment the following
#TCLLIB = -Ltclpath -ltcl
# note that tclpath must be on the LD_LIBRARY_PATH

# Where to put man pages (for commands)
MAN1DIR = /opt/man/cat1
# Extension to put on man pages (for commands)
MAN1EXT = 1
# Where to put man pages (for libraries)
MAN3DIR = /opt/man/cat3
# Extension to put on man pages (for libraries)
MAN3EXT = 3

# Where to put final expect binary
BINDIR = /opt/bin

# Where to put expect library archive
LIBDIR = /opt/lib/expect


# Where to put expect include files
HDIR = /opt/lib/expect

# Where to put expect scripts that are used as UNIX commands
# (This can be the same as BINDIR, except for mixed-architecture sites that
#  share things like architecture-independent programs like expect scripts.)
ARCH_INDEP_BINDIR = /opt/lib/expect/scripts

# Directory where expect binary can be found (NOT necessarily where it is PUT).
# (This can be the same as BINDIR, except when BINDIR is so literally so long
#  that the #! mechanism breaks.  Then the solution is to create a directory
#  with a very short name, which consists only of symbolic links back to the
#  true binaries.)
SHORT_BINDIR = /opt/bin

# Where to put expect scripts that are commonly sourced by other expect scripts
SCRIPTDIR = /opt/lib/expect/scripts

# You might like to install some of the example scripts that come with expect.
# Edit out the ones you don't want installed.  The INSTALL file describes
# these in more detail.
SCRIPTS = rogue.exp chess.exp time.exp timed_read ftp-rfc \
	passwd.exp archie robohunt lpunlock weather passmass rftp \
	kibitz rlogin.exp
# A couple (well, one for now) of the scripts have man pages of their own.
# You can delete these, too, if you don't want'em.
SCRIPTS_MANPAGES = kibitz

# If your Make doesn't know about $(MAKE), uncomment the following
#MAKE = make

# If want to set to ownership, modes, etc, to someone else upon installation,
# set the defn of CP to install or something similar.
CP = cp
#CP = install -o libes
# As above, but for ownership, modes, etc. of new directories.
MKDIR = mkdir
#MKDIR = install -d -o libes
#
# Which C compiler to use.  Yes, you can compile expect with gcc.
CC = gcc
#CC = cc
#
# flags to pass to both cc and ld
# -O for production version
# -g for debuggable version
CLFLAGS = -g  -O3
#
# choose one of:
#	bsd - BSD
#	usg - USG (i.e., SIII through SVR3), SCO, HP
#	unicos - Cray Unicos
#	sgi3 - Silicon Graphics, 3 or earlier
#	sgi - Silicon Graphics (using _getpty)
#	svr4 - SVR4
#	aix3 - recent versions of AIX (3.1.5 and later)
PTY_TYPE = svr4
#
# if you don't have ranlib, comment out the "RANLIB = ranlib" line and use
# the "RANLIB = echo" below.  If you don't know if you have ranlib, type
# "ranlib" at the shell and see if it finds the program or not.
# RANLIB = ranlib
RANLIB = echo
#
# If you are on a cray, comment out the "SETUID = echo" line and use the
# "SETUID = chmod s+o" below.  expect must be setuid on crays in order to
# open ptys (and accordingly, you must run this Makefile as root).
# See the FAQ for more info on why this is necessary on Crays.
SETUID = echo
# SETUID = chmod u+s
#
# choose one of the following: select, poll, posix, or simple
INTERACT_TYPE = select
# If your system (Sun, recent HP, Ultrix, etc) has select(2) use select
# Note that many SV machines support select.  Starting with SVR4, all will.
# If your machine supports poll(2) but not select, use poll.
# See the beginning of that file (inter_poll.c) for more info.
# If that doesn't work, use simple.  This does not support
# multiple processes, but you will still find the result useful.
# Note that some machines (e.g., 3b2 running SVR3) have poll, but do not
# support it for ttys.  In this # case, you must also use simple.
# posix is just a placeholder for future work.  Eventually, POSIX
# is supposed to provide a select/poll replacement, but they have not yet
# done so.  In the meantime, if you wish to cobble up something using
# the same idea as simple but supporting multiple processes AND
# 1003.1 compliant, send it to me.

# Define default parameters for ptys.  This is used when 1) running in the
# background, 2) user has not defined the variable STTY_INIT to initialize
# ptys, and 3) the pty-driver's defaults suck.
# Use whatever format your local stty program likes.  
STTY=\"sane\"

CPPFLAGS = -I. -I$(TCLHDIR) -DVERSION=$(VERSION) -DDFLT_STTY=$(STTY) \
		-DSCRIPTDIR=\"$(SCRIPTDIR)/\"

#
# Flags to pass to cc (i.e. add to the end of the CFLAGS line below).
# Note that setting one will not set others automatically.  Set all that
# are relevant.
# 
# -DPOSIX if you are have POSIX tty support.  (Currently only tested on AIX
#	3.x).  
# -DNOWAITPID if your system doesn't have waitpid.  Only really old systems
#	lack this.
# -DNO_PID_T if your system doesn't have a typedef for pid_t.  If you don't
#	know, try without defining it.  If it compiles, you're ok.
# -DNO_MEMCPY if your system does not have memcpy.  If you don't know, try
#	without defining it.  If it compiles, you're ok.
# -DNO_STRING_H if your system does not have /usr/include/string.h.  If you
#	don't know, try without defining it.  If it compiles, you're ok.
# -DSYSV3 if you are running SVR3 or later (includes recent HPs)
# -DSVSV4 if you are running SVR4.  This option does not preclude -DSYSV3.
# -DHPUX if you are running HP-UX.
# -DCRAY=51 or 60, 70, etc, if you are running Cray Unicos 5.1, 6.0, 7.0, etc.
# -DAUX2 if you are running Mac A/UX 2.
# -DUTS if you are on an Amdahl.
# -DMIPS_BSD if you are on a Mips machine using the BSD universe.
# -DSIG_FN_RETURN=int if you are running SunOS-3.X or similar systems that
#	define the function argument to signal as int (*func)() rather than
#	void (*func)().
# -DREARM_SIG if you are running systems (such as V7, SV, Unicos, HP) where
#	signal handlers need to be rearmed after use.
# -D_BSD_SIGNALS if you are on a Silicon Graphics AND want BSD semantics when
#	using the expect library.  Otherwise, you are better off just sticking
#	with defining REARM_SIG (above).
# -DEXTERN_ERRNO if your system needs "extern int errno" (e.g., MORE/bsd).
# -DTERM=whatever if your system is unhappy with the default terminal struct
#	name (termios for SV, and sgttyb for BSD).  Some SV systems (3b2 &
#	pre-8.0 HPUX) want "termio".
# -DNOSTDLIB if your system doesn't have /usr/include/stdlib.h.  If you don't
#	know, try without.  If you get bad declarations on malloc, then add.

# If your "make" automatically includes CPPFLAGS in CC, use
#CFLAGS = $(CLFLAGS)
# else use following two lines 
CFLAGS = $(CLFLAGS) $(CPPFLAGS) -DSYSV4 -DSYSV3
LFLAGS = $(CLFLAGS)

# Flags to pass to ld (i.e., add to the end of the LIBS line below).
#
# -lc -lBSD     If you are using the BSD compatibility library on an HP/UX,
#		force libc.a to be loaded first.
# -lsocket	For SCO UNIX 3.2.2
LIBS = $(TCLLIB) -lm -lsocket -lnsl

# undefine the following two lines for Make's which don't include CPPFLAGS (eg.
# Ultrix), or which include extra stuff (Sun's "-target" which annoys gcc)
#.c.o:
#	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

#
# END OF THINGS YOU PROBABLY WANT TO CHANGE
#

TCLLINTLIB = /depot/sundry/src/tcl$(TCL)/llib-ltcl.ln

INTERACT = inter_$(INTERACT_TYPE)
PTY = pty_$(PTY_TYPE)
CFILES = main.c command.c expect.c $(PTY).c $(INTERACT).c unit_random.c
OFILES = main.o command.o expect.o $(PTY).o $(INTERACT).o unit_random.o
LIBCFILES = lib_exp.c lib_string.c $(PTY).c lib_debug.c
LIBOFILES = lib_exp.o lib_string.o $(PTY).o lib_debug.o
LIBEXPECT = libexpect.a

SHARFILES = README expect.man HISTORY Makefile INSTALL global.h command.h \
	translate.h term.h main.c command.c expect.c unit_random.c \
	pty_bsd.c pty_usg.c pty_unicos.c pty_sgi.c pty_sgi3.c pty_svr4.c \
	pty_aix3.c \
	inter_select.c inter_poll.c inter_simple.c \
	libexpect.man expect.h lib_exp.c lib_string.c lib_debug.c \
	test/rogue.exp test/chess.exp test/dvorak \
	test/Makefile \
	test/chesslib.c test/chesslib2.c test/chesslib++.c \
	test/time.exp test/timed_read test/ftp-rfc \
	test/ftp.exp test/passwd.exp test/archie \
	test/robohunt test/lpunlock test/kibitz test/kibitz.man \
	test/weather test/passmass test/rftp test/rlogin.exp \
	fixline1 vgrindefs FAQ CONVERTING

#	Not yet converted over to new version
#	test/gethostbyaddr	7199

all: expect $(LIBEXPECT)

expect:	$(OFILES)
	$(CC) $(CLFLAGS) -o expect $(OFILES) $(LIBS)
	$(SETUID) expect

$(LIBEXPECT): $(LIBOFILES)
	ar cr $(LIBEXPECT) $(LIBOFILES)
	$(RANLIB) $(LIBEXPECT)

install:	expect $(LIBEXPECT) $(SCRIPTS)
# install expect binary and man page, and create script directory
	$(CP) expect $(BINDIR)
	strip $(BINDIR)/expect
	$(SETUID) $(BINDIR)/expect
	nroff -man expect.man > $(MAN1DIR)/expect.$(MAN1EXT)
	-$(MKDIR) $(LIBDIR)
	-$(MKDIR) $(SCRIPTDIR)
# install expect library, man page and include file
	nroff -man libexpect.man > $(MAN3DIR)/libexpect.$(MAN3EXT)
	$(CP) $(LIBEXPECT) $(LIBDIR)
	-$(RANLIB) $(LIBDIR)/$(LIBEXPECT)
	$(CP) expect.h $(HDIR)
# install scripts and their man pages, if requested
	-if [ -n "$(SCRIPTS)" ]; then \
		chmod +x $(SCRIPTS); \
		$(CP) $(SCRIPTS) $(ARCH_INDEP_BINDIR); \
		rm $(SCRIPTS); \
	fi
	-if [ -n "$(SCRIPTS_MANPAGES)" ]; then \
	    for file in "$(SCRIPTS_MANPAGES)" ; do \
	      nroff -man test/$$file.man> $(MAN1DIR)/$$file.$(MAN1EXT) ; \
	    done \
	fi

$(SCRIPTS):
	./expect fixline1 $(SHORT_BINDIR)/expect < test/$@ > $@

clean:
	-rm -f *.o core expect *.lint *.glint dumb exho devtty *.a
	(cd test; $(MAKE) cleanup)

test:	expect dumb exho devtty
	(cd test; $(MAKE) )
	ln expect dumb exho devtty test
	# To run all tests, type: cd test; checkall

lint:
	lint $(CPPFLAGS) $(CFILES) $(TCLLINTLIB) | tee expect.lint
	lint $(CPPFLAGS) $(LIBCFILES) | tee libexpect.lint

glint:
	gcc -c -Wall $(CPPFLAGS) $(CFILES) | tee expect.glint
	gcc -c -Wall $(CPPFLAGS) $(LIBCFILES) | tee libexpect.glint
	# can't use the gcc .o with cc .o files
	rm *.o

saber_src:	$(CFILES)
	#load $(CPPFLAGS) $(CFILES) $(LIBS)

saber_obj:	$(OFILES)
	#load $(CPPFLAGS) $(OFILES) $(LIBS)

ftp:	expect.shar.Z
	rcp expect.shar.Z durer:~ftp/pub/expect
	rcp HISTORY durer:~ftp/pub/expect/HISTORY
	rm expect.shar.Z
	rsh durer ls -l /ftp/pub/expect/expect.shar.Z

expect.shar.Z:	expect.shar
	rm -f expect.shar.Z
	compress -f expect.shar

expect.shar:
	shar $(SHARFILES) > expect.shar

# for sending out to comp.sources archives, includes USENIX paper
expect.full.shar:
	shar -a -n expect -o Part -l 75 -p $(SHARFILES) -C doc/expect.ps

# create uuencoded compressed shared source - suitable for mailing
expect.src.uu:	expect.shar.Z
	uuencode expect.shar.Z expect.shar.Z > expect.src.uu
#	makekit $(SHARFILES)

print:
	enscript -2r $(SHARFILES)

# useful program for testing an expect script
dumb:	dumb.o
	$(CC) -g dumb.o -o dumb

# useful program for testing argument passing in exec/spawn/system
# stands for "expect's echo"
exho:	exho.o
	$(CC) -g exho.o -o exho

devtty:	devtty.o
	$(CC) -g devtty.o -o devtty

# test how pty's are initialized
ptytest:	ptytest.o $(PTY).o
	$(CC) -g ptytest.o $(PTY).o

expect.o:	global.h command.h
main.o:		global.h
command.o:	global.h command.h
$(INTERACT).o:	global.h command.h
$(PTY).o:	term.h translate.h

