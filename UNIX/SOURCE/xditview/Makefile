# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.65 91/07/25 17:50:17 rws Exp $
#
# The cpp used on this machine replaces all newlines and multiple tabs and
# spaces in a macro expansion with a single space.  Imake tries to compensate
# for this, but is not always successful.
#

# -------------------------------------------------------------------------
# Makefile generated from "Imake.tmpl" and <Imakefile>
# $XConsortium: Imake.tmpl,v 1.139 91/09/16 08:52:48 rws Exp $
#
# Platform-specific parameters may be set in the appropriate <vendor>.cf
# configuration files.  Site-specific parameters should be set in the file
# site.def.  Full rebuilds are recommended if any parameters are changed.
#
# If your C preprocessor does not define any unique symbols, you will need
# to set BOOTSTRAPCFLAGS when rebuilding imake (usually when doing
# "make World" the first time).
#

# -------------------------------------------------------------------------
# site-specific configuration parameters that need to come before
# the platform-specific parameters - edit site.def to change

# site:  $XConsortium: site.def,v 1.2 91/07/30 20:26:44 rws Exp $

# -------------------------------------------------------------------------
# platform-specific configuration parameters - edit x386.cf to change

# $Header: /home/x_cvs/mit/config/x386.cf,v 1.22 1992/07/16 10:45:34 dawes Exp $
# platform:  $XConsortium: x386.cf,v 1.7 91/08/16 19:30:10 gildea Exp $

  EXTRA_INCLUDES =

# $XConsortium: sv4Lib.rules,v 1.8 91/07/19 15:38:53 rws Exp $

# operating system:  Unix System V/386 Release 4.2 Consensys

# -------------------------------------------------------------------------
# site-specific configuration parameters that go after
# the platform-specific parameters - edit site.def to change

# site:  $XConsortium: site.def,v 1.2 91/07/30 20:26:44 rws Exp $

#
# ProjectRoot is where you want to install the binaries, libs, fonts etc
#

      PROJECTROOT = /opt
         XWINHOME = /usr/X
       XWINLIBDIR = $(XWINHOME)/lib
       XWINBINDIR = $(XWINHOME)/bin

     USLCLIENTSRC = $(TOP)/uslclients
       DESKTOPDIR = $(PROJECTROOT)/desktop
         DTADMDIR = $(PROJECTROOT)/adm
        XDEFAULTS = $(PROJECTROOT)/defaults

       FONTINCSRC = $(FONTSRC)/include
    FONTLIBINCSRC = $(FONTSRC)/lib/include
      FOLIOLIBSRC = $(FONTSRC)/lib/folio
       FONTLIBSRC = $(FONTSRC)/lib/font
    BITMAPFONTSRC = $(FONTSRC)/lib/bitmap
      FONTSLIBSRC = $(FONTSRC)/lib/
           XOLSRC = $(LIBSRC)/Xol
        XOLLIBSRC = $(LIBSRC)/Xol
            NROFF = nroff
         MANFLAGS = 444
