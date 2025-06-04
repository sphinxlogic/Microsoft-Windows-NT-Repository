#
#       Makefile
#
###
#
#  Copyright (c) 1994           David Albert Bagley, bagleyd@perry.njit.edu
#
#                   All Rights Reserved
#
#  Permission to use, copy, modify, and distribute this software and
#  its documentation for any purpose and without fee is hereby granted,
#  provided that the above copyright notice appear in all copies and
#  that both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of the author not be
#  used in advertising or publicity pertaining to distribution of the
#  software without specific, written prior permission.
#
#  This program is distributed in the hope that it will be "useful",
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#

# if this fails to build one may have to edit the individual Imakefiles
# especially if your OS does not have srand48() and drand48().
# Please consult the individual README's.
# This Makefile is relatively new, so use with caution!

# After ftping all the *.tar.gz files you want (each program is independent)
#	make -f altris.Makefile gunzip  # beware: destroys the original files
#	make -f altris.Makefile
#	make -f altris.Makefile run     # this cycles through all programs

#SHELL=/bin/sh

XLOCK=more
STUFF=abacus dial threed
ROTATIONAL=rubik skewb pyraminx oct mball
SLIDING=cubes triangles hexagons mlink
PUZZLES=${ROTATIONAL} ${SLIDING}
ALTRIS=tetris tertris hextris welltris

#PRE=xlock
#NAME=xlockmore.
#PRE=x
#NAME=xpuzzle.
PRE=al
NAME=altris.

PROGRAMS=${ALTRIS}

# Used by me to zip and write and read from my floppy drive
UNIXDIR=${HOME}/net/pending
DOSDIR=/dosa
# mntflop is a mount floppy disk utility for users that I maintain for SunOS,
# Solaris, and Linux.  If there is interest, I will make it publicly available.
# If there is something out there already, let me know.

all:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			xmkmf;\
			make clean;\
			make;\
			cd ..;\
		fi;\
	done
	
xmkmf:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			xmkmf;\
			cd ..;\
		fi;\
	done

lint:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make lint;\
			cd ..;\
		fi;\
	done

run:
	for i in ${PROGRAMS}; do\
		cd ${PRE}$${i};\
		./$${i};\
		cd ..;\
	done

runs:
	for i in ${PROGRAMS}; do\
		cd ${PRE}$${i};\
		./$${i} -s;\
		cd ..;\
	done

clean:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			xmkmf;\
			make clean;\
			cd ..;\
		fi;\
	done

clean.all:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			xmkmf;\
			make clean.all;\
			cd ..;\
		fi;\
	done

fast.clean:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std clean;\
			cd ..;\
		fi;\
	done

fast.clean.all:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std clean.all;\
			cd ..;\
		fi;\
	done

tar:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std tar;\
			cd ..;\
		fi;\
	done

compress:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std compress;\
			cd ..;\
		fi;\
	done

gzip:
	cp ${NAME}README ${UNIXDIR}/${NAME}README;\
	cp ${NAME}Makefile ${UNIXDIR}/${NAME}Makefile;\
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std gzip;\
			cd ..;\
			mv ${PRE}$${i}.tar.gz ${UNIXDIR};\
		fi;\
	done

tgz:
	mntflop -d;\
	cp ${NAME}README ${DOSDIR}/${NAME}rea;\
	cp ${NAME}Makefile ${DOSDIR}/${NAME}mak;\
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			make -f Makefile.std tgz;\
			cd ..;\
			cp `echo ${PRE}$${i} | cut -c1-8`.tgz ${DOSDIR};\
		if [ -w ${DOSDIR}/`echo ${PRE}$${i} | cut -c1-8`.tgz ]; then\
				rm -f `echo ${PRE}$${i} | cut -c1-8`.tgz;\
			fi;\
		fi;\
	done;\
	mntflop -u;\
	if [ -x /usr/bin/eject ]; then\
		/usr/bin/eject floppy;\
	fi

utar:
	for i in ${PROGRAMS}; do\
		tar xvf ${PRE}$${i}.tar;\
		rm -f ${PRE}$${i}.tar;\
	done

uncompress:
	for i in ${PROGRAMS}; do\
		uncompress ${PRE}$${i}.tar.Z;\
		tar xvf ${PRE}$${i}.tar;\
		rm -f ${PRE}$${i}.tar;\
	done

gunzip:
	for i in ${PROGRAMS}; do\
		gunzip ${PRE}$${i}.tar.gz;\
		tar xvf ${PRE}$${i}.tar;\
		rm -f ${PRE}$${i}.tar;\
	done

utgz:
	for i in ${PROGRAMS}; do\
		if [ -r `echo ${PRE}$${i} | cut -c1-8`.tgz ]; then\
			gunzip -fN `echo ${PRE}$${i} | cut -c1-8`.tgz;\
			tar xvf ${PRE}$${i}.tar;\
			rm -f ${PRE}$${i}.tar;\
		fi;\
	done

extract:
	mntflop -d;\
	cp ${DOSDIR}/${NAME}rea ${NAME}README;\
	cp ${DOSDIR}/${NAME}mak ${NAME}Makefile;\
	chmod 600 ${NAME}README ${NAME}Makefile;\
	for i in ${PROGRAMS}; do\
		if [ -r ${DOSDIR}/`echo ${PRE}$${i} | cut -c1-8`.tgz ]; then\
			cp ${DOSDIR}/`echo ${PRE}$${i} | cut -c1-8`.tgz .;\
		fi;\
	done;\
	mntflop -u;\
	if [ -x /usr/bin/eject ]; then\
		/usr/bin/eject floppy;\
	fi

read:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			more README;\
			cd ..;\
		fi;\
	done

man:
	for i in ${PROGRAMS}; do\
		if [ -d ${PRE}$${i} ]; then\
			cd ${PRE}$${i};\
			nroff -man ${PROGRAM}.man | more;\
			cd ..;\
		fi;\
	done
	
#print:

#install:
