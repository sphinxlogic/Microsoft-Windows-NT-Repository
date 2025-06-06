# Generated automatically from Makefile.in by configure.
# Makefile for dvips.

version = 5.490s

# Installation directories.
prefix = /opt
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
datadir = $(prefix)/lib
infodir=$(prefix)/info
manext = 1
mandir = $(prefix)/man/man$(manext)

# Where TeX is `rooted'.
texprefix = $(datadir)/tex

# dvips-specific stuff.
dvipsdir = $(texprefix)/dvips

# Default paths for you to override.  You can either change these and
# run `make', or copy include/paths.h-dist to include/paths.h and change
# them manually.  These paths are overridden by various environment
# variables; see the documentation.
default_tfm_path = $(texprefix)/fonts//:.
default_pk_path = $(default_tfm_path)
default_vf_path = $(default_tfm_path)

# Where the configuration files get installed,
# the default path for searching for them.
# and the name of the config file.
configdir = $(dvipsdir)
default_config_path = .:$(configdir)
configfile = config.ps

# Where the PostScript header files go,
# and the default path.  Downloadable fonts are looked for here, also.
headerdir = $(configdir)
default_header_path = $(default_config_path):$(texprefix)/fonts//

# Where the TeX macro files get installed,
# and the default path for searching for epsf and PS files
# (should match the default TeX input path).
texmacros = $(texprefix)/macros
texmacrodir = $(texmacros)/dvips
default_fig_path = .:$(texmacros)//

# What to search for IFF files, etc.
default_pict_path = $(default_fig_path)


# Add -DDEFRES=dpi to DEFS to change the default resolution from 300dpi.
# Add -DDEBUG to turn on debugging.
# Add -DTPIC for tpic support.
# Add -DFONTLIB to search font libraries.
# Add -DEMTEX for EMTEX specials.
# Add -DSECURE if you will install dvips with special privileges.
# 
# For VM/CMS, need -DVMCMS; see the vmcms/mkdvips.exec.
# For VMS, need -DVMS; see the vms subdirectory.
# For DOS, need -DDOS; see the pc subdirectory.
# For Xenix, need -DXENIX; sorry, configure should figure that out.
# 
DEFS = -DTPIC -DDEBUG #-DDEFRES=400

# Perhaps you will want to optimize or use -Wall here.
CFLAGS = -g

# Start of system configuration section.
SHELL = /bin/sh
srcdir = .
VPATH = .

# The user can set either CFLAGS or XCFLAGS on the command line.
CC = gcc -O3

CPPFLAGS = -I. -I$(srcdir) $(DEFS)
.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

# Need -lNeXT_s -lsys_s on the NeXT?
#extralibs= -lNeXT_s -lsys_s
LIBS =  $(extralibs) -lm
LDFLAGS = $(CFLAGS) $(XLDFLAGS)

INSTALL = /opt/bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

MAKEINFO = makeinfo

pathsearch_objects = concat.o concat3.o dir-p.o file-p.o pathsrch.o xmalloc.o \
xopendir.o xrealloc.o xstat.o xstrdup.o

objects = dospecial.o dviinput.o fontdef.o loadfont.o dvips.o tfmload.o \
	download.o prescan.o scanpage.o skippage.o output.o scalewidth.o \
	dosection.o dopage.o resident.o search.o unpack.o drawPS.o \
	header.o makefont.o repack.o virtualfont.o dpicheck.o finclude.o \
	pprescan.o papersiz.o flib.o color.o bbox.o $(pathsearch_objects)

programs = dvips squeeze afm2tfm
prologues = tex.pro texps.pro texc.pro special.pro finclude.pro \
            color.pro crop.pro

all:	$(programs) $(prologues) dvips.info

dvips:	$(objects)
	$(CC) $(LDFLAGS) -o $@ $(objects) $(LIBS)

afm2tfm: afm2tfm.o
	$(CC) $(LDFLAGS) -o $@ afm2tfm.o $(LIBS)


