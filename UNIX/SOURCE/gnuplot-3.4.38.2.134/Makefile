#
# $Id: makefile.unx%v 3.38.2.134 1993/06/04 01:03:20 woo Exp woo $
#
#
############################################################
#
# GNUPLOT 3.0 Makefile (Unix X11 support) for
#  Apollo/Sun/Dec5000/IBM-RS6000/HP9000/SGI/3B1/386IX/Cray
#
# Original version by:
#   oliveria@caen.engin.umich.edu (ROQUE DONIZETE DE OLIVEIRA)
#   Wed, 3 Jul 91 14:31:37 -0400
#
#>>> Customizing: You must customize part of this makefile for your site.
#>>> Then type 'make' for further instructions.
#>>> Customization instructions look like these lines do (#>>>).
#

.NOEXPORT:

TARGET = All# What to make by default

############################################################
#>>> Decide where the binaries and manuals will go.
# directory where to install executables on 'make install'
DEST=/opt/bin
# directory for installing man page on 'make man_install'.
MANDEST=/opt/man/man1
# where to install help file gnuplot.gih
HELPDEST=/opt/lib
#HELPDEST=docs/gnuplot.gih
# Where to send email about bugs and comments (locally)
EMAIL=bug-gnuplot@dartmouth.edu

############################################################
#>>> Some machines may need MAKE to be defined explicitly
MAKE   = make
#>>> Choose your C compiler and basic compiler flags.
# the C compiler
CC     = gcc
# -O if you trust your compiler's optimizer
COPTS  = -O3 -g
# default loading command
LD     = $(CC) $(CFLAGS)
NPROC  = 2
RCSVER = 3.38.2.50
RCSCOM = "D. Lewart's RS6000 fixes"
#
############################################################
#>>> Choose some optional features.
#>>> At this point there are only two optional features:
# READLINE:
#   If READLINE is defined, then command-line editing is supported.
#   Otherwise, your normal terminal editing is all you get.
#   Some machines will not support this, and they will turn this
#   option off (for example, apollos running SR10.2 or SR10.3 and
#   loaded with BSD4.3 instead of SYS5). Note: problems with
#   gnuplot prompts have been noted when this feature is selected
#   on IBM RS/6000 AIX, and compile errors are encountered on
#   Sequent Dynix 3 and Convex OS 9.0.
# NOCWDRC:
#   If NOCWDRC is defined, then any .gnuplot in the current directory
#   is not read on startup. This is a security consideration
#   especially for root users ( we recommend you define -DNOCWDRC ).
OPTIONS = -DREADLINE -DNOCWDRC

#>>> Optionally install the lasergnu script.
# Lasergnu is a handy shell script for creating a plot from the
# command line and sending it directly to the printer. It currently
# supports PostScript and imagen printers, and probably would need
# tailoring to your site.
# Use lasergnu_install to install lasergnu.
# Use lasergnu_noinstall to not install lasergnu (default).
LASERGNU = lasergnu_install

############################################################
# X11 support
#

#>>> List your X11 libraries
X11LIBS = -L/usr/X/lib -lX11 -lnsl

#>>> List where the X11 include directory is found (if other than /usr/include)
X11INCLUDES = -I/usr/X/include

#>>> List additional CFLAGS for X11 compilation
# -DOLD_SELECT if you have an old 4.2 BSD OS (e.g. Sun OS 3.5) and
#   encounter compile error for missing include file <sys/select.h>
# -DBSD_TYPES if your system defines FD_* macros for select in
#   <sys/bsdtypes.h>
# -DOW_BOTCH if you encounter load error for missing _XtInherit on Sun
#   Open Windows platforms
# -ISC22 to circumvent missing plots past the first on ISC 2.2
X11FLAGS =

#>>> You shouldn't have to change these, since they are controlled by
#>>> Machine dependent definitions below.
#       Compile option for plot.c and TERMFLAGS, to include X11 support
PLOTXFLAG = -DX11
#       make gnuplot_x11 by default
GNUPLOT_X11 = gnuplot_x11
#       install gnuplot_x11 by default
X11INSTALL = x11_install

############################################################
#>>> Okay, you've changed enough. Now type 'make'.

############################################################
# This is used to pass many of the above definitions to make
# subprocesses. Don't change this.
MY_FLAGS  = CC="$(CC)" COPTS="$(COPTS)" DEST="$(DEST)" \
        MANDEST="$(MANDEST)" HELPDEST="$(HELPDEST)" \
        EMAIL="$(EMAIL)" LASERGNU="$(LASERGNU)"

