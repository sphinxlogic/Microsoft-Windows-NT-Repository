# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.72 92/09/14 11:44:22 rws Exp $
#
# The cpp used on this machine replaces all newlines and multiple tabs and
# spaces in a macro expansion with a single space.  Imake tries to compensate
# for this, but is not always successful.
#

# -------------------------------------------------------------------------
# Makefile generated from "Imake.tmpl" and <Imakefile>
# $XConsortium: Imake.tmpl,v 1.158 92/09/03 19:54:25 rws Exp $
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
# platform-specific configuration parameters - edit sun.cf to change

#
# platform:  $XConsortium: sun.cf,v 1.77 92/05/29 18:37:21 rws Exp $

# operating system:  SunOS 5.0

.INIT: Makefile
REVCTLARCHFILE = SCCS/s.Imakefile
REVCTLCMD = sccs get

# $XConsortium: sv4Lib.rules,v 1.8 91/07/19 15:38:53 rws Exp $

# -------------------------------------------------------------------------
# site-specific configuration parameters that go after
# the platform-specific parameters - edit site.def to change

# site:  $XConsortium: site.def,v 1.2 91/07/30 20:26:44 rws Exp $

            SHELL = /bin/sh

              TOP = .
      CURRENT_DIR = .

               AR = ar cq
  BOOTSTRAPCFLAGS =
               CC = cc
               AS = as

         COMPRESS = compress
              CPP = /usr/ccs/lib/cpp $(STD_CPP_DEFINES)
    PREPROCESSCMD = cc -E $(STD_CPP_DEFINES)
          INSTALL = install
               LD = ld
              LEX = lex
             YACC = yacc
             LINT = lint
      LINTLIBFLAG = -o
         LINTOPTS = -ax
               LN = ln -s
             MAKE = make
               MV = mv
               CP = cp

               RM = rm -f
            TROFF = troff -Tpost -t
         MSMACROS = -ms
              TBL = tbl
              EQN = eqn
     STD_INCLUDES =
  STD_CPP_DEFINES = -DSVR4 -DSYSV
      STD_DEFINES = -DSVR4 -DSYSV
 EXTRA_LOAD_FLAGS =
  EXTRA_LDOPTIONS = -R$(LD_RUN_PATH):/usr/openwin/lib
  EXTRA_LIBRARIES = -lsocket -lnsl
             TAGS = ctags

    SHAREDCODEDEF =
         SHLIBDEF =

    PROTO_DEFINES =

     INSTPGMFLAGS =

     INSTBINFLAGS = -m 0755
     INSTUIDFLAGS = -m 4755
     INSTLIBFLAGS = -m 0644
     INSTINCFLAGS = -m 0444
     INSTMANFLAGS = -m 0444
     INSTDATFLAGS = -m 0444
    INSTKMEMFLAGS = -m 4755

      PROJECTROOT = $(OPENWINHOME)
      DESTDIR =

     TOP_INCLUDES = -I$(INCROOT)

      CDEBUGFLAGS = -O -xF
        CCOPTIONS = -DSYSV -DSVR4 -xF -Wa,-cg92

      ALLINCLUDES = $(INCLUDES) $(EXTRA_INCLUDES) $(TOP_INCLUDES) $(STD_INCLUDES)
       ALLDEFINES = $(ALLINCLUDES) $(STD_DEFINES) $(EXTRA_DEFINES) $(PROTO_DEFINES) $(DEFINES)
           CFLAGS = $(CDEBUGFLAGS) $(CCOPTIONS) $(ALLDEFINES)
	  CCFLAGS = $(CDEBUGFLAGS) $(CCCOPTIONS) $(ALLDEFINES)
        LINTFLAGS = $(LINTOPTS) -DLINT $(ALLDEFINES)

           LDLIBS = $(SYS_LIBRARIES) $(EXTRA_LIBRARIES)

        LDOPTIONS = $(CDEBUGFLAGS) $(CCOPTIONS) $(EXTRA_LDOPTIONS) $(LOCAL_LDFLAGS) -L$(USRLIBDIR)

   LDCOMBINEFLAGS = -r
      DEPENDFLAGS =

        MACROFILE = sun.cf
           RM_CMD = $(RM) mapfile *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut *.O

    IMAKE_DEFINES =

         IRULESRC = $(CONFIGDIR)
        IMAKE_CMD = $(IMAKE) -DUseInstalled -I$(IRULESRC) $(IMAKE_DEFINES)

     ICONFIGFILES = $(IRULESRC)/Imake.tmpl $(IRULESRC)/Imake.rules \
			$(IRULESRC)/Project.tmpl $(IRULESRC)/site.def \
			$(IRULESRC)/$(MACROFILE)  \
			$(EXTRA_ICONFIGFILES)

