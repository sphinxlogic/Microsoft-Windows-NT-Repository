# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.65 91/07/25 17:50:17 rws Exp $
#
# The cpp used on this machine replaces all newlines and multiple tabs and
# spaces in a macro expansion with a single space.  Imake tries to compensate
# for this, but is not always successful.
#

# -------------------------------------------------------------------------
# Makefile generated from "Imake.tmpl" and <Imakefile>
# $XFree86: mit/config/Imake.tmpl,v 2.6 1993/10/02 07:11:59 dawes Exp $
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

# $XFree86: mit/config/site.def,v 2.23 1993/10/16 17:28:07 dawes Exp $
# site:  $XConsortium: site.def,v 1.2 91/07/30 20:26:44 rws Exp $

# -------------------------------------------------------------------------
# platform-specific configuration parameters - edit x386.cf to change

# $XFree86: mit/config/x386.cf,v 2.28 1993/10/21 15:48:52 dawes Exp $
# platform:  $XConsortium: x386.cf,v 1.7 91/08/16 19:30:10 gildea Exp $

# -------------------------------------------------------------------------
# XFree86 version definition
# $XFree86: mit/config/xf86_vers.def,v 2.7 1993/10/08 16:11:07 dawes Exp $

# -------------------------------------------------------------------------
# XFree86 version: 2000
# -------------------------------------------------------------------------

# $XFree86: mit/config/sv4Lib.rules,v 1.7 1993/03/27 08:55:08 dawes Exp $
# $XConsortium: sv4Lib.rules,v 1.8 91/07/19 15:38:53 rws Exp $

#  --------------------------------------------------------------------
#  X386Conf.tmpl  -- configuration parameters for X386
#  This file is automatically generated -- DO NOT EDIT
#  --------------------------------------------------------------------

         X386SRC = $(SERVERSRC)/ddx/x386
    VGADRIVERSRC = $(X386SRC)/vga256/drivers
  VGA16DRIVERSRC = $(X386SRC)/vga16/drivers
   VGA2DRIVERSRC = $(X386SRC)/vga2/drivers
   HGA2DRIVERSRC = $(X386SRC)/hga2/drivers
   BDM2DRIVERSRC = $(X386SRC)/bdm2/drivers
     S3DRIVERSRC = $(X386SRC)/accel/s3/drivers
     RENDERERSRC = $(FONTSRC)/lib/font
 RENDERERCONFOBJ = $(RENDERERSRC)/fontfile/rendererConf.o
RENDERERCONFSUBDIR = $(RENDERERSRC)/fontfile
         VGAOBJS = \
                   $(VGADRIVERSRC)/et4000/et4000.o \
                   $(VGADRIVERSRC)/et3000/et3000.o \
                   $(VGADRIVERSRC)/pvga1/pvga1.o \
                   $(VGADRIVERSRC)/gvga/gvga.o \
                   $(VGADRIVERSRC)/ati/ati.o \
                   $(VGADRIVERSRC)/tvga8900/tvga8900.o \
                   $(VGADRIVERSRC)/cirrus/cirrus.o \
                   $(VGADRIVERSRC)/ncr/ncr.o \
                   $(VGADRIVERSRC)/compaq/compaq.o \
                   $(VGADRIVERSRC)/oak/oak.o
      VGASUBDIRS = \
                   et4000 \
                   et3000 \
                   pvga1 \
                   gvga \
                   ati \
                   tvga8900 \
                   cirrus \
                   ncr \
                   compaq \
                   oak
        VGA16OBJS = \
                   $(VGA16DRIVERSRC)/et4000/et4000.o \
                   $(VGA16DRIVERSRC)/generic/generic.o
     VGA16SUBDIRS = \
                   et4000 \
                   generic
        VGA2OBJS = \
                   $(VGA2DRIVERSRC)/et4000/et4000.o \
                   $(VGA2DRIVERSRC)/et3000/et3000.o \
                   $(VGA2DRIVERSRC)/pvga1/pvga1.o \
                   $(VGA2DRIVERSRC)/gvga/gvga.o \
                   $(VGA2DRIVERSRC)/tvga8900/tvga8900.o \
                   $(VGA2DRIVERSRC)/ncr/ncr.o \
                   $(VGA2DRIVERSRC)/compaq/compaq.o \
                   $(VGA2DRIVERSRC)/oak/oak.o \
                   $(VGA2DRIVERSRC)/generic/generic.o
     VGA2SUBDIRS = \
                   et4000 \
                   et3000 \
                   pvga1 \
                   gvga \
                   tvga8900 \
                   ncr \
                   compaq \
                   oak \
                   generic
        HGA2OBJS =
     HGA2SUBDIRS =
        BDM2OBJS =
     BDM2SUBDIRS =
        S3OBJS = \
                   $(S3DRIVERSRC)/mmio_928/mmio_928.o \
                   $(S3DRIVERSRC)/s3_generic/s3_generic.o
     S3SUBDIRS = \
                   mmio_928 \
                   s3_generic
    RENDERERLIBS = \
                   $(RENDERERSRC)/Speedo/libSpeedo.a \
                   $(RENDERERSRC)/Type1/libType1.a
 RENDERERSUBDIRS = \
                   Speedo \
                   Type1

     VGA2CONFOBJ = $(X386SRC)/vga2/vga2Conf.o
  VGA2CONFSUBDIR = $(X386SRC)/vga2
       VGA2BUILD = -DBUILD_VGA2

   X386MCONFOBJS = $(VGA2CONFOBJ) $(HGA2CONFOBJ) $(BDM2CONFOBJ)
