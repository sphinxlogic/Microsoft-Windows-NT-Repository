# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.65 91/07/25 17:50:17 rws Exp $
#
# The cpp used on this machine replaces all newlines and multiple tabs and
# spaces in a macro expansion with a single space.  Imake tries to compensate
# for this, but is not always successful.
#

# -------------------------------------------------------------------------
# Makefile generated from "Imake.tmpl" and </tmp/IIf.a006Zx>
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

EXTENSIONLIB =
DEPLIBS =
LOCAL_LIBRARIES = $(XLIB)
LINTOPTS=-axzu
# WHY do this? (Greg Lehey, LEMIS, 13 September 1993)
# CC=cc
# CCOPTIONS=

SRCS 	= xcoral.c handle_menus.c init_menus.c cb_names.c new_window.c \
	main_events.c handle_key.c chars_cmds.c shadow.c ctr_panel.c \
	main_text.c scroll.c page.c parse.c get_file.c file_select.c \
	bm_search.c mark_reg.c get_string.c main_buffer.c text_cursor.c \
	kill_buf.c browser_init.c browser_eve.c \
	browser_eng.c browser_pars.c browser_util.c class_dict.c \
	file_dict.c proc_dict.c gen.tab.c lex.yy.c

OBJS	= xcoral.o handle_menus.o init_menus.o cb_names.o new_window.o \
	main_events.o handle_key.o chars_cmds.o shadow.o ctr_panel.o \
	main_text.o scroll.o page.o parse.o get_file.o file_select.o \
	bm_search.o mark_reg.o get_string.o main_buffer.o text_cursor.o \
	kill_buf.o browser_init.o browser_eve.o gen.tab.o lex.yy.o \
	browser_eng.o browser_pars.o browser_util.o class_dict.o \
	file_dict.o proc_dict.o

HACHE	= browser.h config.h options.h browser_eng.h file_dict.h proc_dict.h \
	browser_pars.h flist.h result_types.h browser_util.h gen.tab.h scroll.h \
	buffer.h global_parse.h text.h buttons.h m_key.h xcoral.h class_dict.h menus.h

OFILES	= Copyright FIXES TODO Imakefile Makefile Makefile.std README Resume \
	button.bm gen.l gen.y logo.bm xcoral.man

all:: xcoral

 PROGRAM = xcoral

all:: xcoral

xcoral: $(OBJS) $(DEPLIBS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LDOPTIONS) $(LOCAL_LIBRARIES) $(LDLIBS) $(EXTRA_LOAD_FLAGS)