# -------------------------------------------------------------------------
# X Window System Build Parameters
# $XConsortium: Project.tmpl,v 1.152 92/08/10 17:47:45 eswu Exp $

# -------------------------------------------------------------------------
# X Window System make variables; this need to be coordinated with rules

          PATHSEP = /
        USRLIBDIR = $(OPENWINHOME)/lib
         SHLIBDIR = $(OPENWINHOME)/lib
           BINDIR = $(OPENWINHOME)/bin
          DEMODIR = $(OPENWINHOME)/demo
          INCROOT = $(OPENWINHOME)/include
     BUILDINCROOT = $(TOP)
      BUILDINCDIR = $(BUILDINCROOT)/X11
      BUILDINCTOP = ..
           INCDIR = $(INCROOT)/X11
           ADMDIR = /usr/adm
           LIBDIR = $(USRLIBDIR)/X11
        CONFIGDIR = /usr/openwin/lib/config
       LINTLIBDIR = $(USRLIBDIR)/lint

          FONTDIR = $(LIBDIR)/fonts
         XINITDIR = $(LIBDIR)/xinit
           XDMDIR = $(LIBDIR)/xdm
           TWMDIR = $(LIBDIR)/twm
          MANPATH = $(OPENWINHOME)/man
    MANSOURCEPATH = $(MANPATH)/man
        MANSUFFIX = n
     LIBMANSUFFIX = 3
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

            FONTC = bdftopcf

        MKFONTDIR = mkfontdir
        MKDIRHIER = /bin/sh $(BINDIR)/mkdirhier

        CONFIGSRC = $(TOP)/config
       DOCUTILSRC = $(TOP)/doc/util
        CLIENTSRC = $(TOP)/clients
          DEMOSRC = $(TOP)/demos
           LIBSRC = $(TOP)/lib
          FONTSRC = $(TOP)/../../../lib/libfont
       INCLUDESRC = $(TOP)/X11
        SERVERSRC = $(TOP)/server
          DDXSRC  = $(SERVERSRC)/ddx
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

      BDFTOPCFSRC = $(FONTSRC)/clients/bdftopcf

     MKFONTDIRSRC = $(FONTSRC)/clients/mkfontdir
         FSLIBSRC = $(FONTSRC)/lib/fs
    FONTSERVERSRC = $(FONTSRC)/server
     EXTENSIONSRC = $(TOP)/extensions
         XILIBSRC = $(TOP)/../../../lib/libXinput
      XTESTLIBSRC = $(EXTENSIONSRC)/lib/xtest
        PEXLIBSRC = $(EXTENSIONSRC)/lib/PEXlib
      PHIGSLIBSRC = $(EXTENSIONSRC)/lib/PEX
	  DGALIBSRC = $(EXTENSIONSRC)/lib/dga

# $XConsortium: sv4Lib.tmpl,v 1.8 92/06/28 17:43:23 rws Exp $

       SERVERETC = $(OPENWINHOME)/server/etc

       SERVERLIB = $(OPENWINHOME)/server/lib

   SERVERMODULES = $(OPENWINHOME)/server/modules