############################################################
# Explanations of CFLAGS definitions.
#  These should not need to be changed by you.
# They are set correctly for each machine below. If your machine
# doesn't fit the one of the patterns, override on the make command
# line or make a new target for it and a new _FLAGS definition.
#  -DNOVFORK if you're unix and you have don't have vfork()
#  -DMEMSET if you need to use memset() instead of bzero()
#  -DMEMCPY if your bcopy() is called memcpy()
#  -DNOCOPY if you don't have a memcpy() by any name
#  -DGAMMA=foo if your gamma function is called foo(). Apollos have
#    lgamma(3m). The default is -DGAMMA=gamma.
#  -DGETCWD if your unix uses getcwd() instead of getcd()
#    this is needed by HP-UX and Cray Unicos systems.
#  -DULTRIX_KLUDGE if you run X windows on Ultrix and experience the
#    "every other plot" problem.
#  -DCRIPPLED_SELECT if "select errors" are encountered with X. This
#    option is needed on SVR3 platforms with incomplete support for
#    the BSD select() system call
#  -DXPG3_LOCALE if your system uses the 'setlocale' function to
#    support foreign charactersets (currently only ISO8859-1).
#  -Dunix is required to explicitly define "unix" for SCO and IBM
#          RS/6000 running AIX 3.2
#  -fswitch if you are compiling on a Sun3 (or even -f68881)
#    (but -fswitch is buggy on some systems, so watch out)

# Defaults in case the user types 'make All' directly
# Should match X11_FLAGS's CFLAGS definition
CFLAGS = $(COPTS) $(OPTIONS)

############################################################
# Terminal (device) support
#
# All devices available to a given machine are compiled in by default.
# This documents the flags available in TERMFLAGS, although TERMFLAGS
# is usually controlled by the machine-dependent definitions below.
# See other terminal defines in term.h.
# Define ULTRIX_KLUDGE if you have the every-other plot problem in Ultrix X11.
#
# -DAPOLLO      Apollo Graphics Primitive Resource (window resize after replot)
# -DGPR         Apollo Graphics Primitive Resource (fixed-size window)
# -DCGI         SCO CGI
# -DIRIS4D      IRIS4D series computer
# -DSUN         Sun Microsystems Workstation
# -DUNIXPC      unixpc (ATT 3b1 or ATT 7300)
# -DUNIXPLOT    unixplot
# -DGNUGRAPH    use GNU graphics version of UNIX plot library
#               This can only be used in combination with -DUNIXPLOT
# -DX11         X11 Window System (This is $(PLOTXFLAG))
TERMFLAGS = -Iterm $(PLOTXFLAG)

############################################################
# Library explanations.
#  You shouldn't need to adjust this; again, it is handled by the
# machine-dependent definitions below.
#
#  -lplot if you have -DUNIXPLOT in TERMFLAGS
#  -lsuntool -lsunwindow -lpixrect  if you have -DSUN in TERMFLAGS
#  -lgl_s if -DIRIS4D in TERMFLAGS
#  -lccgi if -DCGI in TERMFLAGS
LIBS = -lm -lnsl -L/usr/ucblib -lucb# -lplot  (this library is no longer shipped on SUN systems)

####################################################################
# List of object files except version.o
OBJS = bitmap.o command.o contour.o eval.o graphics.o graph3d.o help.o \
       internal.o misc.o parse.o plot.o readline.o scanner.o \
       setshow.o specfun.o standard.o term.o util.o gnubin.o binary.o

NEXTOBJS = epsviewe.o
############################################################
# Machine-dependent settings.
#
X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS)" \
           LIBS="$(LIBS)" X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS)"

DEC_OSF_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -I/usr/sys/include -I/usr/sys/BINARY -DANSI -DNOVFORK -DNOGAMMA -D_XOPEN_SOURCE " \
           LIBS="$(LIBS)" \
           X11FLAGS="$(X11FLAGS) -DMOTIF -D_NO_PROTO -D_OSF_SOURCE" \
           X11INCLUDES="$(X11INCLUDES) -I/usr/include/Xm" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) "

DEC_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) " \
           LIBS="-lplot $(LIBS)" X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) -DULTRIX_KLUDGE"

APOLLO_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGAMMA=lgamma" \
           LIBS="$(LIBS)" X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="-L/usr/lib/X11 -lX11" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) -DAPOLLO -DGPR"

HP_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DMEMSET -DMEMCPY -DGETCWD" \
           LIBS="-lm" X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES="-I/usr/include/X11R4" \
           X11LIBS="-L/usr/lib/X11R4 -lX11" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="-Iterm -DX11"

SOLARIS_X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DMEMSET -DMEMCPY -DGETCWD -DSOLARIS" \
           X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) -DSOLARIS"

SUN_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DSUN" \
           LIBS="-lsuntool -lsunwindow -lpixrect $(LIBS)" \
           X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " \
           PLOTXFLAG=" " GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm"

