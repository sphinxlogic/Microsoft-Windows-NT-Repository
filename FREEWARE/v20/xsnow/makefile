# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.51 89/12/12 12:37:30 jim Exp $

###########################################################################
# Makefile generated from "Imake.tmpl" and <Imakefile>
# $XConsortium: Imake.tmpl,v 1.77 89/12/18 17:01:37 jim Exp $
#
# Platform-specific parameters may be set in the appropriate .cf
# configuration files.  Site-wide parameters may be set in the file
# site.def.  Full rebuilds are recommended if any parameters are changed.
#
# If your C preprocessor doesn't define any unique symbols, you'll need
# to set BOOTSTRAPCFLAGS when rebuilding imake (usually when doing
# "make Makefile", "make Makefiles", or "make World").
#
# If you absolutely can't get imake to work, you'll need to set the
# variables at the top of each Makefile as well as the dependencies at the
# bottom (makedepend will do this automatically).
#

###########################################################################
# platform-specific configuration parameters - edit sgi.cf to change

CRT1 = /usr/lib/crt1.o
CRTN = /usr/lib/crtn.o

###########################################################################
# site-specific configuration parameters - edit site.def to change

# site:  $XConsortium: site.def,v 1.21 89/12/06 11:46:50 jim Exp $

        OPTIMIZER = -O2

            SHELL = /bin/sh

              TOP = .
      CURRENT_DIR = .

               AR = ar scq
  BOOTSTRAPCFLAGS = "$(OPTIMIZER)"
               CC = cc

         COMPRESS = compress
              CPP = /lib/cpp $(STD_CPP_DEFINES)
    PREPROCESSCMD = cc -E $(STD_CPP_DEFINES)
          INSTALL = bsdinst.sh
               LD = ld
             LINT = lint
      LINTLIBFLAG = -o
         LINTOPTS = -ax
               LN = ln -s

               MV = mv
               CP = cp
           RANLIB = /bin/true
  RANLIBINSTFLAGS =
               RM = rm -f
  VENDOR_INCLUDES =
     STD_INCLUDES =
  STD_CPP_DEFINES = -DSYSV
      STD_DEFINES = -DSYSV -DMALLOC_0_RETURNS_NULL
 EXTRA_LOAD_FLAGS =
  EXTRA_LIBRARIES = -lsun -lmalloc -lc_s
             TAGS = ctags

    SHAREDCODEDEF = -DSHAREDCODE
         SHLIBDEF = -DATTSHAREDLIB

    PROTO_DEFINES = -DFUNCPROTO -DNARROWPROTO

     INSTPGMFLAGS = -s

     INSTBINFLAGS = -m 0755
     INSTUIDFLAGS = -m 4755
     INSTLIBFLAGS = -m 0664
     INSTINCFLAGS = -m 0444
     INSTMANFLAGS = -m 0444
     INSTDATFLAGS = -m 0444
    INSTKMEMFLAGS = -m 2755 -g sys

          DESTDIR =

     TOP_INCLUDES = -I$(INCROOT)

      CDEBUGFLAGS = $(OPTIMIZER)
        CCOPTIONS = -prototypes -cckr -float
      COMPATFLAGS =

      ALLINCLUDES = $(STD_INCLUDES) $(TOP_INCLUDES) $(INCLUDES) $(EXTRA_INCLUDES) $(VENDOR_INCLUDES)
       ALLDEFINES = $(ALLINCLUDES) $(STD_DEFINES) $(PROTO_DEFINES) $(DEFINES) $(COMPATFLAGS)
           CFLAGS = $(CDEBUGFLAGS) $(CCOPTIONS) $(ALLDEFINES)
        LINTFLAGS = $(LINTOPTS) -DLINT $(ALLDEFINES)
           LDLIBS = $(SYS_LIBRARIES) $(EXTRA_LIBRARIES)
        LDOPTIONS = $(CDEBUGFLAGS) $(CCOPTIONS)
   LDCOMBINEFLAGS = -r

        MACROFILE = sgi.cf
           RM_CMD = $(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut

    IMAKE_DEFINES =

         IRULESRC = $(CONFIGDIR)
        IMAKE_CMD = $(IMAKE) -DUseInstalled -I$(IRULESRC) $(IMAKE_DEFINES)

     ICONFIGFILES = $(IRULESRC)/Imake.tmpl $(IRULESRC)/Imake.rules \
			$(IRULESRC)/Project.tmpl $(IRULESRC)/site.def \
			$(IRULESRC)/$(MACROFILE) $(EXTRA_ICONFIGFILES)

###########################################################################
# X Window System Build Parameters
# $XConsortium: Project.tmpl,v 1.68 90/07/03 12:14:47 rws Exp $

###########################################################################
# X Window System make variables; this need to be coordinated with rules
# $XConsortium: Project.tmpl,v 1.68 90/07/03 12:14:47 rws Exp $

          PATHSEP = /
        USRLIBDIR = $(DESTDIR)/usr/lib
           BINDIR = $(DESTDIR)/usr/bin/X11
          INCROOT = $(DESTDIR)/usr/include
     BUILDINCROOT = $(TOP)
      BUILDINCDIR = $(BUILDINCROOT)/X11
      BUILDINCTOP = ..
           INCDIR = $(INCROOT)/X11
           ADMDIR = $(DESTDIR)/usr/adm
           LIBDIR = $(USRLIBDIR)/X11
        CONFIGDIR = $(LIBDIR)/config
       LINTLIBDIR = $(USRLIBDIR)/lint

          FONTDIR = $(LIBDIR)/fonts
         XINITDIR = $(LIBDIR)/xinit
           XDMDIR = $(LIBDIR)/xdm
           AWMDIR = $(LIBDIR)/awm
           TWMDIR = $(LIBDIR)/twm
           GWMDIR = $(LIBDIR)/gwm
          MANPATH = $(DESTDIR)/usr/man
    MANSOURCEPATH = $(MANPATH)/man
           MANDIR = $(MANSOURCEPATH)n
        LIBMANDIR = $(MANSOURCEPATH)3
      XAPPLOADDIR = $(LIBDIR)/app-defaults

       FONTCFLAGS = -t

     INSTAPPFLAGS = $(INSTDATFLAGS)

            IMAKE = imake
           DEPEND = makedepend
              RGB = rgb

            FONTC = bdftopcf

        MKFONTDIR = mkfontdir

        MKDIRHIER = mkdir -p

        CONFIGSRC = $(TOP)/config
        CLIENTSRC = $(TOP)/clients
          DEMOSRC = $(TOP)/demos
           LIBSRC = $(TOP)/lib

          FONTSRC = $(TOP)/fonts.pcf

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
         XILIBSRC = $(EXTENSIONSRC)/lib/xinput
      BDFTOSNFSRC = $(FONTSRC)/tools/bdftosnf
      BDFTOPCFSRC = $(FONTSRC)/tools/bdftopcf
     MKFONTDIRSRC = $(FONTSRC)/tools/mkfontdir
     EXTENSIONSRC = $(TOP)/extensions

  DEPEXTENSIONLIB =  $(USRLIBDIR)/libXext.a
     EXTENSIONLIB =   -lXext

          DEPXLIB = $(DEPEXTENSIONLIB)  $(USRLIBDIR)/libX11_s.a
             XLIB = $(EXTENSIONLIB)  -lX11_s

      DEPXAUTHLIB =  $(USRLIBDIR)/libXau.a
         XAUTHLIB =   -lXau

        DEPXMULIB =  $(USRLIBDIR)/libXmu.a
           XMULIB =   -lXmu

       DEPOLDXLIB =  $(USRLIBDIR)/liboldX.a
          OLDXLIB =   -loldX

      DEPXTOOLLIB =  $(USRLIBDIR)/libXt_s.a
         XTOOLLIB =  -lXt_s

        DEPXAWLIB =  $(USRLIBDIR)/libXaw.a
           XAWLIB =   -lXaw

         DEPXILIB =  $(USRLIBDIR)/libXi.a
            XILIB =   -lXi

 LINTEXTENSIONLIB =  $(LINTLIBDIR)/llib-lXext.ln
         LINTXLIB =  $(LINTLIBDIR)/llib-lX11.ln
          LINTXMU =  $(LINTLIBDIR)/llib-lXmu.ln
        LINTXTOOL =  $(LINTLIBDIR)/llib-lXt.ln
          LINTXAW =  $(LINTLIBDIR)/llib-lXaw.ln

          DEPLIBS = $(DEPXAWLIB) $(DEPXMULIB) $(DEPXTOOLLIB) $(DEPXLIB)

         DEPLIBS1 = $(DEPLIBS)
         DEPLIBS2 = $(DEPLIBS)
         DEPLIBS3 = $(DEPLIBS)

###########################################################################
# Imake rules for building libraries, programs, scripts, and data files
# rules:  $XConsortium: Imake.rules,v 1.70 90/05/04 16:57:30 keith Exp $

###########################################################################
# start of Imakefile

SYS_LIBRARIES = -lm
LOCAL_LIBRARIES = $(XLIB)
DEPLIBS = $(DEPXLIB)
SRCS = xsnow.c snowmap.h vroot.h
OBJS = xsnow.o

        PROGRAM = xsnow

all:: xsnow

xsnow: $(OBJS) $(DEPLIBS)
	 $(RM) $@ ; \
	$(CC) -o $@ $(OBJS) $(LDOPTIONS) $(LOCAL_LIBRARIES) $(LDLIBS) $(EXTRA_LOAD_FLAGS)

install:: xsnow
	$(INSTALL) -c $(INSTPGMFLAGS) $(INSTALLFLAGS) xsnow $(BINDIR)

install.man:: xsnow.man
	$(INSTALL) -c $(INSTMANFLAGS) xsnow.man $(MANDIR)/xsnow.n

depend::
	$(DEPEND) -s "# DO NOT DELETE" -- $(ALLDEFINES) -- $(SRCS)

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

clean::
	$(RM) $(PROGRAM)

###########################################################################
# common rules for all Makefiles - do not edit

emptyrule::

clean::
	$(RM_CMD) \#*

Makefile::
	-@if [ -f Makefile ]; then \
		echo "	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
		$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else : ; fi ; \
	$(IMAKE_CMD) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)

