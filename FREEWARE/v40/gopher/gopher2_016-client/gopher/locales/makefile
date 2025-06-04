#**********************************************************************
# Changes from Alan Coopersmith's patches:
#    - Domain/OS support added
#**********************************************************************

# Swedish support added by Johan Svensson <Johan@ecsdg.lu.se>,
# EkonomiCentrum Software Development Group, Lund University, Sweden.
# 1994-06-08
# 


.SUFFIXES:
.SUFFIXES: .cat .msg $(SUFFIXES)

include ../../Makefile.config

LOCALES = Es.cat En_US.cat Fr.cat sv.cat

# OSF systems
#LOCALEDIR = /usr/lib/nls

LOCALEDIR = /usr/lib/locale

.msg.cat:
	rm -f $@
	gencat $@ $<

all : $(LOCALES)

clean :
	rm -f *.cat *.cat.m *~

#
# Since many vendors don't have standard directories for languages
# we must have separate install scripts...
#


install_sol2 :
	cp Fr.cat $(LOCALEDIR)/fr/LC_MESSAGES/gopher.cat
	cp Es.cat $(LOCALEDIR)/es/LC_MESSAGES/gopher.cat
	cp sv.cat $(LOCALEDIR)/sv/LC_MESSAGES/gopher.cat

install_osf :
	cp Fr.cat /usr/lib/nls/msg/fr_FR.iso8859-1/gopher.cat
	cp Es.cat /usr/lib/nls/msg/es_ES.iso8859-1/gopher.cat
	cp sv.cat /usr/lib/nls/msg/sv_SE.iso8859-1/gopher.cat

# HP/Apollo Domain/OS 10.4 or newer
install_domain:
	install -c -m 644 En_US.cat /usr/nlslib/en_US.iso88591/gopher.cat
	install -c -m 644 Fr.cat /usr/nlslib/fr_FR.iso88591/gopher.cat
	install -c -m 644 Es.cat /usr/nlslib/es_ES.iso88591/gopher.cat
	install -c -m 644 sv.cat /usr/nlslib/sv_SE.iso88591/gopher.cat

#
# Hacked install for sites that haven't set up locales correctly
#

install :
	-mkdir $(CLIENTLIB)/gophernls
	cp Fr.cat $(CLIENTLIB)/gophernls
	cp Es.cat $(CLIENTLIB)/gophernls
	cp sv.cat $(CLIENTLIB)/gophernls
