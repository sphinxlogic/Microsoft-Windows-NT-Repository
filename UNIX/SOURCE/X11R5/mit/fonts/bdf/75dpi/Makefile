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
      CURRENT_DIR = ./fonts/bdf/75dpi

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

# $XConsortium: Imakefile,v 1.18 91/07/16 23:03:27 gildea Exp $
OBJS1 =	courB08.pcf.Z courB10.pcf.Z courB12.pcf.Z courB14.pcf.Z \
	courB18.pcf.Z courB24.pcf.Z courBO08.pcf.Z \
	courBO10.pcf.Z courBO12.pcf.Z courBO14.pcf.Z \
	courBO18.pcf.Z courBO24.pcf.Z courO08.pcf.Z \
	courO10.pcf.Z courO12.pcf.Z courO14.pcf.Z courO18.pcf.Z \
	courO24.pcf.Z courR08.pcf.Z courR10.pcf.Z courR12.pcf.Z \
	courR14.pcf.Z courR18.pcf.Z courR24.pcf.Z helvB08.pcf.Z \
	helvB10.pcf.Z helvB12.pcf.Z helvB14.pcf.Z helvB18.pcf.Z \
	helvB24.pcf.Z helvBO08.pcf.Z helvBO10.pcf.Z \
	helvBO12.pcf.Z helvBO14.pcf.Z helvBO18.pcf.Z \
	helvBO24.pcf.Z helvO08.pcf.Z helvO10.pcf.Z helvO12.pcf.Z \
	helvO14.pcf.Z helvO18.pcf.Z helvO24.pcf.Z helvR08.pcf.Z \
	helvR10.pcf.Z helvR12.pcf.Z helvR14.pcf.Z helvR18.pcf.Z \
	helvR24.pcf.Z ncenB08.pcf.Z ncenB10.pcf.Z ncenB12.pcf.Z \
	ncenB14.pcf.Z ncenB18.pcf.Z ncenB24.pcf.Z ncenBI08.pcf.Z \
	ncenBI10.pcf.Z ncenBI12.pcf.Z ncenBI14.pcf.Z \
	ncenBI18.pcf.Z ncenBI24.pcf.Z ncenI08.pcf.Z \
	ncenI10.pcf.Z ncenI12.pcf.Z ncenI14.pcf.Z ncenI18.pcf.Z \
	ncenI24.pcf.Z ncenR08.pcf.Z ncenR10.pcf.Z ncenR12.pcf.Z \
	ncenR14.pcf.Z ncenR18.pcf.Z ncenR24.pcf.Z symb08.pcf.Z \
	symb10.pcf.Z symb12.pcf.Z symb14.pcf.Z symb18.pcf.Z \
	symb24.pcf.Z timB08.pcf.Z timB10.pcf.Z timB12.pcf.Z \
	timB14.pcf.Z timB18.pcf.Z timB24.pcf.Z timBI08.pcf.Z \
	timBI10.pcf.Z timBI12.pcf.Z timBI14.pcf.Z timBI18.pcf.Z \
	timBI24.pcf.Z timI08.pcf.Z timI10.pcf.Z timI12.pcf.Z \
	timI14.pcf.Z timI18.pcf.Z timI24.pcf.Z timR08.pcf.Z \
	timR10.pcf.Z timR12.pcf.Z timR14.pcf.Z timR18.pcf.Z \
	timR24.pcf.Z