tags::
	$(TAGS) -w *.[ch]
	$(TAGS) -xw *.[ch] > TAGS

###########################################################################
# empty rules for directories that do not have SUBDIRS - do not edit

install::
	@echo "install in $(CURRENT_DIR) done"

install.man::
	@echo "install.man in $(CURRENT_DIR) done"

Makefiles::

includes::

###########################################################################
# dependencies generated by makedepend

# DO NOT DELETE

xsnow.o: /usr/include/X11/Intrinsic.h /usr/include/X11/Xlib.h
xsnow.o: /usr/include/sys/types.h /usr/include/X11/X.h
xsnow.o: /usr/include/X11/Xutil.h /usr/include/X11/Xresource.h
xsnow.o: /usr/include/X11/Xos.h /usr/include/strings.h /usr/include/string.h
xsnow.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
xsnow.o: /usr/include/sys/file.h /usr/include/sys/time.h /usr/include/time.h
xsnow.o: /usr/include/X11/Core.h /usr/include/X11/Composite.h
xsnow.o: /usr/include/X11/Constraint.h /usr/include/X11/Object.h
xsnow.o: /usr/include/X11/RectObj.h vroot.h /usr/include/X11/Xatom.h
xsnow.o: /usr/include/stdio.h /usr/include/math.h /usr/include/sgimath.h
xsnow.o: /usr/include/signal.h /usr/include/sys/signal.h
xsnow.o: /usr/include/limits.h snowmap.h snow00.xbm snow01.xbm snow02.xbm
xsnow.o: snow03.xbm snow04.xbm snow05.xbm snow06.xbm tannenbaum.xbm
xsnow.o: sleigh1_0.xbm sleigh1_1.xbm sleigh1_2.xbm sleigh2_0.xbm
xsnow.o: sleigh2_1.xbm sleigh2_2.xbm sleigh3_0.xbm sleigh3_1.xbm
xsnow.o: sleigh3_2.xbm santa1.xbm santa2.xbm santa3.xbm santa1_fur.xbm
xsnow.o: santa2_fur.xbm santa3_fur.xbm
snowmap.o: snow00.xbm snow01.xbm snow02.xbm snow03.xbm snow04.xbm snow05.xbm
snowmap.o: snow06.xbm tannenbaum.xbm sleigh1_0.xbm sleigh1_1.xbm
snowmap.o: sleigh1_2.xbm sleigh2_0.xbm sleigh2_1.xbm sleigh2_2.xbm
snowmap.o: sleigh3_0.xbm sleigh3_1.xbm sleigh3_2.xbm santa1.xbm santa2.xbm
snowmap.o: santa3.xbm santa1_fur.xbm santa2_fur.xbm santa3_fur.xbm
vroot.o: /usr/include/X11/X.h /usr/include/X11/Xatom.h
vroot.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