SHLIBLDFLAGS = -G -z text -M mapfile
PICFLAGS = -K PIC
MAPFILE = mapfile

  DEPEXTENSIONLIB =
     EXTENSIONLIB = -lXext

          DEPXLIB = $(DEPEXTENSIONLIB)
         XLIBONLY = -lX11
             XLIB = $(EXTENSIONLIB) $(XLIBONLY)

        DEPXMULIB =
       XMULIBONLY = -lXmu
           XMULIB = $(XMULIBONLY) -z nodefs

       DEPOLDXLIB =
          OLDXLIB = -loldX

      DEPXTOOLLIB =
         XTOOLLIB = -lXt

        DEPXAWLIB =
           XAWLIB = -lXaw

        DEPXILIB =
           XILIB = -lXi

        DEPXTESTLIB =
           XTESTLIB = -lXtst

        DEPPEXLIB =
         PEXLIB = -lPEX5

        DEPDGALIB =
           DGALIB = -ldga

        SOXLIBREV = 5.0
          SOXTREV = 5.0
         SOXAWREV = 5.0
        SOOLDXREV = 5.0
         SOXMUREV = 5.0
        SOXEXTREV = 5.0
      SOXINPUTREV = 5.0
       SOXTESTREV = 1.0
       SOXTRAPREV = 1.0
         SOPEXREV = 1.0
	 SODPSREV = 5
	 SODGAREV = 1

      DEPXAUTHLIB = $(USRLIBDIR)/libXau.a
         XAUTHLIB =  -lXau
      DEPXDMCPLIB = $(USRLIBDIR)/libXdmcp.a
         XDMCPLIB =  -lXdmcp

        DEPPHIGSLIB = $(USRLIBDIR)/libphigs.a
           PHIGSLIB =  -lphigs

       DEPXBSDLIB = $(USRLIBDIR)/libXbsd.a
          XBSDLIB =  -lXbsd

 LINTEXTENSIONLIB = $(LINTLIBDIR)/llib-lXext.ln
         LINTXLIB = $(LINTLIBDIR)/llib-lX11.ln
          LINTXMU = $(LINTLIBDIR)/llib-lXmu.ln
        LINTXTOOL = $(LINTLIBDIR)/llib-lXt.ln
          LINTXAW = $(LINTLIBDIR)/llib-lXaw.ln
           LINTXI = $(LINTLIBDIR)/llib-lXi.ln
          LINTPEX = $(LINTLIBDIR)/llib-lPEX5.ln
        LINTPHIGS = $(LINTLIBDIR)/llib-lphigs.ln
		LINTDGA = $(LINTLIBDIR)/llib-ldga.ln

          DEPLIBS = $(DEPXAWLIB) $(DEPXMULIB) $(DEPXTOOLLIB) $(DEPXLIB)

         DEPLIBS1 = $(DEPLIBS)
         DEPLIBS2 = $(DEPLIBS)
         DEPLIBS3 = $(DEPLIBS)

# -------------------------------------------------------------------------
# Imake rules for building libraries, programs, scripts, and data files
# rules:  $XConsortium: Imake.rules,v 1.129 92/05/29 17:01:19 rws Exp $

# -------------------------------------------------------------------------
# start of Imakefile

PROG = CaribbeanStud
DEPEND = makedepend

XINCLUDES= -I/opt/SUNWmotif/include -I/usr/dt/include \
              -I/usr/openwin/include -I/usr/openwin/include/X11

XLIBS= -L/opt/SUNWmotif/lib -L/usr/dt/lib -L/usr/openwin/lib

UILFLAGS=-I/usr/include/uil -I/usr/dt/include/uil -I/opt/SUNWmotif/share/include/uil

EXTRA_INCLUDES =  -I/opt/SUNWspro/SC4.0/include/CC ${XINCLUDES} ${UILFLAGS}

EXTRA_LIBRARIES = -lXm -lXmu -lXt -lX11 -lgen
EXTRA_LDOPTIONS = -xildon

  CDEBUGFLAGS =

CCCOPTIONS = $(CDEBUGFLAGS)
CCOPTIONS =

HEADERS = \
   Card.h        \
   Dealer.h   \
   Deck.h        \
   Hands.h       \
   Player.h     \
   Seat.h       \
   Table.h

SRCS = CaribbeanStud.cpp \
	Card.cpp    	 \
	Dealer.cpp	 \
	Deck.cpp    	 \
	Hands.cpp   	 \
	Player.cpp  	\
	Seat.cpp    	\
	Table.cpp

OBJS = Card.o    	 \
	Dealer.o	 \
	Deck.o    	 \
	Hands.o   	 \
	Player.o  	\
	Seat.o    	\
	Table.o		\
	CaribbeanStud.o

OTHERS = \
   Imakefile \
   $(PROG).res \
   README

all:: $(PROG)