install:: xcoral
	@if [ -d $(DESTDIR)$(BINDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(DESTDIR)$(BINDIR)); fi
	$(INSTALL) -c $(INSTPGMFLAGS)  xcoral $(DESTDIR)$(BINDIR)

install.man:: xcoral.man
	@if [ -d $(MANDIR) ]; then set +x; \
	else (set -x; $(MKDIRHIER) $(MANDIR)); fi
	$(NROFF) -man xcoral.man > $(MANDIR)/xcoral.$(MANSUFFIX)
	chmod $(MANFLAGS) $(MANDIR)/xcoral.$(MANSUFFIX)

depend::
	$(DEPEND) $(DEPENDFLAGS) -s "# DO NOT DELETE" -- $(ALLDEFINES) $(DEPEND_DEFINES) -- $(SRCS)

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

clean::
	$(RM) $(PROGRAM)

zarch:
	tar cf -  $(OFILES) $(HACHE) $(SRCS) | \
	compress > xcoral-`grep CURRENT_VERSION xcoral.h | awk '{print $$5}'`.tar.Z

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

xcoral.o: /usr/include/stdio.h /usr/include/signal.h
xcoral.o: /usr/include/sys/signal.h /usr/include/sys/types.h
xcoral.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
xcoral.o: /usr/include/string.h /usr/X/include/X11/X.h
xcoral.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
xcoral.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
xcoral.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
xcoral.o: /usr/include/sys/param.h /usr/include/sys/fs/s5param.h options.h
xcoral.o: xcoral.h text.h buffer.h scroll.h buttons.h m_key.h menus.h
xcoral.o: browser.h result_types.h browser_eng.h flist.h
handle_menus.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
handle_menus.o: /usr/X/include/X11/memutil.h /usr/include/string.h
handle_menus.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
handle_menus.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
handle_menus.o: /usr/X/include/X11/Xutil.h menus.h
init_menus.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
init_menus.o: /usr/X/include/X11/memutil.h /usr/include/string.h
init_menus.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
init_menus.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
init_menus.o: /usr/X/include/X11/Xutil.h /usr/include/malloc.h menus.h
cb_names.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
cb_names.o: /usr/X/include/X11/memutil.h /usr/include/string.h
cb_names.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
cb_names.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
cb_names.o: /usr/X/include/X11/Xutil.h options.h xcoral.h text.h buffer.h
cb_names.o: scroll.h buttons.h m_key.h menus.h browser.h result_types.h
cb_names.o: browser_eng.h flist.h
new_window.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
new_window.o: /usr/X/include/X11/memutil.h /usr/include/string.h
new_window.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
new_window.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
new_window.o: /usr/X/include/X11/Xutil.h options.h xcoral.h text.h buffer.h
new_window.o: scroll.h buttons.h m_key.h menus.h browser.h result_types.h
new_window.o: browser_eng.h logo.bm flist.h
main_events.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
main_events.o: /usr/X/include/X11/memutil.h /usr/include/string.h
main_events.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
main_events.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
main_events.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
main_events.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
main_events.o: xcoral.h text.h buffer.h scroll.h buttons.h m_key.h menus.h
main_events.o: browser.h result_types.h browser_eng.h flist.h
handle_key.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
handle_key.o: /usr/X/include/X11/memutil.h /usr/include/string.h
handle_key.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
handle_key.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
handle_key.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
handle_key.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
handle_key.o: /usr/include/ctype.h xcoral.h text.h buffer.h scroll.h
handle_key.o: buttons.h m_key.h menus.h flist.h
chars_cmds.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
chars_cmds.o: /usr/X/include/X11/memutil.h /usr/include/string.h
chars_cmds.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
chars_cmds.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
chars_cmds.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
chars_cmds.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
chars_cmds.o: /usr/include/ctype.h /usr/include/sys/time.h
chars_cmds.o: /usr/include/signal.h /usr/include/sys/signal.h
chars_cmds.o: /usr/include/malloc.h text.h buffer.h scroll.h buttons.h
chars_cmds.o: flist.h
shadow.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
shadow.o: /usr/include/string.h /usr/include/sys/types.h
shadow.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
shadow.o: /usr/X/include/X11/Xosdefs.h /usr/include/stdio.h
ctr_panel.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
ctr_panel.o: /usr/X/include/X11/memutil.h /usr/include/string.h
ctr_panel.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
ctr_panel.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
ctr_panel.o: /usr/X/include/X11/Xutil.h /usr/include/malloc.h text.h buffer.h
ctr_panel.o: scroll.h buttons.h button.bm flist.h
main_text.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
main_text.o: /usr/X/include/X11/memutil.h /usr/include/string.h
main_text.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
main_text.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
main_text.o: /usr/X/include/X11/Xutil.h /usr/include/sys/time.h
main_text.o: /usr/include/malloc.h text.h buffer.h scroll.h buttons.h
main_text.o: browser.h result_types.h browser_eng.h
scroll.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
scroll.o: /usr/X/include/X11/memutil.h /usr/include/string.h
scroll.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
scroll.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
scroll.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
scroll.o: /usr/include/malloc.h scroll.h
page.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
page.o: /usr/include/string.h /usr/include/sys/types.h /usr/X/include/X11/X.h
page.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
page.o: /usr/X/include/X11/Xutil.h /usr/include/stdio.h
page.o: /usr/include/sys/time.h buffer.h text.h scroll.h buttons.h
parse.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
parse.o: /usr/X/include/X11/memutil.h /usr/include/string.h
parse.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
parse.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
parse.o: /usr/X/include/X11/Xresource.h /usr/X/include/X11/cursorfont.h
parse.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/keysym.h
parse.o: /usr/X/include/X11/keysymdef.h /usr/include/pwd.h
parse.o: /usr/include/malloc.h config.h options.h text.h buffer.h scroll.h
parse.o: buttons.h
get_file.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
get_file.o: /usr/X/include/X11/memutil.h /usr/include/string.h
get_file.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
get_file.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
get_file.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
get_file.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
get_file.o: /usr/include/sys/stat.h /usr/include/sys/time.h
get_file.o: /usr/include/sys/file.h /usr/include/sys/param.h
get_file.o: /usr/include/sys/fs/s5param.h /usr/include/sys/unistd.h
get_file.o: /usr/include/sys/x.out.h /usr/include/sys/vnode.h
get_file.o: /usr/include/sys/mac.h /usr/include/sys/uio.h
get_file.o: /usr/include/sys/exec.h /usr/include/malloc.h
get_file.o: /usr/include/errno.h /usr/include/sys/errno.h m_key.h menus.h
get_file.o: buttons.h scroll.h text.h buffer.h browser.h result_types.h
get_file.o: browser_eng.h flist.h
file_select.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
file_select.o: /usr/include/string.h /usr/include/sys/types.h
file_select.o: /usr/X/include/X11/X.h /usr/X/include/X11/Xfuncproto.h
file_select.o: /usr/X/include/X11/Xosdefs.h /usr/include/stdio.h
file_select.o: /usr/include/dirent.h /usr/include/sys/dirent.h
file_select.o: /usr/include/sys/param.h /usr/include/sys/fs/s5param.h
file_select.o: /usr/include/sys/stat.h /usr/include/sys/time.h
file_select.o: /usr/include/malloc.h text.h buffer.h scroll.h buttons.h
file_select.o: options.h flist.h
bm_search.o: /usr/include/stdio.h /usr/include/string.h /usr/include/malloc.h
bm_search.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
bm_search.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
bm_search.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
bm_search.o: text.h buffer.h scroll.h buttons.h flist.h
mark_reg.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
mark_reg.o: /usr/X/include/X11/memutil.h /usr/include/string.h
mark_reg.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
mark_reg.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
mark_reg.o: text.h buffer.h scroll.h buttons.h
get_string.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
get_string.o: /usr/X/include/X11/memutil.h /usr/include/string.h
get_string.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
get_string.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
get_string.o: /usr/X/include/X11/cursorfont.h /usr/X/include/X11/Xutil.h
get_string.o: /usr/X/include/X11/keysym.h /usr/X/include/X11/keysymdef.h
get_string.o: /usr/include/ctype.h /usr/include/sys/stat.h
get_string.o: /usr/include/sys/time.h /usr/include/pwd.h options.h flist.h
main_buffer.o: /usr/include/stdio.h /usr/include/malloc.h
main_buffer.o: /usr/include/string.h buffer.h
text_cursor.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
text_cursor.o: /usr/X/include/X11/memutil.h /usr/include/string.h
text_cursor.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
text_cursor.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
text_cursor.o: /usr/X/include/X11/Xutil.h text.h buffer.h scroll.h buttons.h
kill_buf.o: /usr/include/stdio.h /usr/include/malloc.h /usr/include/string.h
kill_buf.o: /usr/X/include/X11/Xlib.h /usr/X/include/X11/memutil.h
kill_buf.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
kill_buf.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
kill_buf.o: text.h buffer.h scroll.h buttons.h flist.h
browser_init.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
browser_init.o: /usr/X/include/X11/memutil.h /usr/include/string.h
browser_init.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
browser_init.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
browser_init.o: /usr/X/include/X11/Xutil.h /usr/include/dirent.h
browser_init.o: /usr/include/sys/dirent.h /usr/include/sys/stat.h
browser_init.o: /usr/include/sys/time.h /usr/include/sys/param.h
browser_init.o: /usr/include/sys/fs/s5param.h /usr/include/malloc.h options.h
browser_init.o: browser.h text.h buffer.h scroll.h buttons.h result_types.h
browser_init.o: browser_eng.h flist.h
browser_eve.o: /usr/include/stdio.h /usr/X/include/X11/Xlib.h
browser_eve.o: /usr/X/include/X11/memutil.h /usr/include/string.h
browser_eve.o: /usr/include/sys/types.h /usr/X/include/X11/X.h
browser_eve.o: /usr/X/include/X11/Xfuncproto.h /usr/X/include/X11/Xosdefs.h
browser_eve.o: /usr/X/include/X11/Xutil.h /usr/X/include/X11/cursorfont.h
browser_eve.o: /usr/include/malloc.h /usr/include/sys/stat.h
browser_eve.o: /usr/include/sys/time.h options.h browser.h text.h buffer.h
browser_eve.o: scroll.h buttons.h result_types.h browser_eng.h xcoral.h
browser_eve.o: m_key.h menus.h flist.h
browser_eng.o: result_types.h file_dict.h proc_dict.h class_dict.h
browser_eng.o: browser_eng.h browser_pars.h browser_util.h
browser_eng.o: /usr/include/string.h /usr/include/stdio.h
browser_pars.o: result_types.h file_dict.h proc_dict.h class_dict.h
browser_pars.o: browser_pars.h browser_util.h /usr/include/string.h
browser_pars.o: /usr/include/stdio.h
browser_util.o: result_types.h browser_util.h /usr/include/stdio.h
browser_util.o: /usr/include/string.h /usr/include/malloc.h
class_dict.o: result_types.h file_dict.h class_dict.h browser_util.h
class_dict.o: /usr/include/string.h /usr/include/stdio.h
file_dict.o: result_types.h file_dict.h browser_util.h /usr/include/string.h
file_dict.o: /usr/include/stdio.h
proc_dict.o: result_types.h file_dict.h proc_dict.h browser_util.h
proc_dict.o: /usr/include/string.h /usr/include/stdio.h
gen.tab.o: global_parse.h result_types.h file_dict.h proc_dict.h class_dict.h
gen.tab.o: browser_pars.h browser_util.h /usr/include/stdio.h
lex.yy.o: /usr/include/stdio.h global_parse.h gen.tab.h browser_util.h
lex.yy.o: /usr/include/string.h /usr/include/ctype.h
