# Makefile for Ease Translator (et).
#
#	$Header: /tmp_mnt/home/kreskin/u0/barnett/Src/Ease/ease/src/RCS/Makefile,v 3.3 1991/09/09 16:33:23 barnett Exp $
#
#	$Log: Makefile,v $
# Revision 3.3  1991/09/09  16:33:23  barnett
# Minor bug fix release
#
# Revision 3.2  1991/05/16  10:45:25  barnett
# Better support for System V machines
# Support for machines with read only text segments
#
# Revision 3.1  1991/02/25  22:09:52  barnett
# Fixed some portability problems
#
# Revision 2.1  1990/01/30  13:54:13  jeff
# Updated for release 2.1 Aplha.
#
# Revision 2.0  88/06/15  14:47:35  root
# Baseline release for net posting. ADR.
# 
#
#	James S. Schoner, Purdue University Computing Center,
#			  West Lafayette, Indiana  47907
#
#	Copyright (c) 1985 by Purdue Research Foundation
#
#	All rights reserved.
#

INCLUDE =

# where to install the binaries
BINDIR          =	../bin

OWNER = root
GROUP = staff
MODE = 755
# Important Compile flags are defined here
# DATA_RW - this is defined if your system allows string constants to 
#           be modifiable. If you have one of the following systems, I am told
#	    you should NOT define it: (Thanks to rainer@flyer.uni-duisburg.de)
#	         - A sun 3/60 using gcc
#		 - A esix SysV PC  with gcc
#                - A HP9000/845 with cc
#		 - Apollo/Domain with BSD4.3 environment
#           The default is to define it, because this is what earlier versions 
#           of Ease did.
#
# SYSV
# 		If you have a system V machine, and have <string.h>
#		instead of <strings.h>, then define this
#
#OFLAGS=-DSYSV 
OFLAGS=-DDATA_RW
 
DEFS = ${OFLAGS}
LIBS = -ll
#CC = cc
CC = gcc
YACC = yacc 
#YACC = bison -y

LP = lpr
LPFLAGS = -J"Ease Source"


# System V install different from BSD install sigh...
#INSTALL = install -c -m ${MODE} -o ${OWNER} -g ${GROUP}
INSTALL=cp

# Special Debug flags
# select this one to debug the grammar
#DEFS =-DYYDEBUG
# The following two lines are used with the SunOS malloc() debug package.
#DEFS = -DMALLOC_DEBUG	#  Part of the SunOS malloc package
#LIBS = /usr/lib/debug/malloc.o -ll
# some people must remove the -O
CFLAGS = -DSYSV -O3 -g ${DEFS} ${INCLUDE} -DUSE_FLEX


HDR = symtab.h
SRC = main.c emitcf.c errors.c idman.c strops.c symtab.c fixstrings.c
LST = Makefile lexan.l parser.y ${HDR} ${SRC}
DEP = parser.c lexan.c ${SRC}
OBJ = parser.o lexan.o main.o emitcf.o errors.o idman.o strops.o symtab.o \
	fixstrings.o
CFILES= $(SRC) $(DEP)

all: et

et: ${OBJ}
	${CC} ${CFLAGS} -o et ${OBJ} ${LIBS}

clean: FRC
	rm -f et *.o lexan.c parser.c y.output yacc.acts yacc.tmp \
	      lexdefs.h y.tab.h errs Makefile.bak y.output y.tab.c y.tok.h

depend:
	${CC} -M ${CFLAGS} ${CFILES} | \
	sed -e ':loop' \
	    -e 's/\.\.\/[^ /]*\/\.\./../' \
	    -e 't loop' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } ; \
	      END { print rec } ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ex - Makefile < eddep
	rm eddep makedep

install: et FRC
	${INSTALL} et ${BINDIR}
	${INSTALL} ease.sh ${BINDIR}/ease


lint:   ${DEP} symtab.h FRC
	lint -hxn ${DEP}

print:  ${LST} FRC
	@pr -f ${LST} | ${LP} ${LPFLAGS}

spotless: clean FRC
	rcsclean ${LST}

y.tab.h parser.c: parser.y
	@rm -f parser.c
	${YACC} -v -d parser.y
	sed 's/=yylex/=yyyylex/' < y.tab.c >parser.c

# the following dummy rule is because of the results of 'make depend'
# However, under SunOS Make - it complains. You may have to comment it out
./lexdefs.h:	lexdefs.h

lexdefs.h:	y.tab.h
	-(cmp -s y.tab.h lexdefs.h || cp y.tab.h lexdefs.h)

lexan.c: lexan.l
	flex  -t lexan.l > lexan.c

parser.o: y.tok.h
y.tok.h:	y.tab.h
	grep '^#.*define' y.tab.h |\
	sed 's/^# define \([^ ]*\) [^ ]*$$/	"\1",/' >y.tok.h

${HDR} ${SRC} lexan.l parser.y:
	co $@

FRC:


# DO NOT DELETE THIS LINE -- make depend uses it

main.o: main.c fixstrings.h 
emitcf.o: emitcf.c  symtab.h fixstrings.h
errors.o: errors.c fixstrings.h
idman.o: idman.c  symtab.h fixstrings.h
strops.o: strops.c fixstrings.h 
strops.o: symtab.h
symtab.o: symtab.c fixstrings.h 
symtab.o: symtab.h
fixstrings.o: fixstrings.c 
parser.o: parser.c fixstrings.h  symtab.h
lexan.o: lexan.c  fixstrings.h symtab.h lexdefs.h
main.o: main.c fixstrings.h 
emitcf.o: emitcf.c  symtab.h fixstrings.h
errors.o: errors.c fixstrings.h
idman.o: idman.c  symtab.h fixstrings.h
strops.o: strops.c fixstrings.h 
strops.o: symtab.h
symtab.o: symtab.c fixstrings.h 
symtab.o: symtab.h
fixstrings.o: fixstrings.c 