SUN_X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DSUN" \
           LIBS="-lsuntool -lsunwindow -lpixrect $(LIBS)" \
           X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS)"

SGI_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DNOVFORK" \
           LIBS="-lgl_s -lm" X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " \
           PLOTXFLAG=" " GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm -DIRIS4D"

SGIX11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DNOVFORK" \
           LIBS="-lgl_s -lm -lc_s" X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="-L/usr/lib/X11 -lX11" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="-Iterm -DX11 -DIRIS4D"

CGI_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -Dunix" \
           LIBS="-lccgi $(LIBS)" X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " PLOTXFLAG=" " GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm -DUNIXPLOT -DCGI"

3B1_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DNOVFORK" \
           LIBS="$(LIBS)" X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " \
           PLOTXFLAG=" " GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           LD="ld /lib/crt0s.o /lib/shlib.ifile" \
           TERMFLAGS="-Iterm -DUNIXPC"

3B2_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DNOVFORK" \
           X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " \
           PLOTXFLAG=" " GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm -DUNIXPLOT" LIBS="$(LIBS) -lplot"

386IX_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DNOVFORK -DTCSETSW -DTCGETS" \
           LIBS="$(LIBS) -lcposix" X11FLAGS=" " X11INCLUDES=" " \
           X11LIBS=" " PLOTXFLAG=" " GNUPLOT_X11=" " \
           X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm -DUNIXPLOT"

386IX_X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DNOVFORK -DTCSETSW -DTCGETS" \
           LIBS="$(LIBS) -lcposix" X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11="$(GNUPLOT_X11)" \
           X11INSTALL= "$(X11INSTALL)" \
           TERMFLAGS="-Iterm -DUNIXPLOT -DX11"

ISC22_X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DNOVFORK -DISC22" \
           LIBS="$(LIBS) -lplot -lcposix" X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS) -linet -lpt -lcposix" PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11="$(GNUPLOT_X11)" \
           X11INSTALL= "$(X11INSTALL)" \
           TERMFLAGS="-Iterm -DUNIXPLOT -DX11"

AIX_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGAMMA=gamma -DXPG3_LOCALE -Dunix" \
           LIBS="$(LIBS) -lbsd" X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) -qmaxmem=9000" \
           COMFLAGS="-qmaxmem=3000" SSFLAGS="-qmaxmem=3000"

AIXO_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DGAMMA=gamma -DXPG3_LOCALE -Dunix" \
           LIBS="$(LIBS) -lbsd" X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS)"

NEXT_FLAGS = \
           CFLAGS="$(COPTS) -DGAMMA=lgamma -DNEXT -DREADLINE -DSGTTY" \
           LIBS="-lNeXT_s -lsys_s -lm" \
           X11FLAGS="$(X11FLAGS)" \
           X11INCLUDES=" " X11LIBS=" " PLOTXFLAG=" " \
           GNUPLOT_X11=" " X11INSTALL=x11_noinstall \
           TERMFLAGS="-Iterm -ObjC" OBJS=" $(OBJS) $(NEXTOBJS)"

NEXT_X11_FLAGS = \
           CFLAGS="$(COPTS) -DGAMMA=lgamma -DNEXT -DREADLINE -DSGTTY -DX_NOT_POSIX" \
           LIBS="-lNeXT_s -lsys_s -lm" \
           X11LIBS="-L/usr/lib/X11 -lX11" \
           X11INCLUDES="$(X11INCLUDES)" X11FLAGS="$(X11FLAGS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="-Iterm -DX11 -ObjC" OBJS=" $(OBJS) $(NEXTOBJS)"

CRAY_FLAGS = \
       CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DNOVFORK" \
       LIBS="-lm" X11FLAGS=" " \
       X11INCLUDES=" " \
       X11LIBS=" " \
       PLOTXFLAG=" " GNUPLOT_X11=" " \
       X11INSTALL=x11_noinstall \
       TERMFLAGS="-Iterm"

CRAY_X11_FLAGS = \
       CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DNOVFORK" \
       LIBS="-lm" X11FLAGS="$(X11FLAGS)" \
       X11INCLUDES="$(X11INCLUDES)" \
       X11LIBS="$(X11LIBS)" \
       PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11="$(GNUPLOT_X11)" \
       X11INSTALL=$(X11INSTALL) \
       TERMFLAGS="-Iterm -DX11"

PTX_X11_FLAGS = \
       CFLAGS="$(COPTS) $(OPTIONS) -DGETCWD -DMEMSET -DMEMCPY -DCRIPPLED_SELECT" \
       LIBS="-lm -lplot" X11FLAGS="$(X11FLAGS)" \
       X11INCLUDES="$(X11INCLUDES)" \
       X11LIBS="-lX11 -lseq -lsocket -linet -lnsl" \
       PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11="$(GNUPLOT_X11)" \
       X11INSTALL=$(X11INSTALL) \
       TERMFLAGS="$(TERMFLAGS)"

SEQUENT_X11_FLAGS = \
       CFLAGS="$(COPTS) $(OPTIONS) -DSGTTY -DCRIPPLED_SELECT" \
       LIBS="-lm -lplot" X11FLAGS="$(X11FLAGS) -DX_WCHAR" \
       X11INCLUDES="$(X11INCLUDES)" \
       X11LIBS="-lX11 -lseq " \
       PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11="$(GNUPLOT_X11)" \
       X11INSTALL=$(X11INSTALL) \
       TERMFLAGS="$(TERMFLAGS)"

CONVEX_X11_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -DHUGE=8.9e307" \
           LIBS="$(LIBS)" X11FLAGS="$(X11FLAGS)" X11INCLUDES="$(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS)"

KSR_FLAGS = \
           CFLAGS="$(COPTS) $(OPTIONS) -I/usr/sys/include -I/usr/sys/BINARY -DANSI -DNOVFORK -DNOGAMMA -D_XOPEN_SOURCE -DKSR" \
           LIBS="$(LIBS)" \
           X11FLAGS="$(X11FLAGS) -DMOTIF -D_NO_PROTO -D_OSF_SOURCE" \
           X11INCLUDES="$(X11INCLUDES) -I/usr/include/Xm" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS) "

LINUX_FLAGS = \
		   CFLAGS="$(COPTS) $(OPTIONS)" -m486 \
		   LIBS="-L/usr/X386/lib -lX11 $(LIBS)" \
           X11FLAGS="$(X11FLAGS)" X11INCLUDES="-I/usr/X386/include $(X11INCLUDES)" \
           X11LIBS="$(X11LIBS)" \
           PLOTXFLAG="$(PLOTXFLAG)" GNUPLOT_X11=$(GNUPLOT_X11) \
           X11INSTALL="$(X11INSTALL)" \
           TERMFLAGS="$(TERMFLAGS)"

####################################################################
# List of source files
# Used for making shar files, lint, and some dependencies.
DIRS = term demo docs docs/latextut

CSOURCE1 = bf_test.c binary.c command.c setshow.c
CSOURCE2 = help.c gnubin.c graphics.c graph3d.c internal.c
CSOURCE3 = misc.c eval.c parse.c plot.c readline.c scanner.c standard.c
CSOURCE4 = bitmap.c term.c util.c version.c
CSOURCE5 = term/ai.trm term/amiga.trm term/aed.trm term/atari.trm \
	term/bigfig.trm term/cgi.trm term/corel.trm \
	term/djsvga.trm term/dumb.trm \
	term/dxf.trm term/dxy.trm term/debug.trm \
	term/emxvga.trm term/eepic.trm term/epson.trm term/excl.trm \
	term/fig.trm term/grass.trm term/hp26.trm term/hp2648.trm term/hpgl.trm \
	term/hp500c.trm term/hpljii.trm term/metafont.trm \
	term/apollo.trm term/gpr.trm term/hppj.trm term/compact.c
CSOURCE6 = term/impcodes.h term/imagen.trm term/next.trm term/object.h \
	term/iris4d.trm term/kyo.trm term/latex.trm term/mif.trm \
	term/pbm.trm term/pslatex.trm term/gpic.trm
CSOURCE7 = term/post.trm term/pstricks.trm term/qms.trm term/regis.trm \
	term/rgip.trm term/sun.trm \
	term/t410x.trm term/tek.trm term/texdraw.trm term/tgif.h \
	term/tgif.trm term/tpic.trm \
	term/unixpc.trm term/unixplot.trm \
	term/v384.trm term/vws.trm term/x11.trm term/xlib.trm
CSOURCE8 = contour.c specfun.c gplt_x11.c
NEXTSRC  = epsviewe.m epsviewe.h
# not C code, but still needed

DEMOS = demo/1.dat demo/2.dat demo/3.dat demo/contours.dem \
	demo/controls.dem demo/electron.dem demo/glass.dat demo/param.dem \
	demo/polar.dem demo/simple.dem demo/surface1.dem \
	demo/surface2.dem demo/using.dat demo/using.dem demo/world.cor \
	demo/world.dat demo/world.dem \
	demo/err.dat demo/poldat.dem demo/polar.dat demo/errorbar.dem \
	demo/antenna.dat demo/all.dem demo/animate.dem demo/bivariat.dem \
	demo/prob.dem demo/stat.inc demo/prob2.dem demo/random.dem \
	demo/discrete.dem demo/hidden.dem demo/airfoil.dem demo/gnuplot.rot\
	demo/binary.dem demo/spline.dem demo/steps.dem demo/steps.dat \
	demo/multimsh.dem demo/whale.dat demo/hemisphr.dat \
	demo/scatter.dem demo/scatter2.dat demo/singulr.dem demo/klein.dat