OBJS2 = charB08.pcf.Z charB10.pcf.Z charB12.pcf.Z \
	charB14.pcf.Z charB18.pcf.Z charB24.pcf.Z charBI08.pcf.Z \
	charBI10.pcf.Z charBI12.pcf.Z charBI14.pcf.Z \
	charBI18.pcf.Z charBI24.pcf.Z charI08.pcf.Z \
	charI10.pcf.Z charI12.pcf.Z charI14.pcf.Z charI18.pcf.Z \
	charI24.pcf.Z charR08.pcf.Z charR10.pcf.Z charR12.pcf.Z \
	charR14.pcf.Z charR18.pcf.Z charR24.pcf.Z tech14.pcf.Z \
	techB14.pcf.Z term14.pcf.Z termB14.pcf.Z luBIS08.pcf.Z \
	luBIS10.pcf.Z luBIS12.pcf.Z luBIS14.pcf.Z luBIS18.pcf.Z \
	luBIS24.pcf.Z luBS08.pcf.Z luBS10.pcf.Z luBS12.pcf.Z \
	luBS14.pcf.Z luBS18.pcf.Z luBS24.pcf.Z luIS08.pcf.Z \
	luIS10.pcf.Z luIS12.pcf.Z luIS14.pcf.Z luIS18.pcf.Z \
	luIS24.pcf.Z luRS08.pcf.Z luRS10.pcf.Z luRS12.pcf.Z \
	luRS14.pcf.Z luRS18.pcf.Z luRS24.pcf.Z lubB08.pcf.Z \
	lubB10.pcf.Z lubB12.pcf.Z lubB14.pcf.Z lubB18.pcf.Z \
	lubB24.pcf.Z lubBI08.pcf.Z lubBI10.pcf.Z lubBI12.pcf.Z \
	lubBI14.pcf.Z lubBI18.pcf.Z lubBI24.pcf.Z lubI08.pcf.Z \
	lubI10.pcf.Z lubI12.pcf.Z lubI14.pcf.Z lubI18.pcf.Z \
	lubI24.pcf.Z lubR08.pcf.Z lubR10.pcf.Z lubR12.pcf.Z \
	lubR14.pcf.Z lubR18.pcf.Z lubR24.pcf.Z lutBS08.pcf.Z \
	lutBS10.pcf.Z lutBS12.pcf.Z lutBS14.pcf.Z lutBS18.pcf.Z \
	lutBS24.pcf.Z lutRS08.pcf.Z lutRS10.pcf.Z lutRS12.pcf.Z \
	lutRS14.pcf.Z lutRS18.pcf.Z lutRS24.pcf.Z luBIS19.pcf.Z \
	luBS19.pcf.Z luIS19.pcf.Z luRS19.pcf.Z lubB19.pcf.Z \
	lubBI19.pcf.Z lubI19.pcf.Z lubR19.pcf.Z lutBS19.pcf.Z \
	lutRS19.pcf.Z

OBJS = $(OBJS1) $(OBJS2)

FONTINSTDIR = $(FONTDIR)/75dpi

all:: $(OBJS)

all:: fonts.dir

fonts.dir: $(OBJS)
	$(MKFONTDIR) .

clean::
	$(RM) fonts.dir

clean::
	$(RM) *.snf *.snf.Z *.pcf *.pcf.Z

courB08.pcf.Z: courB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courB10.pcf.Z: courB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courB12.pcf.Z: courB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courB14.pcf.Z: courB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courB18.pcf.Z: courB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courB24.pcf.Z: courB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO08.pcf.Z: courBO08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO10.pcf.Z: courBO10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO12.pcf.Z: courBO12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO14.pcf.Z: courBO14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO18.pcf.Z: courBO18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courBO24.pcf.Z: courBO24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO08.pcf.Z: courO08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO10.pcf.Z: courO10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO12.pcf.Z: courO12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO14.pcf.Z: courO14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO18.pcf.Z: courO18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courO24.pcf.Z: courO24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR08.pcf.Z: courR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR10.pcf.Z: courR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR12.pcf.Z: courR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR14.pcf.Z: courR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR18.pcf.Z: courR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

