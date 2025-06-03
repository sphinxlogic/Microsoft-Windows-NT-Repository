#
# Makefile for kit (sub-directory shar)
#

# $Id: Makefile.SH,v 2.0.1.2 92/04/20 17:42:50 ram Exp $
#
# $Log:	Makefile.SH,v $
# Revision 2.0.1.2  92/04/20  17:42:50  ram
# patch15: now creates directories before installing
# 
# Revision 2.0.1.1  92/01/11  19:22:47  ram
# patch11: created
# 

SHELL = /bin/sh

RM = rm
libdir = /opt/lib/kit

scripts = makeshar rshar
scriptsh = makeshar.SH rshar.SH
added = Makefile config.sh

all: $(scripts)

makeshar: makeshar.SH
	sh $@.SH

rshar: rshar.SH
	sh $@.SH

install install-manman install-catman: $(scripts)
	../install -d $(libdir)
	-for file in $(scripts); do \
		../install -m 555 $$file $(libdir); \
	done

clean:
	$(RM) -f $(scripts)

realclean clobber: clean
	$(RM) -f $(added)