X386MCONFSUBDIRS = $(VGA2CONFSUBDIR) $(HGA2CONFSUBDIR) $(BDM2CONFSUBDIR)

    VGA16CONFOBJ = $(X386SRC)/vga16/vga16Conf.o
 VGA16CONFSUBDIR = $(X386SRC)/vga16
      VGA16BUILD = -DBUILD_VGA16

   VGA16CONFOBJS = $(VGA16CONFOBJ)
VGA16CONFSUBDIRS = $(VGA16CONFSUBDIR)

      VGACONFOBJ = $(X386SRC)/vga256/vga256Conf.o
   VGACONFSUBDIR = $(X386SRC)/vga256
        VGABUILD = -DBUILD_VGA256

    X386CONFOBJS = $(VGACONFOBJ)
 X386CONFSUBDIRS = $(VGACONFSUBDIR)

       S3CONFOBJ = $(X386SRC)/accel/s3/s3Conf.o
    S3CONFSUBDIR = $(X386SRC)/accel/s3

 X386SCREENFLAGS = $(VGA2BUILD) $(HGA2BUILD) $(BDM2BUILD) $(VGA16BUILD) $(VGABUILD)

# operating system:  Unix System V/386 Release 4.2 Consensys Corp.

# -------------------------------------------------------------------------
# site-specific configuration parameters that go after
# the platform-specific parameters - edit site.def to change

# $XFree86: mit/config/site.def,v 2.23 1993/10/16 17:28:07 dawes Exp $
# site:  $XConsortium: site.def,v 1.2 91/07/30 20:26:44 rws Exp $

# Definitions from site.def

          DEFINES = -DBSD_COMP

# Add these paths to find GNU ndbm
   EXTRA_INCLUDES = -I/opt/include
           DBMLIB = -L/opt/lib -lgdbm