# How to make .pro files from .lpro files:
.SUFFIXES: .pro .lpro
.lpro.pro:
	./squeeze < $< > $@

texc.lpro: texc.script tex.lpro
	$(srcdir)/texc.script tex.lpro texc.lpro

squeeze: squeeze.o
	$(CC) $(LDFLAGS) -o $@ squeeze.o $(LIBS)

dvips.info: dvips.texi
	$(MAKEINFO) $<

paths.h: paths.h.in Makefile.in Makefile
	rm -f paths.h
	echo "/* Generated from paths.h.in (`date`).  */" > paths.h
	sed -e "s,replace-with-pk-path,$(default_pk_path),"		\
            -e "s,replace-with-tfm-path,$(default_tfm_path),"		\
            -e "s,replace-with-vf-path,$(default_vf_path),"		\
            -e "s,replace-with-config-path,$(default_config_path),"	\
            -e "s,replace-with-header-path,$(default_header_path),"	\
            -e "s,replace-with-fig-path,$(default_fig_path),"		\
            -e "s,replace-with-pict-path,$(default_pict_path),"		\
          $(srcdir)/paths.h.in >> paths.h

install: all doc
	-mkdir -p $(prefix) $(exec_prefix) $(datadir) $(texprefix) $(texmacros)\
	$(bindir) $(headerdir) $(configdir) $(texmacrodir) $(mandir) $(infodir)
	$(INSTALL_PROGRAM) dvips $(bindir)/dvips
	$(INSTALL_PROGRAM) afm2tfm $(bindir)/afm2tfm
	$(INSTALL_PROGRAM) $(srcdir)/MakeTeXPK $(bindir)/MakeTeXPK
	for i in dvips.info*; do $(INSTALL_DATA) $$i $(infodir)/$$i; done
	$(INSTALL_DATA) $(srcdir)/tex.pro $(headerdir)/tex.pro
	$(INSTALL_DATA) $(srcdir)/texc.pro $(headerdir)/texc.pro
	$(INSTALL_DATA) $(srcdir)/texps.pro $(headerdir)/texps.pro
	$(INSTALL_DATA) $(srcdir)/special.pro $(headerdir)/special.pro
	$(INSTALL_DATA) $(srcdir)/finclude.pro $(headerdir)/finclude.pro
	$(INSTALL_DATA) $(srcdir)/color.pro $(headerdir)/color.pro
	$(INSTALL_DATA) $(srcdir)/crop.pro $(headerdir)/crop.pro
	$(INSTALL_DATA) $(srcdir)/config.ps $(configdir)/$(configfile)
	$(INSTALL_DATA) $(srcdir)/psfonts.map $(configdir)/psfonts.map
	$(INSTALL_DATA) $(srcdir)/epsf.tex $(texmacrodir)/epsf.tex
	$(INSTALL_DATA) $(srcdir)/epsf.sty $(texmacrodir)/epsf.sty
	$(INSTALL_DATA) $(srcdir)/rotate.tex $(texmacrodir)/rotate.tex
	$(INSTALL_DATA) $(srcdir)/rotate.sty $(texmacrodir)/rotate.sty
	$(INSTALL_DATA) $(srcdir)/colordvi.sty $(texmacrodir)/colordvi.sty
	$(INSTALL_DATA) $(srcdir)/colordvi.tex $(texmacrodir)/colordvi.tex
	$(INSTALL_DATA) $(srcdir)/blackdvi.sty $(texmacrodir)/blackdvi.sty
	$(INSTALL_DATA) $(srcdir)/blackdvi.tex $(texmacrodir)/blackdvi.tex
	$(INSTALL_DATA) $(srcdir)/dvips.1 $(mandir)/dvips.$(manext)
	$(INSTALL_DATA) $(srcdir)/afm2tfm.1 $(mandir)/afm2tfm.$(manext)

doc:	dvips.texi
	tex dvips.texi

