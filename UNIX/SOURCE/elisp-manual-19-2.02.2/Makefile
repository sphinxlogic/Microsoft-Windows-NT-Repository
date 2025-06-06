# Makefile for the   GNU Emacs Lisp Reference Manual.
#
# 11 August 1990

# Redefine `TEX' if `tex' does not invoke plain TeX. For example:
# TEX=platex

TEX=tex

# Where the TeX macros are kept:
texmacrodir = /opt/lib/tex/macros

# Where the Emacs hierarchy lives ($EMACS in the INSTALL document for Emacs.)
# For example: 
# emacslibdir = /opt/gnu/lib/emacs

# Directory where Emacs is installed, by default:
emacslibdir = /opt/lib/emacs

# Unless you have a nonstandard Emacs installation, these shouldn't have to 
# be changed.
prefix = /opt
infodir = ${prefix}/info

# The name of the manual:

VERSION=2.02.2
manual = elisp-manual-19-$(VERSION)

# Uncomment this line for permuted index.
# permuted_index = 1 

# List of all the texinfo files in the manual:

srcs = elisp.texi back.texi \
  abbrevs.texi anti.texi backups.texi locals.texi buffers.texi \
  calendar.texi commands.texi compile.texi control.texi debugging.texi \
  display.texi errors.texi eval.texi files.texi \
  frames.texi functions.texi help.texi hooks.texi \
  internals.texi intro.texi keymaps.texi lists.texi \
  loading.texi macros.texi maps.texi markers.texi \
  minibuf.texi modes.texi numbers.texi objects.texi \
  os.texi positions.texi processes.texi searching.texi \
  sequences.texi streams.texi strings.texi symbols.texi \
  syntax.texi text.texi tips.texi variables.texi \
  windows.texi \
  index.unperm index.perm

.PHONY: elisp.dvi clean

elisp.dvi: $(srcs) index.texi # texindex
	# Avoid losing old contents of aux file entirely.
	-mv elisp.aux elisp.oaux
	# First shot to define xrefs:
	$(TEX) elisp.texi
	if [ a${permuted_index} != a ]; \
	then \
	  ./permute-index; \
	  mv permuted.fns elisp.fns; \
	else \
	  texindex elisp.??; \
	fi
	$(TEX) elisp.texi

index.texi:
	if [ a${permuted_index} != a ]; \
	then \
	  ln -s index.perm index.texi; \
	else \
	  ln -s index.unperm index.texi; \
	fi

# The info file is named `elisp'.

elisp: $(srcs) index.texi # makeinfo
	makeinfo elisp.texi

install: elisp elisp.dvi
	mv elisp elisp-* $(infodir)
	@echo also add the line for elisp to $(infodir)/dir.

installall: install
	install -c texinfo.tex $(texmacrodir)

clean:
	rm -f *.toc *.aux *.log *.cp *.cps *.fn *.fns *.tp *.tps \
              *.vr *.vrs *.pg *.pgs *.ky *.kys
	rm -f make.out core 
	rm -f makeinfo.o makeinfo getopt.o getopt1.o
	rm -f texindex.o texindex index.texi

dist:
	-mkdir temp
	-mkdir temp/$(manual)
	-ln README Makefile permute-index $(srcs) \
   texinfo.tex getopt.c getopt1.c getopt.h \
   elisp.dvi elisp.aux elisp.??s elisp elisp-[0-9] elisp-[0-9][0-9] temp/$(manual)
	-rm -f temp/$(manual)/texindex.c temp/$(manual)/makeinfo.c
	cp texindex.c makeinfo.c temp/$(manual)
	(cd temp/$(manual); rm -f *~)
	(cd temp; tar chf - $(manual)) | gzip > $(manual).tar.gz
	-rm -rf temp

# Make two programs used in generating output from texinfo.

CFLAGS = -g

texindex: texindex.o
	$(CC) -o $@ $(LDFLAGS) $(CFLAGS) $?
texindex.o: texindex.c

MAKEINFO_MAJOR = 1
MAKEINFO_MINOR = 0
MAKEINFO_FLAGS = -DMAKEINFO_MAJOR=$(MAKEINFO_MAJOR) -DMAKEINFO_MINOR=$(MAKEINFO_MINOR)

makeinfo: makeinfo.o getopt.o getopt1.o
	$(CC) $(LDFLAGS) -o makeinfo makeinfo.o getopt.o getopt1.o

makeinfo.o: makeinfo.c
	$(CC) -c $(CFLAGS) $(MAKEINFO_FLAGS) makeinfo.c
