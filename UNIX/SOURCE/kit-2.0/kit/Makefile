#
# Makefile for kit (sub-directory kit)
#

# $Id: Makefile.SH,v 2.0.1.6 92/09/04 19:22:28 ram Exp $
#
# $Log:	Makefile.SH,v $
# Revision 2.0.1.6  92/09/04  19:22:28  ram
# patch19: forgot the 'range' target
# 
# Revision 2.0.1.5  92/05/02  14:13:13  ram
# patch16: added rules for new range script
# 
# Revision 2.0.1.4  92/04/20  17:32:57  ram
# patch15: now creates directories before installing
# patch15: minikit is now created via a .SH file
# 
# Revision 2.0.1.3  91/11/25  15:49:18  ram
# patch9: now installs minikit in the private kit library
# 
# Revision 2.0.1.2  91/11/25  12:03:28  ram
# patch8: this file was stamped with a wrong package name
# 
# Revision 2.0.1.1  91/05/27  14:21:28  ram
# patch5: some dumb make failed on previous makefile
# 
# Revision 2.0  91/02/19  15:49:49  ram
# Baseline for first official release.
# 

SHELL = /bin/sh

RM = rm
binsh = /opt/bin
libdir = /opt/lib/kit

bin = kit mailkit unkit
lib = minikit range
scripts = $(bin) $(lib)
scriptsh = kit.SH mailkit.SH unkit.SH minikit.SH
added = Makefile config.sh

all: $(scripts)

kit: kit.SH
	sh $@.SH

mailkit: mailkit.SH
	sh $@.SH

unkit: unkit.SH
	sh $@.SH

minikit: minikit.SH
	sh $@.SH

range: range.SH
	sh $@.SH

install install-manman install-catman: $(scripts)
	../install -d $(binsh)
	-for file in $(bin); do \
		../install -m 755 $$file $(binsh); \
	done
	../install -d $(libdir)
	-for file in $(lib); do \
		../install -m 755 $$file $(libdir); \
	done

clean:
	$(RM) -f $(scripts)

realclean clobber: clean
	$(RM) -f $(added)