distdir = dvipsk-$(version)
dist: dvips.info
	rm -rf $(distdir)
	mkdir $(distdir)
	ln CHANGES INSTALLATION README $(distdir)
	ln configure config.??* *.in $(distdir)
	ln *.sty *.tex *.texi *.info* *.lpro psfonts.map $(distdir)
	ln *.1 texc.script *.c *.h $(distdir)
	cp -p $(plain)/texinfo.tex $(distdir)
	cp -p $(local)/bin/MakeTeXPK $(distdir)
	cp -p $(gnu)/lib/aclocal.m4 $(distdir)
	cp $(ktex)/fontname/adobe.txt $(distdir)/adobe
	for d in contrib pc reencode vmcms vms; do mv $${d} $(distdir); done
	cd $(distdir); rm -f c-auto.h paths.h config.status
	tar czf $(distdir).tar.Z $(distdir)
	for d in contrib pc reencode vmcms vms; do mv $(distdir)/$${d} .; done
	rm -rf $(distdir)

# Prevent GNU make 3 from overflowing arg limit on system V.
.NOEXPORT:

Makefile: Makefile.in 
	sh config.status

configure: configure.in
	autoconf

depend: paths.h
	$(CC) -MM *.c > /tmp/depend

TAGS:
	etags -t *.c *.h

mostlyclean:
	rm -f *.o $(programs)

clean: mostlyclean
	rm -f *.dvi *.log *gf *.tfm *.pl
	rm -f *.pro texc.lpro

distclean: clean
	rm -f *~ *# core a.out
	rm -f Makefile paths.h c-auto.h config.status

realclean: distclean
	rm -f TAGS dvips.info*

# Dependencies.
afm2tfm.o : afm2tfm.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
bbox.o : bbox.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
color.o : color.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
concat.o : concat.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
concat3.o : concat3.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
dir-p.o : dir-p.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h dirio.h xstat.h 
dopage.o : dopage.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
dosection.o : dosection.c structures.h config.h c-auto.h c-std.h c-memstr.h \
  c-errno.h c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h \
  debug.h 
dospecial.o : dospecial.c structures.h config.h c-auto.h c-std.h c-memstr.h \
  c-errno.h c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h \
  debug.h 
download.o : download.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
dpicheck.o : dpicheck.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
drawPS.o : drawPS.c 
dviinput.o : dviinput.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
dvips.o : dvips.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
file-p.o : file-p.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h xstat.h 
finclude.o : finclude.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
flib.o : flib.c 
fontdef.o : fontdef.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
header.o : header.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
loadfont.o : loadfont.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h \
  c-pathmx.h 
makefont.o : makefont.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
output.o : output.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
papersiz.o : papersiz.c 
pathsrch.o : pathsrch.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h c-pathch.h c-namemx.h \
  c-pathmx.h paths.h xstat.h dirio.h pathsrch.h 
pprescan.o : pprescan.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
prescan.o : prescan.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
repack.o : repack.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
resident.o : resident.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h \
  pathsrch.h 
scalewidth.o : scalewidth.c structures.h config.h c-auto.h c-std.h c-memstr.h \
  c-errno.h c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h \
  debug.h 
scanpage.o : scanpage.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
search.o : search.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h \
  pathsrch.h 
skippage.o : skippage.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
squeeze.o : squeeze.c 
tfmload.o : tfmload.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h \
  c-pathmx.h 
unpack.o : unpack.c structures.h config.h c-auto.h c-std.h c-memstr.h c-errno.h \
  c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h debug.h 
virtualfont.o : virtualfont.c structures.h config.h c-auto.h c-std.h c-memstr.h \
  c-errno.h c-minmax.h c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h paths.h \
  debug.h c-pathmx.h 
xmalloc.o : xmalloc.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
xopendir.o : xopendir.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h dirio.h 
xrealloc.o : xrealloc.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
xstat.o : xstat.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h xstat.h 
xstrdup.o : xstrdup.c config.h c-auto.h c-std.h c-memstr.h c-errno.h c-minmax.h \
  c-limits.h c-seek.h c-fopen.h c-proto.h types.h lib.h 
