#	$Header: /home/kreskin/u0/barnett/Src/Ease/ease/RCS/Makefile,v 3.4 1991/10/15 17:37:27 barnett Exp $
#
#	$Log: Makefile,v $
# Revision 3.4  1991/10/15  17:37:27  barnett
# Changed 'make shar'
#
# Revision 3.3  1991/09/09  16:41:11  barnett
# added '-' to /bin/rm in "make all"
#
# Revision 3.2  1991/05/16  10:51:13  barnett
# Fixed a few typos and added the MAKE variable
#
# Revision 1.2  1991/02/25  22:07:29  barnett
# Added RCS header, amd specified value for MAKE variable
#
# Revision 1.1  1991/02/25  14:37:28  barnett
# Initial revision - release to net
#
#
# where do you want to place the binaries and scripts?
# the programs are: et cfc cfdiff cfstrip
BINDIR=/opt/bin

#where is your original sendmail file?
ORIGINAL=/etc/sendmail.cf
#ORIGINAL=/usr/lib/sendmail.cf
#ORIGINAL=/usr/lib/sendmail.main.cf
#ORIGINAL=/usr/lib/sendmail.subsidiary.cf

# What arguments to cfc do you want for cfc (see the file CONVERTING)
# IDA
#CONVERT_FLAGS=-i
#SunOS Subsidiary
CONVERT_FLAGS=-s -CV

#what extenstion do you want for the manual pages?
EXT=1
#EXT=l
#EXT=n
#EXT=0
SHELL=/bin/sh
MAKE=make

all:
	cd cfc; ${MAKE} ${FLAGS}  $@
	cd src; ${MAKE} ${FLAGS}  $@
	cd utils; ${MAKE} ${FLAGS}  $@
	touch all
	
install:
	test -d ${BINDIR} || mkdir ${BINDIR}
#	cd cf; make $(MFLAGS) $@
	BD=${BINDIR};cd cfc; ${MAKE} ${MFLAGS} BINDIR=$$BD $@
	BD=${BINDIR};cd src; ${MAKE} ${MFLAGS} BINDIR=$$BD $@
	BD=${BINDIR};cd utils; ${MAKE} ${MFLAGS} BINDIR=$$BD $@
	cd doc; ${MAKE} ${MFLAGS} EXT=${EXT} $@



convert:	${ORIGINAL} all 
		${BINDIR}/cfc ${CONVERT_FLAGS} < ${ORIGINAL} >sendmail.ease

test_conversion:	${ORIGINAL} sendmail.cf
	PATH=${BINDIR}:$$PATH;export PATH;cfdiff ${ORIGINAL} sendmail.cf

clean:
#	cd cf; make $(MFLAGS) $@
	cd src; ${MAKE} ${MFLAGS} $@
	cd cfc; ${MAKE} ${MFLAGS} $@
	cd utils; ${MAKE} ${MFLAGS} $@
	-/bin/rm all


debug:	all sendmail.ease
	BD=${BINDIR};cd debug; ${MAKE} ${MFLAGS} BINDIR=$$BD $@
sendmail.ease:
	@ echo where is the sendmail.ease file you want to debug?	

sendmail.cf: all sendmail.ease
	${BINDIR}/et <sendmail.ease >sendmail.cf

hostname.ease: SCCS/s.hostname.ease
	sccs get hostname.ease

hostname.cf:	hostname.ease
	et -C <hostname.ease >hostname.cf

install_hostname: hostname.cf
	@sccs check
	/bin/mv ${ORIGINAL} ${ORIGINAL}.old
	/bin/cp hostname.cf ${ORIGINAL}
	build-new-aliases

mail: Part01 Part02 Part03 Part04 Part05 Part06
	@ WHO=${WHO} && \
	[ $${WHO:-missing} = missing ] && \
	echo USAGE:  make mail WHO=address || \
	(( mail -s "Ease 3.0: Part 1 of 6" $${WHO} <Part01) ;\
	 ( mail -s "Ease 3.0: Part 2 of 6" $${WHO} <Part02) ;\
	 ( mail -s "Ease 3.0: Part 3 of 6" $${WHO} <Part03) ;\
	 ( mail -s "Ease 3.0: Part 4 of 6" $${WHO} <Part04) ;\
	 ( mail -s "Ease 3.0: Part 5 of 6" $${WHO} <Part05) ;\
	 ( mail -s "Ease 3.0: Part 6 of 6" $${WHO} <Part06; ))

depend lint print:
	-for dir in *; do [ -f $$dir/[Mm]akefile ] && (cd $$dir; make $(MFLAGS) $@); done

shar:
	dirname=`basename $$PWD`; cd ..; shar `find $$dirname -name RCS -prune -o -name GE -prune -o -type f -print|egrep -v '~|#' ` >/tmp/ease.shar