ETC = Copyright 0README README.gnu README.ami makefile.unx makefile.vms \
	linkopt.amg makefile.amg makefile.ami linkopt.vms buildvms.com \
	lasergnu makefile.r makefile.nt makefile.g 0FAQ 0BUGS\
	term/README History gnuplot.el intergra.x11 0INSTALL\
	README.3p2 README.3p3 README.3p4 README.pro README.nex README.x11 \
	README.3d README.mf README.win

#BETA files (not standard distribution files)
BETA = BETA
# PC-specific files
PC = corgraph.asm corplot.c header.mac hrcgraph.asm lineproc.mac \
	linkopt.msc makefile.msc makefile.tc makefile.st makefile.djg \
	pcgraph.asm gnuplot.def makefile.286 gnuplot.prj makefile.emx \
	makefile.ztc linkopt.ztc term/fg.trm term/pc.trm
WINDOWS = makefile.win makefile.msw README.win win/wcommon.h \
	win/wgnuplib.c win/wgnuplib.def win/wgnuplib.h win/wgnuplib.rc \
	win/wgnuplot.def win/wgnuplot.hpj win/wgnuplot.mnu win/wgnuplot.rc \
	win/wgraph.c win/winmain.c win/wmenu.c win/wpause.c \
	win/wprinter.c win/wresourc.h win/wtext.c win/wtext.h \
	win/geticon.c docs/doc2rtf.c term/win.trm
OS2 = makefile.os2 os2/makefile os2/dialogs.c os2/dialogs.h os2/gclient.c \
	os2/gnuicon.uue os2/gnupmdrv.c os2/gnupmdrv.def os2/gnupmdrv.h \
	os2/gnupmdrv.itl os2/gnupmdrv.rc os2/print.c docs/doc2ipf.c \
	README.os2 term/pm.trm

# Documentation and help files
DOCS1 = docs/makefile docs/README docs/checkdoc.c docs/doc2gih.c \
	docs/doc2hlp.c docs/doc2hlp.com docs/doc2ms.c docs/doc2tex.c \
	docs/gnuplot.1 docs/lasergnu.1 docs/toc_entr.sty docs/doc2info.pl \
	docs/titlepag.ms docs/titlepag.tex docs/makefile.ami \
	docs/doc2rtf.c
DOCS2 = docs/gnuplot.doc docs/gpcard.tex
DOCS3 = docs/latextut/makefile docs/latextut/eg1.plt \
	docs/latextut/eg2.plt docs/latextut/eg3.dat docs/latextut/eg3.plt \
	docs/latextut/eg4.plt docs/latextut/eg5.plt docs/latextut/eg6.plt \
	docs/latextut/header.tex docs/latextut/tutorial.tex \
	docs/latextut/linepoin.plt