# End of definitions from site.def

            SHELL = /bin/sh

              TOP = ../../../.
      CURRENT_DIR = ./fonts/server/difs

               AR = ar cq
  -DSVR4 -DSYSV386 =
               CC = gcc
               AS = as

             YACC = yacc

         COMPRESS = compress
              CPP = /lib/cpp $(STD_CPP_DEFINES)
    PREPROCESSCMD = gcc -E $(STD_CPP_DEFINES)
          INSTALL = $(SCRIPTSRC)/bsdinst.sh
               LD = /bin/ld
             LINT = lint
      LINTLIBFLAG = -o
         LINTOPTS = -ax
               LN = ln -s
             MAKE = make
               MV = mv
               CP = cp

               RM = rm -f
            TROFF = psroff
         MSMACROS = -ms
              TBL = tbl
              EQN = eqn
            NROFF = nroff
             NEQN = neqn
     STD_INCLUDES =
  STD_CPP_DEFINES =
      STD_DEFINES =
 EXTRA_LOAD_FLAGS =
  EXTRA_LIBRARIES = -lsocket -lnsl
     OS_LIBRARIES =
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

      PROJECTROOT = /usr/X11R5

     TOP_INCLUDES = -I$(TOP)

      CDEBUGFLAGS = -O3 -g
        CCOPTIONS = -m486 -fpcc-struct-return -Di386
    ANSICCOPTIONS =

      ALLINCLUDES = $(INCLUDES) $(EXTRA_INCLUDES) $(TOP_INCLUDES) $(STD_INCLUDES)
       ALLDEFINES = $(ALLINCLUDES) $(STD_DEFINES) $(EXTRA_DEFINES) $(PROTO_DEFINES) $(DEFINES)
           CFLAGS = $(ANSICCOPTIONS) $(CDEBUGFLAGS) $(CCOPTIONS) $(ALLDEFINES)
        LINTFLAGS = $(LINTOPTS) -DLINT $(ALLDEFINES)

           LDLIBS = -L$(USRLIBDIR) $(OS_LIBRARIES) $(SYS_LIBRARIES) $(EXTRA_LIBRARIES)

        LDOPTIONS = $(ANSICCOPTIONS) $(CDEBUGFLAGS) $(CCOPTIONS) $(LOCAL_LDFLAGS)

   LDCOMBINEFLAGS = -r
      DEPENDFLAGS =

        MACROFILE = x386.cf
           RM_CMD = $(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut

    IMAKE_DEFINES =

         IRULESRC = $(CONFIGSRC)
        IMAKE_CMD = $(NEWTOP)$(IMAKE) -I$(NEWTOP)$(IRULESRC) $(IMAKE_DEFINES)

     ICONFIGFILES = $(IRULESRC)/Imake.tmpl $(IRULESRC)/Imake.rules \
			$(IRULESRC)/Project.tmpl $(IRULESRC)/site.def \
			$(IRULESRC)/$(MACROFILE) $(EXTRA_ICONFIGFILES)

# -------------------------------------------------------------------------
# X Window System Build Parameters
# $XFree86: mit/config/Project.tmpl,v 2.1 1993/09/17 16:10:43 dawes Exp $
# $XConsortium: Project.tmpl,v 1.138.1.1 92/11/11 09:49:19 rws Exp $

_percentC_ = %C

# -------------------------------------------------------------------------
# X Window System make variables; this need to be coordinated with rules

          PATHSEP = /
        USRLIBDIR = /usr/X11R5/lib
           BINDIR = /usr/X11R5/bin
          INCROOT = /usr/X11R5/include
     BUILDINCROOT = $(TOP)
      BUILDINCDIR = $(BUILDINCROOT)/X11
      BUILDINCTOP = ..
           INCDIR = $(INCROOT)/X11
           ADMDIR = /usr/adm
           LIBDIR = $(USRLIBDIR)/X11
        CONFIGDIR = $(LIBDIR)/config
       LINTLIBDIR = $(USRLIBDIR)/lint

          FONTDIR = $(LIBDIR)/fonts
         XINITDIR = $(LIBDIR)/xinit
           XDMDIR = $(LIBDIR)/xdm
           TWMDIR = $(LIBDIR)/twm
          MANPATH = /usr/X11R5/man
    MANSOURCEPATH = $(MANPATH)/man
        MANSUFFIX = 1x
     LIBMANSUFFIX = 3x
    FILEMANSUFFIX = 4
           MANDIR = $(MANSOURCEPATH)1
        LIBMANDIR = $(MANSOURCEPATH)3
       FILEMANDIR = $(MANSOURCEPATH)4
           NLSDIR = $(LIBDIR)/nls
        PEXAPIDIR = $(LIBDIR)/PEX
      XAPPLOADDIR = $(LIBDIR)/app-defaults
       FONTCFLAGS = -t
       LINKKITDIR = $(USRLIBDIR)/Server

     INSTAPPFLAGS = $(INSTDATFLAGS)

            IMAKE = $(IMAKESRC)/imake
           DEPEND = $(DEPENDSRC)/makedepend
              RGB = $(RGBSRC)/rgb

            FONTC = $(BDFTOPCFSRC)/bdftopcf

        MKFONTDIR = $(MKFONTDIRSRC)/mkfontdir
        MKDIRHIER = /bin/sh $(SCRIPTSRC)/mkdirhier.sh

        CONFIGSRC = $(TOP)/config
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
      BDFTOSNFSRC = $(FONTSRC)/bdftosnf
      BDFTOSNFSRC = $(FONTSRC)/clients/bdftosnf
      BDFTOPCFSRC = $(FONTSRC)/clients/bdftopcf
     MKFONTDIRSRC = $(FONTSRC)/clients/mkfontdir
         FSLIBSRC = $(FONTSRC)/lib/fs
    FONTSERVERSRC = $(FONTSRC)/server
     EXTENSIONSRC = $(TOP)/extensions
         XILIBSRC = $(EXTENSIONSRC)/lib/xinput
        PEXLIBSRC = $(EXTENSIONSRC)/lib/PEXlib
      PHIGSLIBSRC = $(EXTENSIONSRC)/lib/PEX

# $XConsortium: sv4Lib.tmpl,v 1.4.1.1 92/11/11 09:57:17 rws Exp $

SHLIBLDFLAGS = -G -z text
PICFLAGS = -fPIC

  DEPEXTENSIONLIB =
     EXTENSIONLIB = -L$(EXTENSIONSRC)/lib -lXext

          DEPXLIB = $(DEPEXTENSIONLIB)
             XLIB = $(EXTENSIONLIB) -L$(XLIBSRC) -lX11

        DEPXMULIB =
       XMULIBONLY = -L$(XMUSRC) -lXmu
           XMULIB = $(XMULIBONLY) -z nodefs

       DEPOLDXLIB =
          OLDXLIB = -L$(OLDXLIBSRC) -loldX

      DEPXTOOLLIB =
         XTOOLLIB = -L$(TOOLKITSRC) -lXt

        DEPXAWLIB =
           XAWLIB = -L$(AWIDGETSRC) -lXaw

        DEPXILIB =
           XILIB = -L$(XILIBSRC) -lXi

        DEPPEXLIB =
         PEXLIB = -L$(PEXLIBSRC) -lPEX5

        SOXLIBREV = 5.0
          SOXTREV = 5.0
         SOXAWREV = 5.0
        SOOLDXREV = 5.0
         SOXMUREV = 5.0
        SOXEXTREV = 5.0
      SOXINPUTREV = 5.0
         SOPEXREV = 1.0

      DEPXAUTHLIB = $(XAUTHSRC)/libXau.a
         XAUTHLIB =  $(DEPXAUTHLIB)
      DEPXDMCPLIB = $(XDMCPLIBSRC)/libXdmcp.a
         XDMCPLIB =  $(DEPXDMCPLIB)

        DEPPHIGSLIB = $(PHIGSLIBSRC)/libphigs.a
           PHIGSLIB =  $(DEPPHIGSLIB)

       DEPXBSDLIB = $(LIBSRC)/Berk/libXbsd.a
          XBSDLIB =  $(DEPXBSDLIB)

 LINTEXTENSIONLIB = $(EXTENSIONSRC)/lib/llib-lXext.ln
         LINTXLIB = $(XLIBSRC)/llib-lX11.ln
          LINTXMU = $(XMUSRC)/llib-lXmu.ln
        LINTXTOOL = $(TOOLKITSRC)/llib-lXt.ln
          LINTXAW = $(AWIDGETSRC)/llib-lXaw.ln
           LINTXI = $(XILIBSRC)/llib-lXi.ln
          LINTPEX = $(PEXLIBSRC)/llib-lPEX5.ln
        LINTPHIGS = $(PHIGSLIBSRC)/llib-lphigs.ln

          DEPLIBS = $(DEPXAWLIB) $(DEPXMULIB) $(DEPXTOOLLIB) $(DEPXLIB)

         DEPLIBS1 = $(DEPLIBS)
         DEPLIBS2 = $(DEPLIBS)
         DEPLIBS3 = $(DEPLIBS)

# -------------------------------------------------------------------------
# Imake rules for building libraries, programs, scripts, and data files
# $XFree86: mit/config/Imake.rules,v 2.1 1993/09/29 11:06:00 dawes Exp $
# rules:  $XConsortium: Imake.rules,v 1.123 91/09/16 20:12:16 rws Exp $

# -------------------------------------------------------------------------
# start of Imakefile

# $XFree86: mit/fonts/server/difs/Imakefile,v 2.0 1993/08/28 07:43:56 dawes Exp $
# $XConsortium: Imakefile,v 1.9 92/05/12 18:08:03 gildea Exp $

# $XFree86: mit/config/Server.tmpl,v 1.3 1993/03/27 08:55:06 dawes Exp $
# $XConsortium: Server.tmpl,v 1.5 91/05/06 19:00:26 rws Exp $

         CC = gcc
  CCOPTIONS = -m486 -fpcc-struct-return -Di386
STD_DEFINES =
CDEBUGFLAGS = -O3 -g
EXT_DEFINES = -DSHAPE -DMULTIBUFFER -DMITMISC  -DMITSHM  -DXTESTEXT1 -DXTEST

SRCS =	main.c dispatch.c extensions.c globals.c events.c tables.c \
	fontinfo.c charinfo.c swapreq.c swaprep.c resource.c \
	fonts.c difsutils.c cache.c initfonts.c atom.c

OBJS =  main.o dispatch.o extensions.o globals.o events.o tables.o \
	fontinfo.o charinfo.o swapreq.o swaprep.o resource.o \
	fonts.o difsutils.o cache.o initfonts.o atom.o

INCLUDES = -I../include -I$(FONTSRC)/include

SPEEDODEFINES = -DFONT_SPEEDO

FONTDEFINES = -DFONT_PCF -DFONT_FS $(SPEEDODEFINES)

DEFAULTCONFIGFILE = $(LIBDIR)/fs/config
CONFIGDEFINES = -DDEFAULT_CONFIG_FILE=\"$(DEFAULTCONFIGFILE)\"

.c.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $(_NOOP_) $*.c

all:: libdifs.a

libdifs.a: $(OBJS)
	$(RM) $@
	$(AR) $@ $(OBJS)

depend:: $(DEPEND)

$(DEPEND):
	@echo "checking $@ over in $(DEPENDSRC) first..."; \
	cd $(DEPENDSRC); $(MAKE); \
	echo "okay, continuing in $(CURRENT_DIR)"

depend::
	$(DEPEND) $(DEPENDFLAGS) -s "# DO NOT DELETE" -- $(ALLDEFINES) -- $(SRCS)

main.o: main.c $(ICONFIGFILES)
	$(RM) $@
	$(CC) -c $(CFLAGS) $(CONFIGDEFINES) $*.c

initfonts.o: initfonts.c $(ICONFIGFILES)
	$(RM) $@
	$(CC) -c $(CFLAGS) $(FONTDEFINES) $*.c

# -------------------------------------------------------------------------
# common rules for all Makefiles - do not edit

emptyrule::

clean::
	$(RM_CMD) "#"*

Makefile:: $(IMAKE)

$(IMAKE):
	@(cd $(IMAKESRC); if [ -f Makefile ]; then \
	echo "checking $@ in $(IMAKESRC) first..."; $(MAKE) all; else \
	echo "bootstrapping $@ from Makefile.ini in $(IMAKESRC) first..."; \
	$(MAKE) -f Makefile.ini -DSVR4 -DSYSV386="$(BOOTSTRAPCFLAGS)"; fi; \
	echo "okay, continuing in $(CURRENT_DIR)")

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

install.linkkit::
	@echo "install.linkkit in $(CURRENT_DIR) done"

Makefiles::

includes::

# -------------------------------------------------------------------------
# dependencies generated by makedepend

# DO NOT DELETE

main.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
main.o: ../../.././fonts/include/FSproto.h ../../.././X11/Xmd.h
main.o: ../include/clientstr.h ../include/client.h ../include/auth.h
main.o: ../include/misc.h ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
main.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
main.o: ../include/resource.h ../include/globals.h ../include/servermd.h
main.o: ../include/cache.h ../include/site.h /usr/include/sys/param.h
main.o: /usr/include/sys/types.h /usr/include/sys/fs/s5param.h
dispatch.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
dispatch.o: ../../.././fonts/include/FSproto.h ../../.././X11/Xmd.h
dispatch.o: ../include/clientstr.h ../include/client.h ../include/auth.h
dispatch.o: ../include/misc.h ../../.././X11/Xosdefs.h
dispatch.o: ../../.././X11/Xfuncs.h ../include/assert.h /usr/include/stddef.h
dispatch.o: ../include/os.h ../include/authstr.h ../include/osstruct.h
dispatch.o: ../include/extentst.h ../include/globals.h ../include/resource.h
dispatch.o: ../include/difsfontst.h ../include/difsfont.h
dispatch.o: ../../.././fonts/include/font.h
dispatch.o: ../../.././fonts/include/fontstruct.h ../../.././X11/Xproto.h
dispatch.o: ../../.././X11/Xprotostr.h ../include/site.h ../include/events.h
dispatch.o: ../include/cache.h
extensions.o: ../../.././fonts/include/FSproto.h
extensions.o: ../../.././fonts/include/FS.h
extensions.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
extensions.o: ../include/misc.h ../../.././X11/Xosdefs.h
extensions.o: ../../.././X11/Xfuncs.h ../include/assert.h
extensions.o: /usr/include/stddef.h ../include/os.h ../include/clientstr.h
extensions.o: ../include/client.h ../include/auth.h ../include/extentst.h
globals.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
globals.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
globals.o: ../include/clientstr.h ../include/client.h ../include/auth.h
globals.o: ../include/misc.h ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
globals.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
globals.o: ../include/site.h
events.o: ../include/clientstr.h ../../.././fonts/include/FS.h
events.o: ../../.././fonts/include/fsmasks.h ../include/client.h
events.o: ../include/auth.h ../include/misc.h ../../.././X11/Xosdefs.h
events.o: ../../.././X11/Xfuncs.h ../include/assert.h /usr/include/stddef.h
events.o: ../include/os.h ../../.././fonts/include/FSproto.h
events.o: ../../.././X11/Xmd.h ../include/globals.h ../include/events.h
tables.o: ../include/globals.h ../../.././fonts/include/FSproto.h
tables.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
tables.o: ../../.././X11/Xmd.h
fontinfo.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
fontinfo.o: ../../.././fonts/include/FSproto.h ../../.././X11/Xmd.h
fontinfo.o: /usr/include/stdio.h ../../.././X11/Xos.h
fontinfo.o: ../../.././X11/Xosdefs.h /usr/include/sys/types.h
fontinfo.o: /usr/include/string.h /usr/include/fcntl.h
fontinfo.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
fontinfo.o: /usr/include/sys/unistd.h /usr/include/sys/time.h
fontinfo.o: ../include/clientstr.h ../include/client.h ../include/auth.h
fontinfo.o: ../include/misc.h ../../.././X11/Xfuncs.h ../include/assert.h
fontinfo.o: /usr/include/stddef.h ../include/os.h ../include/difsfontst.h
fontinfo.o: ../include/difsfont.h ../../.././fonts/include/font.h
fontinfo.o: ../../.././fonts/include/fontstruct.h ../../.././X11/Xproto.h
fontinfo.o: ../../.././X11/Xprotostr.h ../include/closestr.h
fontinfo.o: ../include/closure.h ../include/globals.h
charinfo.o: ../../.././X11/Xos.h ../../.././X11/Xosdefs.h
charinfo.o: /usr/include/sys/types.h /usr/include/string.h
charinfo.o: /usr/include/fcntl.h /usr/include/sys/fcntl.h
charinfo.o: /usr/include/unistd.h /usr/include/sys/unistd.h
charinfo.o: /usr/include/sys/time.h ../include/misc.h ../../.././X11/Xfuncs.h
charinfo.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
charinfo.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
charinfo.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
charinfo.o: ../../.././fonts/include/fontstruct.h ../../.././X11/Xproto.h
charinfo.o: ../../.././X11/Xprotostr.h ../../.././fonts/include/font.h
charinfo.o: ../include/clientstr.h ../include/client.h ../include/auth.h
swapreq.o: ../include/misc.h ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
swapreq.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
swapreq.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
swapreq.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
swapreq.o: ../include/clientstr.h ../include/client.h ../include/auth.h
swapreq.o: ../include/globals.h
swaprep.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
swaprep.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
swaprep.o: ../include/clientstr.h ../include/client.h ../include/auth.h
swaprep.o: ../include/misc.h ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
swaprep.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
resource.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
resource.o: ../include/misc.h ../../.././X11/Xosdefs.h
resource.o: ../../.././X11/Xfuncs.h ../include/assert.h /usr/include/stddef.h
resource.o: ../include/os.h ../../.././fonts/include/FSproto.h
resource.o: ../../.././X11/Xmd.h ../include/resource.h ../include/clientstr.h
resource.o: ../include/client.h ../include/auth.h ../include/globals.h
fonts.o: ../../.././fonts/include/FS.h ../../.././fonts/include/fsmasks.h
fonts.o: ../../.././fonts/include/FSproto.h ../../.././X11/Xmd.h
fonts.o: /usr/include/stdio.h ../../.././X11/Xos.h ../../.././X11/Xosdefs.h
fonts.o: /usr/include/sys/types.h /usr/include/string.h /usr/include/fcntl.h
fonts.o: /usr/include/sys/fcntl.h /usr/include/unistd.h
fonts.o: /usr/include/sys/unistd.h /usr/include/sys/time.h
fonts.o: ../include/clientstr.h ../include/client.h ../include/auth.h
fonts.o: ../include/misc.h ../../.././X11/Xfuncs.h ../include/assert.h
fonts.o: /usr/include/stddef.h ../include/os.h ../include/resource.h
fonts.o: ../include/difsfontst.h ../include/difsfont.h
fonts.o: ../../.././fonts/include/font.h
fonts.o: ../../.././fonts/include/fontstruct.h ../../.././X11/Xproto.h
fonts.o: ../../.././X11/Xprotostr.h ../include/closestr.h
fonts.o: ../include/closure.h ../include/globals.h
difsutils.o: /usr/include/stdio.h /usr/include/ctype.h ../include/misc.h
difsutils.o: ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
difsutils.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
difsutils.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
difsutils.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
difsutils.o: ../include/globals.h ../include/clientstr.h ../include/client.h
difsutils.o: ../include/auth.h ../include/accstr.h ../include/access.h
difsutils.o: ../../.././fonts/include/fontstruct.h ../../.././X11/Xproto.h
difsutils.o: ../../.././X11/Xprotostr.h ../../.././fonts/include/font.h
difsutils.o: ../../.././X11/keysymdef.h
cache.o: ../include/cachestr.h ../include/cache.h ../include/misc.h
cache.o: ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h ../include/assert.h
cache.o: /usr/include/stddef.h ../include/os.h
cache.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
cache.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
initfonts.o: ../../.././fonts/include/font.h
atom.o: ../include/misc.h ../../.././X11/Xosdefs.h ../../.././X11/Xfuncs.h
atom.o: ../include/assert.h /usr/include/stddef.h ../include/os.h
atom.o: ../../.././fonts/include/FSproto.h ../../.././fonts/include/FS.h
atom.o: ../../.././fonts/include/fsmasks.h ../../.././X11/Xmd.h
atom.o: ../include/resource.h
