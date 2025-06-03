#
#	Makefile for kit
#

# $Id: Makefile.SH,v 2.0.1.5 92/01/11 19:08:43 ram Exp $
#
# $Log:	Makefile.SH,v $
# Revision 2.0.1.5  92/01/11  19:08:43  ram
# patch11: added subdirectories badtar and shar
# 
# Revision 2.0.1.4  91/12/08  14:53:40  ram
# patch10: make clobber was not removing cppstdin
# 
# Revision 2.0.1.3  91/11/25  12:03:02  ram
# patch8: this file was stamped with a wrong package name
# 
# Revision 2.0.1.2  91/05/27  14:14:19  ram
# patch5: some dumb make failed on previous makefile
# 
# Revision 2.0.1.1  91/04/01  15:34:43  ram
# patch1: added des target
# 
# Revision 2.0  91/02/19  15:48:41  ram
# Baseline for first official release.
# 

SHELL = /bin/sh

SUBDIRS = badtar btoa kit shar man des
RM = rm

#
#	Building rules
#

# indirect rules added by Greg Lehey, LEMIS, 20 May 1993
INSTALL = install-catman

install:	$(INSTALL)

install-catman install-manman:
	for i in $(SUBDIRS); do \
	  echo installing in $$i; \
	  (cd $$i; make $@) \
	done

all clean realclean: makefiles
	-@for dir in $(SUBDIRS); do \
		(cd $$dir && make $@) \
	done

makefiles:
	-@for dir in $(SUBDIRS); do \
		(cd $$dir && if test ! -f Makefile; then \
			sh Makefile.SH; fi) \
	done

clobber: makefiles
	-@for dir in $(SUBDIRS); do \
		(cd $$dir && make $@) \
	done
	$(RM) -f config.h config.sh Makefile install cppstdin
