    #### Makefile for the Para mode distribution -*- text -*- ####

# Makefile for Para mode 0.27        <<change number below as needed>>

# Robert J. Chassell
# bugs to bob@ai.mit.edu
# 18 Apr 91

# A `make' or `make dist' command creates a compressed tar file of the
# experimental Para mode distribution.  The file is called `para.tar.Z'.

# Note: You also need the Texinfo 2 package to run Para mode---however,
# the `para.tar.Z' distribution includes part of that package, the
# `texinfo.el', and `texnfo-upd.el', files which provide minimal Texinfo
# support.

# To uncompress and extract the contents of `para.tar.Z', use the
# following command:      tar xvzf para.tar.Z

# If you do not have GNU tar, you may need to unpack in two stages:
#    uncompress para.tar.Z
#    tar xvf para.tar

# The contents of the file expand to about 270 kilobytes in a
# subdirectory of the current directory called `para'.  (Without
# the Texinfo files, the contents expand to about 180 kilobytes.)

# Before mailing `para.tar.Z', if you should mail it,
# uuencode the file with the following command:
# uuencode para.tar.Z para.tar.Z > para.tar.Z.uu

PARA_DOCUMENTATION = para.texinfo what-is.texi
PARA_INFO = para-mode-info what-is-para

PARA_ELISP_SOURCES = para.el

PARA_SUPPORT = README Makefile

TEXINFO_MINIMAL_SUPPORT = emacs/texinfo.el emacs/texnfo-upd.el 

PREFIX=/opt
BINDIR=$(PREFIX)/bin
EMACSDIR=$(PREFIX)/emacs
INFODIR=$(PREFIX)/info

# This Makefile creates a subdirectory and links the sources into it.
# When the contents of the tar file are extracted, the files extract
# into the subdirectory called `para' containing the
# documentation and Emacs Lisp files

dist: $(PARA_DOCUMENTATION) $(PARA_ELISP_SOURCES) $(PARA_SUPPORT) \
                $(TEXINFO_MINIMAL_SUPPORT)  
	mkdir para
	ln $(PARA_DOCUMENTATION) $(PARA_ELISP_SOURCES)      \
                $(PARA_SUPPORT) $(TEXINFO_MINIMAL_SUPPORT)  \
		para
	tar chofvz para-0.27.tar.Z para
	-rm -r para

# install target added by Greg Lehey, LEMIS, 18 May 1993
install: info dvi
	if [ -f $(BINDIR)/emacs ] ; then true \
	else echo Please install emacs before installing para; \
	     exit 0; \
	fi
	if [ -f $(BINDIR)/makeinfo ] ; then true \
	else echo Please install texinfo before installing para; \
	     exit 0; \
	fi
	cp $(PARA_ELISP_SOURCES) $(EMACSDIR)
	for i in $(PARA_INFO); do \
		cp $$i $(INFODIR); \
	done
	cp *.el /opt/lib/emacs/site-lisp


# dvi target added by Greg Lehey, LEMIS, 7 February 1994
dvi:	para.texinfo
	tex para.texinfo
	tex para.texinfo

info:	$(PARA_DOCUMENTATION)
	for i in $(PARA_DOCUMENTATION); do \
		makeinfo $$i; \
	done
	
clean:
	rm -f core *~ *.dvi *.log *.toc *.aux what-is.?? para.??