#########################################################################
# Default target (informational)
info:
	@echo "Please do a 'make <MACHINE>' where <MACHINE> is one of the following:"
	@echo
	@echo "apollo       for Apollo running SR10.3 with Apollo's X11"
	@echo "dec_osf      for Dec3100/5000 running DEC OSF/1 v1.0"
	@echo "dec          for Dec3100/5000 running Ultrix 3.1d with MIT's X11"
	@echo "hp           for HP/9000 700 series running HP/UX 8.0 with MIT's X11R4"
	@echo "sun          for Sun sparcstation running SunOS 4.1 with suntools (no X11) "
	@echo "sun_x11      for Sun sparcstation running SunOS 4.1 with suntools and X11 "
	@echo "sgi          for Silicon Graphics IRIS4D machines (no X11) "
	@echo "sgix11       for Silicon Graphics IRIS4D machines (X11) "
	@echo "next         for NeXT Cube and Slab running NeXTOS 2.0+ (no X11)"
	@echo "next_x11     for NeXT Cube and Slab running NeXTOS 2.0+ with X11"
	@echo "3b1          for ATT 3b1 machines (no X11) "
	@echo "3b2          for ATT 3b2 machines (no X11) "
	@echo "386ix        for 386 machines running 386/ix (no X11)"
	@echo "386ix_x11    for 386 machines running 386/ix with T.Roell X386"
	@echo "isc22_x11    for 386 machines running ISC 2.2 with T.Roell X386"
	@echo "aix32        for IBM RS/6000 running AIX 3.2 with xlc 1.2"
	@echo "aix32o       for IBM RS/6000 running AIX 3.2 with xlc 1.1"
	@echo "x11          for a generic machine (like a sun or dec) with MIT's X11"
	@echo "cray         for Cray Y-MP or Cray-2 running Unicos 6.0 or 6.1 (no X11)"
	@echo "cray_x11     for Cray Y-MP or Cray-2 running Unicos 6.0 or 6.1 with X11"
	@echo "ptx_x11      for Sequent Dynix/PTX with MIT X11"
	@echo "sequent_x11  for Sequent Symmetry (DYNIX 3) with X11"
	@echo "convex_x11   for Convex 9.0 with MIT X11"
	@echo "ksr          for KSR1 running DEC OSF/1 v1.0 (use make -j 16)"
	@echo "linux        for LINUX with XFree86-1.2"
	@echo
	@echo "Examples:"
	@echo
	@echo "         make x11"
	@echo "         make apollo"
	@echo "         make apollo       OPTIONS='-DNOCWDRC' "
	@echo "         make apollo       DEST='/usr/um/misc/bin' "
	@echo "         make dec"
	@echo "         make hp"
	@echo "         make next"
	@echo "         make solaris_x11"
	@echo "         make sun          HELPDEST='/usr/um/misc/lib/gnuplot.gih' "
	@echo "         make sun          X11INCLUDES='-I/opt/include' "
	@echo "         make sun_x11"
	@echo "         make sgi"
	@echo "         make 3b1"
	@echo "         make 386ix"
	@echo "         make 386ix_x11"
	@echo "         make isc22_x11"
	@echo "         make aix32        MANDEST='/usr/um/misc/man/man1' COPTS='-O' "
	@echo "         make cray"
	@echo "         make cray_x11"
	@echo
	@echo "If you just type 'make All' , it will build gnuplot for Unix X11"
	@echo "and the following variables will be used as default:"
	@echo
	@echo " DEST                     " $(DEST)
	@echo " MANDEST                  " $(MANDEST)
	@echo " HELPDEST                 " $(HELPDEST)
	@echo " EMAIL                    " $(EMAIL)
	@echo " CC                       " $(CC)
	@echo " COPTS                    " $(COPTS)
	@echo " OPTIONS                  " $(OPTIONS)
	@echo " CFLAGS                   " $(CFLAGS)
	@echo " LIBS                     " $(LIBS)
	@echo " X11FLAGS                 " $(X11FLAGS)
	@echo " X11LIBS                  " $(X11LIBS)
	@echo " X11INCLUDES              " $(X11INCLUDES)
	@echo " TERMFLAGS                " $(TERMFLAGS)
	@echo " LASERGNU                 " $(LASERGNU)
	@echo
	@echo "If you are not familiar with makefiles or just want to know what"
	@echo " 'make <MACHINE>' would do without actually doing anything, then type"
	@echo " 'make <MACHINE> -n' "
	@echo

###############################################################
# Targets for each machine

x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(X11_FLAGS)      $(TARGET)

dec_osf:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(DEC_OSF_FLAGS)  $(TARGET)

dec:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(DEC_FLAGS)      $(TARGET)

apollo:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(APOLLO_FLAGS)   $(TARGET)

hp:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(HP_FLAGS)       $(TARGET)

next:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(NEXT_FLAGS)     $(TARGET)

next_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(NEXT_X11_FLAGS) $(TARGET)

solaris_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(SOLARIS_X11_FLAGS)  $(TARGET)

sun:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(SUN_FLAGS)      $(TARGET)

sun_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(SUN_X11_FLAGS)  $(TARGET)

sgi:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(SGI_FLAGS)      $(TARGET)

sgix11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(SGIX11_FLAGS)   $(TARGET)

cgi:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(CGI_FLAGS)      $(TARGET)

3b1:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(3B1_FLAGS)      $(TARGET)

3b2:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(3B2_FLAGS)      $(TARGET)

386ix:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(386IX_FLAGS)    $(TARGET)

386ix_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(386IX_X11_FLAGS) $(TARGET)

isc22_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(ISC22_X11_FLAGS) $(TARGET)

aix32:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(AIX_FLAGS)      $(TARGET)

aix32o:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(AIXO_FLAGS)     $(TARGET)

cray:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(CRAY_FLAGS)     $(TARGET)

cray_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(CRAY_X11_FLAGS) $(TARGET)

ptx_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(PTX_X11_FLAGS)  $(TARGET)

sequent_x11:
	make $(MFLAGS) $(MY_FLAGS) $(SEQUENT_X11_FLAGS) $(TARGET)

convex_x11:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(CONVEX_X11_FLAGS) $(TARGET)

ksr:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(KSR_FLAGS)  $(TARGET)

linux:
	$(MAKE) $(MFLAGS) $(MY_FLAGS) $(LINUX_FLAGS)  $(TARGET)

#############################################################
# Targets that really do something

all:
	@echo "Please just type  'make'  in order to get some information on "
	@echo "how to build gnuplot under Unix and the X Window System."

All: gnuplot $(GNUPLOT_X11) doc demo/binary1

gnuplot: $(OBJS) version.o
	$(LD) -o gnuplot $(OBJS) version.o $(LIBS)

doc:
	( cd docs; $(MAKE) $(MFLAGS) gnuplot.gih )

gnuplot_x11: gplt_x11.c
	$(CC) $(CFLAGS) $(X11FLAGS) $(X11INCLUDES) -o gnuplot_x11 \
	gplt_x11.c $(X11LIBS)

demo/binary1 demo/binary2 demo/binary3: bf_test
	( cd demo; ../bf_test )

bf_test: bf_test.o binary.o
	$(CC) $(CFLAGS) bf_test.o binary.o -lm -o bf_test
################################################################
# Installation instructions

install:
	@echo
	@echo "Please do a 'make <MACHINE> TARGET=Install' where <MACHINE> is one of the following:"
	@echo
	@echo "apollo, dec, hp, sun, sun_x11, sgi, sgix11, cgi,"
	@echo "next, next_x11, 3b1, 386ix, 386ix_x11, aix32, aix32o,"
	@echo "x11, cray, cray_x11, ptx_x11, sequent_x11, convex_x11"
	@echo
	@echo "Examples:"
	@echo
	@echo "         make x11          TARGET=Install "
	@echo "         make apollo       TARGET=Install "
	@echo "         make dec          TARGET=Install "
	@echo "         make hp           TARGET=Install "
	@echo "         make sun          TARGET=Install HELPDEST='/usr/um/misc/lib/gnuplot.gih' "
	@echo "         make aix32        TARGET=Install MANDEST='/usr/um/misc/man/man1' COPTS='-O' "
	@echo
################################################################
# Installation targets

Install: All man_install $(X11INSTALL) $(LASERGNU)
	cp gnuplot $(DEST)
	strip $(DEST)/gnuplot
	( cd docs; $(MAKE) $(MFLAGS) install-unix HELPDEST=$(HELPDEST) )

x11_install: gnuplot_x11
	cp gnuplot_x11 $(DEST)
	strip $(DEST)/gnuplot_x11

x11_noinstall:
	@echo "X11 not requested, so gnuplot_x11 program not installed"

man_install: docs/gnuplot.1
	cp docs/gnuplot.1 $(MANDEST)

lasergnu_install: lasergnu docs/lasergnu.1
	cp lasergnu $(DEST)
	chmod 755 $(DEST)/lasergnu
	cp docs/lasergnu.1 $(MANDEST)

lasergnu_noinstall:
	@echo
	@echo "Lasergnu will not be installed by default."
	@echo "If you think you need the lasergnu script to print"
	@echo " files on the imagen or PostScript printers, then"
	@echo " type"
	@echo "      'make <MACHINE> TARGET=Install LASERGNU='lasergnu_install' "
	@echo
	@echo "Lasergnu is really not needed since within gnuplot you can"
	@echo " can create files (in impress or PostScript language) and"
	@echo " print them through your favorite print command (lpr, lp, prf, ipr)."
	@echo

################################################################
# Dependencies

plot.o: plot.c
	$(CC) $(CFLAGS) $(PLOTXFLAG) -c plot.c

term.o: term.h term.c $(CSOURCE5) $(CSOURCE6) $(CSOURCE7)
	$(CC) $(CFLAGS) $(TERMFLAGS) -c term.c

version.o:
	$(CC) $(CFLAGS) -DCONTACT=\"$(EMAIL)\" -c version.c

epsviewe.o : epsviewe.h epsviewe.m
	$(CC) -ObjC -c epsviewe.m

$(OBJS): plot.h

command.o: command.c
	$(CC) $(CFLAGS) -c command.c -DHELPFILE=\"$(HELPDEST)/gnuplot.gih\" $(COMFLAGS)

command.o help.o misc.o: help.h

command.o graphics.o graph3d.o misc.o plot.o setshow.o term.o: setshow.h

bitmap.o term.o: bitmap.h

setshow.o: setshow.c
	$(CC) $(CFLAGS) -c setshow.c $(SSFLAGS)

################################################################
# Miscellaneous targets

SOURCES=plot.h help.h setshow.h bitmap.h term.h $(CSOURCE1) $(CSOURCE2) \
	$(CSOURCE3) $(CSOURCE4) $(CSOURCE5) $(CSOURCE6) $(CSOURCE7)\
	$(CSOURCE8) $(NEXTSRC) $(WINDOWS) $(OS2)

DOCS  = $(DOCS1) $(DOCS2) $(DOCS3)