courR24.pcf.Z: courR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB08.pcf.Z: helvB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB10.pcf.Z: helvB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB12.pcf.Z: helvB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB14.pcf.Z: helvB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB18.pcf.Z: helvB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvB24.pcf.Z: helvB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO08.pcf.Z: helvBO08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO10.pcf.Z: helvBO10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO12.pcf.Z: helvBO12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO14.pcf.Z: helvBO14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO18.pcf.Z: helvBO18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvBO24.pcf.Z: helvBO24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO08.pcf.Z: helvO08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO10.pcf.Z: helvO10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO12.pcf.Z: helvO12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO14.pcf.Z: helvO14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO18.pcf.Z: helvO18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvO24.pcf.Z: helvO24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR08.pcf.Z: helvR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR10.pcf.Z: helvR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR12.pcf.Z: helvR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR14.pcf.Z: helvR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR18.pcf.Z: helvR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

helvR24.pcf.Z: helvR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB08.pcf.Z: ncenB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB10.pcf.Z: ncenB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB12.pcf.Z: ncenB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB14.pcf.Z: ncenB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB18.pcf.Z: ncenB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenB24.pcf.Z: ncenB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI08.pcf.Z: ncenBI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI10.pcf.Z: ncenBI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI12.pcf.Z: ncenBI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI14.pcf.Z: ncenBI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI18.pcf.Z: ncenBI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenBI24.pcf.Z: ncenBI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI08.pcf.Z: ncenI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI10.pcf.Z: ncenI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI12.pcf.Z: ncenI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI14.pcf.Z: ncenI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI18.pcf.Z: ncenI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenI24.pcf.Z: ncenI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR08.pcf.Z: ncenR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR10.pcf.Z: ncenR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR12.pcf.Z: ncenR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR14.pcf.Z: ncenR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR18.pcf.Z: ncenR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

ncenR24.pcf.Z: ncenR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb08.pcf.Z: symb08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb10.pcf.Z: symb10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb12.pcf.Z: symb12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb14.pcf.Z: symb14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb18.pcf.Z: symb18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

symb24.pcf.Z: symb24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB08.pcf.Z: timB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB10.pcf.Z: timB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB12.pcf.Z: timB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB14.pcf.Z: timB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB18.pcf.Z: timB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timB24.pcf.Z: timB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI08.pcf.Z: timBI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI10.pcf.Z: timBI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI12.pcf.Z: timBI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI14.pcf.Z: timBI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI18.pcf.Z: timBI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timBI24.pcf.Z: timBI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI08.pcf.Z: timI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI10.pcf.Z: timI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI12.pcf.Z: timI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI14.pcf.Z: timI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI18.pcf.Z: timI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timI24.pcf.Z: timI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR08.pcf.Z: timR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR10.pcf.Z: timR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR12.pcf.Z: timR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR14.pcf.Z: timR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR18.pcf.Z: timR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

timR24.pcf.Z: timR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB08.pcf.Z: charB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB10.pcf.Z: charB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB12.pcf.Z: charB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB14.pcf.Z: charB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB18.pcf.Z: charB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charB24.pcf.Z: charB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI08.pcf.Z: charBI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI10.pcf.Z: charBI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI12.pcf.Z: charBI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI14.pcf.Z: charBI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI18.pcf.Z: charBI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charBI24.pcf.Z: charBI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI08.pcf.Z: charI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI10.pcf.Z: charI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI12.pcf.Z: charI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI14.pcf.Z: charI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI18.pcf.Z: charI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charI24.pcf.Z: charI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR08.pcf.Z: charR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR10.pcf.Z: charR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR12.pcf.Z: charR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR14.pcf.Z: charR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR18.pcf.Z: charR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

charR24.pcf.Z: charR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

tech14.pcf.Z: tech14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

techB14.pcf.Z: techB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

term14.pcf.Z: term14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