BITMAPLIBS_TO_BUILD = libbitmap.a libbitmap.so
FONTLIBS_TO_BUILD = libfont.a libfont.so

            SHELL = /bin/sh

              TOP = .
      CURRENT_DIR = .

               AR = ar cq
  BOOTSTRAPCFLAGS =  -DSVR4 -DSYSV386 -DSYSV -DUSG
               CC = gcc
               AS = as

         COMPRESS = compress
              CPP = /lib/cpp $(STD_CPP_DEFINES)
    PREPROCESSCMD = gcc -E $(STD_CPP_DEFINES)
          INSTALL = /usr/ucb/install
               LD = ld
             LINT = lint
      LINTLIBFLAG = -o
         LINTOPTS = -ax
               LN = ln -s
             MAKE = make
               MV = mv
               CP = cp
              HCC = HccCmd
              SED = SedCmd

               RM = rm -f
            TROFF = psroff
         MSMACROS = -ms
              TBL = tbl
              EQN = eqn
     STD_INCLUDES = -I/usr/X/include -I$(TOOLS)/usr/include
  STD_CPP_DEFINES =  -DSVR4 -DSYSV386 -DSYSV -DUSG
      STD_DEFINES =  -DSVR4 -DSYSV386 -DSYSV -DUSG
 EXTRA_LOAD_FLAGS =
  EXTRA_LIBRARIES = -lsocket -lnsl
             TAGS = ctags

    SHAREDCODEDEF =
         SHLIBDEF =

    PROTO_DEFINES =

     INSTPGMFLAGS = -s

     INSTBINFLAGS = -m 0755
     INSTUIDFLAGS = -m 4755
     INSTLIBFLAGS = -m 0644
     INSTINCFLAGS = -m 0444
     INSTMANFLAGS = -m 0444
     INSTDATFLAGS = -m 0444
    INSTKMEMFLAGS = -m 4755

      PROJECTROOT = /opt

     TOP_INCLUDES = -I$(INCROOT)

      CDEBUGFLAGS = -O3 -g
        CCOPTIONS = -m386 -fpcc-struct-return -Di386
        HCCOPTIONS = DefaultHCCOptions

      ALLINCLUDES = $(INCLUDES) $(EXTRA_INCLUDES) $(TOP_INCLUDES) $(STD_INCLUDES)
       ALLDEFINES = $(ALLINCLUDES) $(STD_DEFINES) $(EXTRA_DEFINES) $(PROTO_DEFINES) $(DEFINES)
           CFLAGS = $(CDEBUGFLAGS) $(CCOPTIONS) $(ALLDEFINES)
           HCFLAGS = $(CDEBUGFLAGS) $(HCCOPTIONS) $(ALLDEFINES)
        LINTFLAGS = $(LINTOPTS) -DLINT $(ALLDEFINES)

           LDLIBS = $(SYS_LIBRARIES) $(EXTRA_LIBRARIES)

        LDOPTIONS = $(CDEBUGFLAGS) $(CCOPTIONS) $(LOCAL_LDFLAGS) -L$(USRLIBDIR)

        HLDOPTIONS = $(CDEBUGFLAGS) $(HCCOPTIONS) $(LOCAL_LDFLAGS)

   LDCOMBINEFLAGS = -r
      DEPENDFLAGS =

        MACROFILE = x386.cf
           RM_CMD = $(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut

    IMAKE_DEFINES =

         IRULESRC = $(CONFIGDIR)
        IMAKE_CMD = $(IMAKE) -DUseInstalled -I$(IRULESRC) $(IMAKE_DEFINES)

     ICONFIGFILES = $(IRULESRC)/Imake.tmpl $(IRULESRC)/Imake.rules \
			$(IRULESRC)/Project.tmpl $(IRULESRC)/site.def \
			$(IRULESRC)/$(MACROFILE) $(EXTRA_ICONFIGFILES)

# -------------------------------------------------------------------------
# X Window System Build Parameters
# $XConsortium: Project.tmpl,v 1.142 92/01/25 16:44:02 rws Exp $

# ----------------------------------------------------------------------
# LEMIS parameters (Greg Lehey, 7 September 1993)
#

         DEFINES = -DLIBDIR=\"$(PROJECTROOT)/lib\" -DDESTDIR=\"$(PROJECTROOT)/bin\"

# -------------------------------------------------------------------------
# X Window System make variables; this need to be coordinated with rules

          PATHSEP = /
        USRLIBDIR = $(XWINHOME)/lib
           BINDIR = /opt/bin
          INCROOT = /opt/include
     BUILDINCROOT = $(TOP)
      BUILDINCDIR = $(BUILDINCROOT)/X11
      BUILDINCTOP = ..
           INCDIR = $(INCROOT)/X11
           ADMDIR = /usr/adm
           LIBDIR = $(USRLIBDIR)
        CONFIGDIR = /cdimage/cdrom/ETC/xconfig
       LINTLIBDIR = $(USRLIBDIR)/lint

          FONTDIR = $(LIBDIR)/fonts
         XINITDIR = $(LIBDIR)/xinit
           XDMDIR = $(LIBDIR)/xdm
           TWMDIR = $(LIBDIR)/twm
          MANPATH = /opt/man
    MANSOURCEPATH = /opt/man/cat
        MANSUFFIX = 1
     LIBMANSUFFIX = 1
           MANDIR = $(MANSOURCEPATH)$(MANSUFFIX)
        LIBMANDIR = $(MANSOURCEPATH)$(LIBMANSUFFIX)
           NLSDIR = $(LIBDIR)/nls
        PEXAPIDIR = $(LIBDIR)/PEX
      XAPPLOADDIR = $(LIBDIR)/app-defaults
       FONTCFLAGS = -t

     INSTAPPFLAGS = $(INSTDATFLAGS)

            IMAKE = imake
           DEPEND = makedepend
              RGB = rgb

            FONTC = bdftosnf

        MKFONTDIR = mkfontdir
        MKDIRHIER = /bin/sh $(XWINLIBDIR)/mconfig/mkdirhier

        CONFIGSRC = $(TOP)/mconfig
       DOCUTILSRC = $(TOP)/doc/util
        CLIENTSRC = $(TOP)/clients
          DEMOSRC = $(TOP)/demos
           LIBSRC = $(TOP)/lib
          FONTSRC = $(TOP)/fonts
       INCLUDESRC = $(TOP)/X11
        SERVERSRC = $(TOP)/server
          UTILSRC = $(TOP)/util
        SCRIPTSRC = $(UTILSRC)/scripts
       EXAMPLESRC = $(TOP)/examples
       CONTRIBSRC = $(TOP)/../contrib
           DOCSRC = $(TOP)/doc
           RGBSRC = $(TOP)/rgb
        DEPENDSRC = $(UTILSRC)/makedepend
         IMAKESRC = $(CONFIGSRC)
         XAUTHSRC = $(LIBSRC)/Xau
          XLIBSRC = $(LIBSRC)/X
           XMUSRC = $(LIBSRC)/Xmu
       TOOLKITSRC = $(LIBSRC)/Xt
       AWIDGETSRC = $(LIBSRC)/Xaw
       OLDXLIBSRC = $(LIBSRC)/oldX
      XDMCPLIBSRC = $(LIBSRC)/Xdmcp

      BDFTOSNFSRC = $(FONTSRC)/clients/bdftosnf

     MKFONTDIRSRC = $(FONTSRC)/clients/mkfontdir
         FSLIBSRC = $(FONTSRC)/lib/fs
    FONTSERVERSRC = $(FONTSRC)/server
     EXTENSIONSRC = $(TOP)/extensions
         XILIBSRC = $(EXTENSIONSRC)/lib/xinput
      XTESTLIBSRC = $(EXTENSIONSRC)/lib/xtest
      PHIGSLIBSRC = $(EXTENSIONSRC)/lib/PEX

# $XConsortium: sv4Lib.tmpl,v 1.6 92/01/25 16:46:55 rws Exp $

#
# USL: BEGIN
#	The default revision numbers from MIT is 5. But since USL has to be
#	backward compatible, so force it to 1
#

         SOXOLREV = 1
        SOOLITREV = 1
       SOOLITMREV = 1
       SOOLITOREV = 1
          SODTREV = 1

# USL: END

SHLIBLDFLAGS = -G -z text
PICFLAGS = -K PIC

  DEPEXTENSIONLIB =
     EXTENSIONLIB = -lXext

          DEPXLIB = $(DEPEXTENSIONLIB)
             XLIB = $(EXTENSIONLIB) -lX11

        DEPXMULIB = $(USRLIBDIR)/libXmu.so.$(SOXMUREV)

              XMULIB = -lXmu

       DEPOLDXLIB =
          OLDXLIB = -loldX

      DEPXTOOLLIB =
         XTOOLLIB = -lXt

DEPXOLCOMPATLIB = $(USRLIBDIR)/libXol.so.$(SOXOLREV)
      DEPXOLLIB = $(USRLIBDIR)/libOlit.so.$(SOXOLREV)
         XOLLIB = -lOlit

        DEPXAWLIB =
           XAWLIB = -lXaw

        DEPXILIB =
           XILIB = -lXi

        DEPXTESTLIB =
           XTESTLIB = -lXtst

#
# USL: BEGIN
#

XHOSTLOC = $(DESTDIR)/$(LIBDIR)
SHLIBFLAGS =  -L $(XHOSTLOC)

#
# USL: END
#

        SOXLIBREV = 1
          SOXTREV = 1
         SOXAWREV = 1
        SOOLDXREV = 1
         SOXMUREV = 1
        SOXEXTREV = 1
      SOXINPUTREV = 1
       SOXTESTREV = 1
       SOXTRAPREV = 1

      DEPXAUTHLIB = $(USRLIBDIR)/libXau.a
         XAUTHLIB =  -lXau
      DEPXDMCPLIB = $(USRLIBDIR)/libXdmcp.a
         XDMCPLIB =  -lXdmcp

        DEPPHIGSLIB = $(USRLIBDIR)/libphigs.a
           PHIGSLIB =  -lphigs

 LINTEXTENSIONLIB = $(LINTLIBDIR)/llib-lXext.ln
         LINTXLIB = $(LINTLIBDIR)/llib-lX11.ln
          LINTXMU = $(LINTLIBDIR)/llib-lXmu.ln
        LINTXTOOL = $(LINTLIBDIR)/llib-lXt.ln
          LINTXAW = $(LINTLIBDIR)/llib-lXaw.ln
           LINTXI = $(LINTLIBDIR)/llib-lXi.ln
        LINTPHIGS = $(LINTLIBDIR)/llib-lphigs.ln

          DEPLIBS = $(DEPXAWLIB) $(DEPXMULIB) $(DEPXTOOLLIB) $(DEPXLIB)

         DEPLIBS1 = $(DEPLIBS)
         DEPLIBS2 = $(DEPLIBS)
         DEPLIBS3 = $(DEPLIBS)

# -------------------------------------------------------------------------
# Imake rules for building libraries, programs, scripts, and data files
# rules:  $XConsortium: Imake.rules,v 1.126 91/12/30 20:14:35 rws Exp $

# -------------------------------------------------------------------------
# start of Imakefile

# $XConsortium: Imakefile,v 1.13 91/07/29 14:17:44 rws Exp $

        DEPLIBS = $(DEPXAWLIB) $(DEPXMULIB) $(DEPXTOOLLIB) $(DEPXLIB)
LOCAL_LIBRARIES = $(XAWLIB) $(XMULIB) $(XTOOLLIB) $(XLIB)
  SYS_LIBRARIES = -lm
        BITMAPS = ldblarrow rdblarrow
           SRCS = xditview.c Dvi.c draw.c font.c lex.c page.c parse.c\
	   	  XFontName.c DviChar.c
           OBJS = xditview.o Dvi.o draw.o font.o lex.o page.o parse.o\
	   	  XFontName.o DviChar.o

 PROGRAM = xditview

all:: xditview

xditview: $(OBJS) $(DEPLIBS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LDOPTIONS) $(LOCAL_LIBRARIES) $(LDLIBS) $(EXTRA_LOAD_FLAGS)

install:: xditview
	@if [ -d $(DESTDIR)$(BINDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(BINDIR)); fi
	$(INSTALL) -c $(INSTPGMFLAGS)  xditview $(DESTDIR)$(BINDIR)

install.man:: xditview.man
	@if [ -d $(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(MANDIR)); fi
	$(NROFF) -man xditview.man > $(MANDIR)/xditview.$(MANSUFFIX)
	chmod $(MANFLAGS) $(MANDIR)/xditview.$(MANSUFFIX)

depend::
	$(DEPEND) $(DEPENDFLAGS) -s "# DO NOT DELETE" -- $(ALLDEFINES) $(DEPEND_DEFINES) -- $(SRCS)

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

clean::
	$(RM) $(PROGRAM)

includes::
	@if [ -d $(BUILDINCDIR)/bitmaps ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(BUILDINCDIR)/bitmaps); fi
	@(set -x; cd $(BUILDINCDIR)/bitmaps; for i in $(BITMAPS); do \
	$(RM) $$i; \
	$(LN) $(BUILDINCTOP)/../$(CURRENT_DIR)/$$i .; \
	done)

install:: Xditview.ad
	@if [ -d $(DESTDIR)$(XAPPLOADDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(XAPPLOADDIR)); fi
	$(INSTALL) -c $(INSTAPPFLAGS) Xditview.ad $(DESTDIR)$(XAPPLOADDIR)/Xditview

install:: Xdit-chrtr.ad
	@if [ -d $(DESTDIR)$(XAPPLOADDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(XAPPLOADDIR)); fi
	$(INSTALL) -c $(INSTAPPFLAGS) Xdit-chrtr.ad $(DESTDIR)$(XAPPLOADDIR)/Xditview-chrtr
INSTALLFLAGS = $(INSTINCFLAGS)

install:: $(BITMAPS)
	@if [ -d $(DESTDIR)$(INCDIR)/bitmaps ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(INCDIR)/bitmaps); fi
	@case '${MFLAGS}' in *[i]*) set +e;; esac; \
	for i in $(BITMAPS); do \
	(set -x; $(INSTALL) -c $(INSTALLFLAGS) $$i $(DESTDIR)$(INCDIR)/bitmaps); \
	done

# -------------------------------------------------------------------------
# common rules for all Makefiles - do not edit

emptyrule::

clean::
	$(RM_CMD) "#"*

Makefile::
	-@if [ -f Makefile ]; then set -x; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)

tags::
	$(TAGS) -w *.[ch]
	$(TAGS) -xw *.[ch] > TAGS

# -------------------------------------------------------------------------
# empty rules for directories that do not have SUBDIRS - do not edit

install::
	@echo "install in $(CURRENT_DIR) done"

install.man::
	@echo "install.man in $(CURRENT_DIR) done"

Makefiles::

includes::

# -------------------------------------------------------------------------
# dependencies generated by makedepend

# DO NOT DELETE

xditview.o: /usr/X/include/X11/Intrinsic.h /usr/X/include/X11/Xlib.h
xditview.o: /usr/X/include/X11/memutil.h /usr/include/string.h
xditview.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
xditview.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
xditview.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
xditview.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
xditview.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
xditview.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/StringDefs.h
xditview.o: /usr/X/include/X11/Xatom.h /usr/X/include/X11/Shell.h
xditview.o: /usr/X/include/X11/Xos.h /usr/include/fcntl.h
xditview.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
xditview.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
xditview.o: /usr/include/sys/time.h /usr/include/time.h
xditview.o: /usr/X/include/X11/Xaw/Paned.h
xditview.o: /usr/X/include/X11/Xmu/Converters.h
xditview.o: /usr/X/include/X11/Xaw/Panner.h /usr/X/include/X11/Xaw/Reports.h
xditview.o: /usr/X/include/X11/Xaw/Porthole.h
xditview.o: /usr/X/include/X11/Xaw/Viewport.h /usr/X/include/X11/Xaw/Form.h
xditview.o: /usr/X/include/X11/Xaw/Box.h /usr/X/include/X11/Xaw/Command.h
xditview.o: /usr/X/include/X11/Xaw/Label.h /usr/X/include/X11/Xaw/Simple.h
xditview.o: /usr/X/include/X11/Xaw/Dialog.h
xditview.o: /usr/X/include/X11/Xaw/MenuButton.h
xditview.o: /usr/X/include/X11/Xaw/SimpleMenu.h
xditview.o: /usr/X/include/X11/Xaw/SmeBSB.h /usr/X/include/X11/Xaw/Sme.h
xditview.o: /usr/X/include/X11/Xaw/AsciiText.h /usr/X/include/X11/Xaw/Text.h
xditview.o: /usr/X/include/X11/Xaw/TextSink.h
xditview.o: /usr/X/include/X11/Xaw/TextSrc.h
xditview.o: /usr/X/include/X11/Xaw/AsciiSrc.h
xditview.o: /usr/X/include/X11/Xaw/AsciiSink.h Dvi.h xdit.bm xdit_mask.bm
xditview.o: /usr/include/stdio.h
Dvi.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
Dvi.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
Dvi.o: /usr/include/string.h /usr/include/sys/types.h /usr/X/include/X11/X.h
Dvi.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
Dvi.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
Dvi.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
Dvi.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
Dvi.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
Dvi.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
Dvi.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
Dvi.o: /usr/X/include/X11/StringDefs.h /usr/X/include/X11/Xmu/Converters.h
Dvi.o: /usr/include/stdio.h /usr/include/ctype.h DviP.h Dvi.h
Dvi.o: /usr/X/include/X11/Xaw/SimpleP.h /usr/X/include/X11/Xaw/Simple.h
Dvi.o: DviChar.h
draw.o: /usr/X/include/X11/Xos.h /usr/X/include/X11/Xosdefs.h
draw.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/types.h
draw.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
draw.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
draw.o: /usr/include/sys/time.h /usr/include/time.h
draw.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
draw.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
draw.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
draw.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
draw.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
draw.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
draw.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
draw.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
draw.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
draw.o: /usr/X/include/X11/StringDefs.h /usr/include/stdio.h
draw.o: /usr/include/ctype.h /usr/include/math.h DviP.h Dvi.h
draw.o: /usr/X/include/X11/Xaw/SimpleP.h /usr/X/include/X11/Xaw/Simple.h
draw.o: /usr/X/include/X11/Xmu/Converters.h DviChar.h
font.o: /usr/X/include/X11/Xos.h /usr/X/include/X11/Xosdefs.h
font.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/types.h
font.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
font.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
font.o: /usr/include/sys/time.h /usr/include/time.h
font.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
font.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
font.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
font.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
font.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
font.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
font.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
font.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
font.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
font.o: /usr/X/include/X11/StringDefs.h /usr/include/stdio.h
font.o: /usr/include/ctype.h DviP.h Dvi.h /usr/X/include/X11/Xaw/SimpleP.h
font.o: /usr/X/include/X11/Xaw/Simple.h /usr/X/include/X11/Xmu/Converters.h
font.o: DviChar.h XFontName.h
lex.o: /usr/X/include/X11/Xos.h /usr/X/include/X11/Xosdefs.h
lex.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/types.h
lex.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
lex.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
lex.o: /usr/include/sys/time.h /usr/include/time.h
lex.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
lex.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
lex.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
lex.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
lex.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
lex.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
lex.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
lex.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
lex.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
lex.o: /usr/X/include/X11/StringDefs.h /usr/include/stdio.h
lex.o: /usr/include/ctype.h DviP.h Dvi.h /usr/X/include/X11/Xaw/SimpleP.h
lex.o: /usr/X/include/X11/Xaw/Simple.h /usr/X/include/X11/Xmu/Converters.h
lex.o: DviChar.h
page.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
page.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
page.o: /usr/include/string.h /usr/include/sys/types.h /usr/X/include/X11/X.h
page.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
page.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
page.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
page.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
page.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
page.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
page.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
page.o: /usr/X/include/X11/StringDefs.h /usr/X/include/X11/Xos.h
page.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h /usr/include/unistd.h
page.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
page.o: /usr/include/sys/time.h /usr/include/time.h /usr/include/stdio.h
page.o: /usr/include/ctype.h DviP.h Dvi.h /usr/X/include/X11/Xaw/SimpleP.h
page.o: /usr/X/include/X11/Xaw/Simple.h /usr/X/include/X11/Xmu/Converters.h
page.o: DviChar.h
parse.o: /usr/X/include/X11/Xos.h /usr/X/include/X11/Xosdefs.h
parse.o: /usr/include/string.h /usr/include/fcntl.h /usr/include/sys/types.h
parse.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
parse.o: /usr/include/sys/unistd.h /usr/include/sys/file.h
parse.o: /usr/include/sys/time.h /usr/include/time.h
parse.o: /usr/X/include/X11/IntrinsicP.h /usr/X/include/X11/Intrinsic.h
parse.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
parse.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
parse.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
parse.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
parse.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
parse.o: /usr/X/include/X11/RectObj.h /usr/X/include/X11/CoreP.h
parse.o: /usr/X/include/X11/CompositeP.h /usr/X/include/X11/ConstrainP.h
parse.o: /usr/X/include/X11/ObjectP.h /usr/X/include/X11/RectObjP.h
parse.o: /usr/X/include/X11/StringDefs.h /usr/include/stdio.h
parse.o: /usr/include/ctype.h DviP.h Dvi.h /usr/X/include/X11/Xaw/SimpleP.h
parse.o: /usr/X/include/X11/Xaw/Simple.h /usr/X/include/X11/Xmu/Converters.h
parse.o: DviChar.h
XFontName.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
XFontName.o: /usr/include/string.h /usr/include/sys/types.h
XFontName.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
XFontName.o: /usr/X/include/X11/Xosdefs.h /usr/X/include/X11/Intrinsic.h
XFontName.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
XFontName.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
XFontName.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
XFontName.o: /usr/X/include/X11/RectObj.h XFontName.h /usr/include/ctype.h
DviChar.o: /usr/X/include/X11/Intrinsic.h /usr/X/include/X11/Xlib.h
DviChar.o: /usr/X/include/X11/memutil.h /usr/include/string.h
DviChar.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
DviChar.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
DviChar.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/Xresource.h
DviChar.o: /usr/X/include/X11/Core.h /usr/X/include/X11/Composite.h
DviChar.o: /usr/X/include/X11/Constraint.h /usr/X/include/X11/Object.h
DviChar.o: /usr/X/include/X11/RectObj.h DviChar.h