lint:
	lint -hx $(SOURCES)

clean:
	rm -f *.o *.orig *.rej *~ *.bak term/*~ term/*.orig term/*.bak
	( cd docs; $(MAKE) $(MFLAGS) clean )
	( cd docs/latextut; $(MAKE) $(MFLAGS) clean )

spotless:
	rm -f *.o *~ *.orig *.rej *.bak term/*~ term/*.orig term/*.bak \
	TAGS gnuplot gnuplot_x11 \
	bf_test demo/binary[1-3]
	( cd docs; $(MAKE) $(MFLAGS) clean )
	( cd docs/latextut; $(MAKE) $(MFLAGS) spotless )

################################################################
# Making shar files for mailing gnuplot

shar: gnuplot.sh00 gnuplot.sh01 gnuplot.sh02 gnuplot.sh03 gnuplot.sh04 \
	gnuplot.sh05 gnuplot.sh06 gnuplot.sh07 gnuplot.sh08 \
	gnuplot.sh09 gnuplot.sh10 gnuplot.sh11 gnuplot.sh12 \
	gnuplot.sh13 gnuplot.sh14 gnuplot.sh15 gnuplot.sh16

gnuplot.sh00:
	echo '#!/bin/sh' > gnuplot.sh00
	echo '# This is a shell file to make directories' >> gnuplot.sh00
	echo mkdir $(DIRS) >> gnuplot.sh00

gnuplot.sh01: $(ETC)
	shar $(ETC) > gnuplot.sh01

gnuplot.sh02: $(DOCS1)
	shar $(DOCS1) > gnuplot.sh02

gnuplot.sh03: $(DOCS2)
	shar $(DOCS2) > gnuplot.sh03

gnuplot.sh04: $(DOCS3)
	shar $(DOCS3) > gnuplot.sh04

gnuplot.sh05: $(CSOURCE1)
	shar $(CSOURCE1) > gnuplot.sh05

gnuplot.sh06: $(CSOURCE2)
	shar $(CSOURCE2) > gnuplot.sh06

gnuplot.sh07: $(CSOURCE3)
	shar $(CSOURCE3) > gnuplot.sh07

gnuplot.sh08: $(CSOURCE4)
	shar $(CSOURCE4) > gnuplot.sh08

gnuplot.sh09: $(CSOURCE5)
	shar $(CSOURCE5) > gnuplot.sh09

gnuplot.sh10: $(CSOURCE6)
	shar $(CSOURCE6) > gnuplot.sh10

gnuplot.sh11: $(CSOURCE7)
	shar $(CSOURCE7) > gnuplot.sh11

gnuplot.sh12: $(PC)
	shar $(PC) > gnuplot.sh12

gnuplot.sh13: $(CSOURCE8)
	shar $(CSOURCE8) > gnuplot.sh13

gnuplot.sh14: $(DEMOS)
	shar $(DEMOS) > gnuplot.sh14

gnuplot.sh15: $(WINDOWS)
	shar $(WINDOWS) > gnuplot.sh15

gnuplot.sh16: $(BETA)
	shar $(BETA) > gnuplot.sh16

tar: $(ETC) $(SOURCES) $(PC) $(DEMOS) $(BETA) $(DOCS)
	$(TAR) cvf /tmp/gnuplot.tar $(ETC) $(SOURCES) $(PC)\
	     $(DEMOS) $(BETA) $(DOCS)
#
# the following uses Rick Saltz's makekit shar generation tools
#

kit: $(ETC) $(SOURCES) $(PC) $(DEMOS) $(BETA) $(DOCS)
	makekit -s135k -k30 $(ETC) $(SOURCES) $(PC)\
	     $(DEMOS) $(BETA) $(DOCS) MANIFEST

branch: rcs rcsdoc rcsdemo

rcs:
	rcs -b$(RCSVER) $(ETC) $(SOURCES) $(PC)

rcsdoc:
	rcs -b$(RCSVER) $(DOCS)

rcsdemo:
	rcs -b$(RCSVER) $(DEMOS)

ciall: ci cidoc cidemo

ci:
	ci -l$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(SOURCES) $(PC) $(ETC)

cidoc:
	ci -l$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(DOCS)

cidemo:
	ci -l$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(DEMOS)

ciforce:
	ci -f$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(SOURCES) $(ETC)
	ci -f$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(DOCS) $(DEMOS)
	ci -f$(RCSVER) -m$(RCSCOM) -t-$(RCSCOM) $(PC)

coall: co codoc codemo

co:
	co -l -r$(RCSVER) $(ETC) $(SOURCES) $(PC)

codoc:
	co -l -r$(RCSVER) $(DOCS)

codemo:
	co -l -r$(RCSVER) $(DEMOS)