termB14.pcf.Z: termB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS08.pcf.Z: luBIS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS10.pcf.Z: luBIS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS12.pcf.Z: luBIS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS14.pcf.Z: luBIS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS18.pcf.Z: luBIS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS19.pcf.Z: luBIS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBIS24.pcf.Z: luBIS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS08.pcf.Z: luBS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS10.pcf.Z: luBS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS12.pcf.Z: luBS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS14.pcf.Z: luBS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS18.pcf.Z: luBS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS19.pcf.Z: luBS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luBS24.pcf.Z: luBS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS08.pcf.Z: luIS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS10.pcf.Z: luIS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS12.pcf.Z: luIS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS14.pcf.Z: luIS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS18.pcf.Z: luIS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS19.pcf.Z: luIS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luIS24.pcf.Z: luIS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS08.pcf.Z: luRS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS10.pcf.Z: luRS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS12.pcf.Z: luRS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS14.pcf.Z: luRS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS18.pcf.Z: luRS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS19.pcf.Z: luRS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

luRS24.pcf.Z: luRS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB08.pcf.Z: lubB08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB10.pcf.Z: lubB10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB12.pcf.Z: lubB12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB14.pcf.Z: lubB14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB18.pcf.Z: lubB18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB19.pcf.Z: lubB19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubB24.pcf.Z: lubB24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI08.pcf.Z: lubBI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI10.pcf.Z: lubBI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI12.pcf.Z: lubBI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI14.pcf.Z: lubBI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI18.pcf.Z: lubBI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI19.pcf.Z: lubBI19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubBI24.pcf.Z: lubBI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI08.pcf.Z: lubI08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI10.pcf.Z: lubI10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI12.pcf.Z: lubI12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI14.pcf.Z: lubI14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI18.pcf.Z: lubI18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI19.pcf.Z: lubI19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubI24.pcf.Z: lubI24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR08.pcf.Z: lubR08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR10.pcf.Z: lubR10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR12.pcf.Z: lubR12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR14.pcf.Z: lubR14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR18.pcf.Z: lubR18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR19.pcf.Z: lubR19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lubR24.pcf.Z: lubR24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS08.pcf.Z: lutBS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS10.pcf.Z: lutBS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS12.pcf.Z: lutBS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS14.pcf.Z: lutBS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS18.pcf.Z: lutBS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS19.pcf.Z: lutBS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutBS24.pcf.Z: lutBS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS08.pcf.Z: lutRS08.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS10.pcf.Z: lutRS10.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS12.pcf.Z: lutRS12.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS14.pcf.Z: lutRS14.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS18.pcf.Z: lutRS18.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS19.pcf.Z: lutRS19.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

lutRS24.pcf.Z: lutRS24.bdf
	$(FONTC) $(FONTCFLAGS) $? | $(COMPRESS) > $@

install:: $(OBJS1)
	@if [ -d $(DESTDIR)$(FONTINSTDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(FONTINSTDIR)); fi
	@case '${MFLAGS}' in *[i]*) set +e;; esac; \
	for i in $(OBJS1); do \
	(set -x; $(INSTALL) -c $(INSTDATFLAGS) $$i $(DESTDIR)$(FONTINSTDIR)); \
	done

install:: fonts.dir
	@if [ -d $(DESTDIR)$(FONTINSTDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(FONTINSTDIR)); fi
	$(MKFONTDIR) $(DESTDIR)$(FONTINSTDIR)

install:: $(OBJS2)
	@if [ -d $(DESTDIR)$(FONTINSTDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(FONTINSTDIR)); fi
	@case '${MFLAGS}' in *[i]*) set +e;; esac; \
	for i in $(OBJS2); do \
	(set -x; $(INSTALL) -c $(INSTDATFLAGS) $$i $(DESTDIR)$(FONTINSTDIR)); \
	done

install:: fonts.dir
	@if [ -d $(DESTDIR)$(FONTINSTDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(FONTINSTDIR)); fi
	$(MKFONTDIR) $(DESTDIR)$(FONTINSTDIR)

install:: fonts.alias
	@if [ -d $(DESTDIR)$(FONTINSTDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(FONTINSTDIR)); fi
	$(INSTALL) -c $(INSTDATFLAGS) fonts.alias $(DESTDIR)$(FONTINSTDIR)

depend::

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