CaribbeanStud:  $(OBJS)
	$(RM) $@
	$(CCC) $(CCFLAGS) -o $@  $(OBJS) $(LDOPTIONS)  $(LDLIBS)  $(EXTRA_LOAD_FLAGS)

depend:: $(SRCS) $(HEADERS)
	$(DEPEND) $(DEPENDFLAGS) -s "# DO NOT DELETE" -- $(ALLDEFINES) -- $(SRCS)
dependlocal:: $(SRCS) $(HEADERS)
	$(DEPEND) $(DEPENDFLAGS) -s "# DO NOT DELETE" -- $(ALLDEFINES) -- $(SRCS)
clean::
	$(RM) CaribbeanStud

Install:
	$(MAKE) $(MFLAGS) Makefile
	$(MAKE) $(MFLAGS) clean
	$(MAKE) $(MFLAGS) includes
	$(MAKE) $(MFLAGS) depend
	$(MAKE) $(MFLAGS) all
	@echo ""

.SUFFIXES:	.cpp $(SUFFIXES)
.cpp.o:
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# -------------------------------------------------------------------------
# common rules for all Makefiles - do not edit

emptyrule::

dependlocal::
includes:: $(HEADERS) $(OTHERFILES)

clean::
	$(RM_CMD) "#"*

Makefile:: Imakefile
	-@if [ -f Makefile ]; then set -x; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)
	$(MAKE) $(MFLAGS) dependlocal

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

CaribbeanStud.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
CaribbeanStud.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Dealer.h
CaribbeanStud.o: /usr/include/assert.h Hands.h Card.h Deck.h
CaribbeanStud.o: /usr/include/sys/types.h /usr/include/sys/feature_tests.h
CaribbeanStud.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
CaribbeanStud.o: /usr/include/time.h Player.h Table.h Seat.h
CaribbeanStud.o: /usr/include/stdio.h /usr/include/sys/va_list.h
CaribbeanStud.o: /usr/openwin/include/X11/Intrinsic.h
CaribbeanStud.o: /usr/openwin/include/X11/Xlib.h /usr/openwin/include/X11/X.h
CaribbeanStud.o: /usr/openwin/include/X11/Xfuncproto.h
CaribbeanStud.o: /usr/openwin/include/X11/Xosdefs.h /usr/include/stddef.h
CaribbeanStud.o: /usr/openwin/include/X11/Xutil.h
CaribbeanStud.o: /usr/openwin/include/X11/Xresource.h /usr/include/string.h
CaribbeanStud.o: /usr/openwin/include/X11/Core.h
CaribbeanStud.o: /usr/openwin/include/X11/Composite.h
CaribbeanStud.o: /usr/openwin/include/X11/Constraint.h
CaribbeanStud.o: /usr/openwin/include/X11/Object.h
CaribbeanStud.o: /usr/openwin/include/X11/RectObj.h
CaribbeanStud.o: /usr/openwin/include/X11/StringDefs.h
CaribbeanStud.o: /usr/openwin/include/X11/Xmu/Drawing.h
CaribbeanStud.o: /usr/dt/include/Xm/Xm.h /usr/openwin/include/X11/Shell.h
CaribbeanStud.o: /usr/openwin/include/X11/Xatom.h
CaribbeanStud.o: /usr/dt/include/Xm/XmStrDefs.h /usr/dt/include/Xm/VirtKeys.h
CaribbeanStud.o: /usr/dt/include/Xm/VendorS.h
Card.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Card.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Card.h
Dealer.o: /usr/include/stdlib.h /usr/include/sys/feature_tests.h
Dealer.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Dealer.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Card.h Deck.h
Dealer.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
Dealer.o: /usr/include/sys/machtypes.h /usr/include/time.h Dealer.h
Dealer.o: /usr/include/assert.h Hands.h Player.h
Deck.o: /usr/include/stdlib.h /usr/include/sys/feature_tests.h
Deck.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Deck.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Card.h Deck.h
Deck.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
Deck.o: /usr/include/sys/machtypes.h /usr/include/time.h
Hands.o: Card.h /opt/SUNWspro/SC4.0/include/CC/iostream.h
Hands.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Hands.h
Player.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Player.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h Player.h Hands.h Card.h
Player.o: Dealer.h /usr/include/assert.h
Seat.o: /usr/include/stdlib.h /usr/include/sys/feature_tests.h
Seat.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Seat.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h
Seat.o: /usr/openwin/include/X11/Intrinsic.h /usr/openwin/include/X11/Xlib.h
Seat.o: /usr/include/sys/types.h /usr/include/sys/isa_defs.h
Seat.o: /usr/include/sys/machtypes.h /usr/openwin/include/X11/X.h
Seat.o: /usr/openwin/include/X11/Xfuncproto.h
Seat.o: /usr/openwin/include/X11/Xosdefs.h /usr/include/stddef.h
Seat.o: /usr/openwin/include/X11/Xutil.h /usr/openwin/include/X11/Xresource.h
Seat.o: /usr/include/string.h /usr/openwin/include/X11/Core.h
Seat.o: /usr/openwin/include/X11/Composite.h
Seat.o: /usr/openwin/include/X11/Constraint.h
Seat.o: /usr/openwin/include/X11/Object.h /usr/openwin/include/X11/RectObj.h
Seat.o: /usr/openwin/include/X11/StringDefs.h
Seat.o: /usr/openwin/include/X11/Xmu/Drawing.h /usr/dt/include/Xm/Xm.h
Seat.o: /usr/openwin/include/X11/Shell.h /usr/openwin/include/X11/Xatom.h
Seat.o: /usr/dt/include/Xm/XmStrDefs.h /usr/dt/include/Xm/VirtKeys.h
Seat.o: /usr/dt/include/Xm/VendorS.h Hands.h Card.h Seat.h
Seat.o: /usr/include/stdio.h /usr/include/sys/va_list.h Table.h
Seat.o: /usr/include/assert.h /opt/SUNWspro/SC4.0/include/CC/strings.h
Seat.o: Bitmaps.h
Table.o: /opt/SUNWspro/SC4.0/include/CC/iostream.h
Table.o: /opt/SUNWspro/SC4.0/include/CC/rlocks.h
Table.o: /usr/openwin/include/X11/Xatom.h
Table.o: /usr/openwin/include/X11/Intrinsic.h /usr/openwin/include/X11/Xlib.h
Table.o: /usr/include/sys/types.h /usr/include/sys/feature_tests.h
Table.o: /usr/include/sys/isa_defs.h /usr/include/sys/machtypes.h
Table.o: /usr/openwin/include/X11/X.h /usr/openwin/include/X11/Xfuncproto.h
Table.o: /usr/openwin/include/X11/Xosdefs.h /usr/include/stddef.h
Table.o: /usr/openwin/include/X11/Xutil.h
Table.o: /usr/openwin/include/X11/Xresource.h /usr/include/string.h
Table.o: /usr/openwin/include/X11/Core.h /usr/openwin/include/X11/Composite.h
Table.o: /usr/openwin/include/X11/Constraint.h
Table.o: /usr/openwin/include/X11/Object.h /usr/openwin/include/X11/RectObj.h
Table.o: /usr/openwin/include/X11/Shell.h /usr/dt/include/Xm/Xm.h
Table.o: /usr/dt/include/Xm/XmStrDefs.h /usr/openwin/include/X11/StringDefs.h
Table.o: /usr/dt/include/Xm/VirtKeys.h /usr/dt/include/Xm/VendorS.h
Table.o: /usr/dt/include/Xm/MessageB.h /usr/dt/include/Xm/DrawingA.h
Table.o: /usr/dt/include/Xm/Form.h /usr/dt/include/Xm/BulletinB.h
Table.o: /usr/dt/include/Xm/Label.h /usr/dt/include/Xm/Scale.h
Table.o: /usr/dt/include/Xm/ScrollBar.h /usr/dt/include/Xm/ToggleB.h
Table.o: /usr/dt/include/Xm/PushBG.h /usr/dt/include/Xm/SeparatoG.h
Table.o: /usr/dt/include/Xm/Text.h /usr/include/stdio.h
Table.o: /usr/include/sys/va_list.h /usr/include/stdlib.h
Table.o: /usr/include/assert.h Dealer.h Hands.h Card.h Player.h Table.h
Table.o: Seat.h /usr/openwin/include/X11/Xmu/Drawing.h
