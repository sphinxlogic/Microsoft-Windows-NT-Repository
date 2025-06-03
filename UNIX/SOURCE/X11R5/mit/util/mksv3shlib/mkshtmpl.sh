#!/bin/sh
######################################################################
# mkshtmpl.sh 1.4                                                    #
# ------------------------------------------------------------------ #
# Make shared library template files from archive library            #
# Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany        #
# (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de		     #
######################################################################

if [ "$1" = "" ] ; then
	echo $0: no archive library name specified. >&2
	exit 1
fi
if [ ! -f $1 ] ; then
	echo $0: named archive library doesn\'t exist. >&2
	exit 1
fi
version="1.4"
LIB=`basename $1 | cut -f1 -d.` ; export LIB
TMPDIR=${TMPDIR:=/usr/tmp} ; export TMPDIR
SVR3SHLIBADDR=sv3ShlibAddr
DEPMAKEFILE=XlibsMakefile


if [ ! -d ${LIB} ] ; then
    echo Making directory ${LIB}...
    mkdir ${LIB}
fi

echo Creating shared library description template files from archive ${LIB}.a...

echo -n "Collecting all external references... "
# create file of all external symbols with object names
nm -ep $1 | \
nawk '$1 ~ /lib.*\.a/ { OBJ=substr($1, index($1,"[")+1, \
index($1,"]")-index($1,"[")-1)  } ; \
$2 ~ /^[C|D|T|U]$/ { print OBJ, $2, $3 }' >${LIB}/exports.out
echo "Done.
`cat ${LIB}/exports.out | wc -l` external references found"

echo "Getting symbols from other libraries... "
> ${TMPDIR}/${LIB}.A
> ${TMPDIR}/${LIB}.nol
if [ -f ${DEPMAKEFILE} ] ; then
	make ${LIB}.A -f ${DEPMAKEFILE}
	cat ${TMPDIR}/${LIB}.A >> ${LIB}/exports.out
	echo Done.
else
	echo Makefile not found.
	echo "****************************************************************"
	echo Please check whether ${LIB}.a imports symbols you could resolve
	echo via the \"#oject noload\" directive. If you want to do that
	echo you must create an auxiliary makefile similar XlibsMakefile.
        echo "****************************************************************"
fi

echo -n "Getting explicit defined symbols from ${LIB}/constData... "
if [ -f ${LIB}/constData ] ; then
	cat ${LIB}/constData >> ${LIB}/exports.out
	echo Done.
else
	echo Not found.
	echo "****************************************************************"
	echo WARNING! Check the library sources whether they use e.g. ANSI-C
	echo style global \"const\"ant data! ANSI-C compilers place these to
	echo the .text section, and this script generates then an incorrect
	echo branch table entry. Hence you should create for such symbols
	echo a file ${LIB}/constData containing explicit definitions like e.g.:
	echo \"foo.o D size\"
	echo "****************************************************************"
fi

echo -n "Collecting all external, inside defined .text symbols... "
# sort bei name and type
sort +2 +1 -o${LIB}/exports.out ${LIB}/exports.out
# count and remove adjacent lines, counters later needed for #init table
uniq -2 -c ${LIB}/exports.out ${TMPDIR}/${LIB}.e

nawk '$3 == "T" { print $4 }' ${TMPDIR}/${LIB}.e >${TMPDIR}/${LIB}.T
echo "Done.
`cat ${TMPDIR}/${LIB}.T | wc -l` inside defined .text symbols found"

echo -n "Collecting all external, inside defined .data symbols... "
nawk '$3 == "C" { print $2, $4 }' ${TMPDIR}/${LIB}.e >${LIB}/common.out
if [ -s ${LIB}/common.out ] ; then
	echo ""
	echo "****************************************************************"
	echo WARNING! Following symbols are \"common\":
	echo ""
	cat ${LIB}/common.out
	echo ""
	echo "(These names are recorded in ${LIB}/common.out)"
	echo Probably you must change the appropriate source files to
	echo initialize them explicitly, to force the compiler to put them
	echo to the .data section!!!
	echo "****************************************************************"
fi
nawk '$3 ~ /[C|D]/ { print $4 }' ${TMPDIR}/${LIB}.e >${TMPDIR}/${LIB}.CD
echo "Done.
`cat ${TMPDIR}/${LIB}.CD | wc -l` inside defined .data symbols found"

echo "Collecting all outside references (imported symbols)... "
nawk '$3 == "U" { print $1, $4, $2 }' ${TMPDIR}/${LIB}.e >${LIB}/imports.out

echo -n "Getting additional imported symbols from ${LIB}/addImports... "
if [ -f ${LIB}/addImports ] ; then
	cat ${LIB}/addImports >> ${LIB}/imports.out
	echo Done.
else
	echo File not found.
fi
echo "Done.
`cat ${LIB}/imports.out | wc -l` imported symbols found"

# create import.h
# -------------------------------------------------------------------

echo -n "Creating ${LIB}/${LIB}.h (this is import.h)... "
echo "/* ${LIB} : import.h
 * indirection defines
 * Template created by $0 $version
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 */

#ifndef _${LIB}_import
#define _${LIB}_import
#if defined(SVR3SHLIB) && !defined(SVR3SHDAT)
" > ${LIB}/${LIB}.h
nawk '
{ if ($2 == "stat") {
     print "/* prevent name conflict with struct stat */" ; 
     print "#define " $2 "(path, buf) (*_"  LIB "_" $2 ")(path, buf)"
  } 
  else { 
     if ($2 == "free") { 
        print "/* prevent name conflict with member free of XFontSetMethods */" ;
        print "#define " $2 "(ptr) (*_"  LIB "_" $2 ")(ptr)"
     }
     else { 
        if ($2 == "close") {
           print "/* prevent name conflict with member close of XIM */" ;
           print "#define " $2 "(fd) (*_"  LIB "_" $2 ")(fd)"
        }
	else {
	  if ($2 == "select") {
	     print "/* prevent name conflict with bitfield select of _XtEventRec */" ;
             print "#define " $2 "(max, rd, wr, ex, to) (*_"  LIB "_" $2 ")(max, rd, wr, ex, to)"
	  }
	  else print "#define " $2 "  (*_" LIB "_" $2 ")"
	}
     }
  }
}' LIB=${LIB} ${LIB}/imports.out >>${LIB}/${LIB}.h

echo "

/* Imported functions declarations
 * Why declaring some imported functions here?
 * This should be done gracefully through including of the systems
 * header files. Unfortunatly there are some source files don't
 * include all headers they should include, there are also some functions
 * nowhere declared in the systems headers and some are declared
 * without "extern" and cause problems since the names are redefined
 * and these declarations then become undesired false pointer definitions.
 */

/* This section needs editing! It's only an example for the X libs. */

#ifdef __STDC__
extern int creat(char const *, unsigned short);
extern void exit(int);
extern int printf(char const *, ...);
extern int sprintf(char *, char const *, ...);
extern int sscanf(char *, char const *, ...);
extern int atoi(char const *);
extern int access(char const *, int);
extern unsigned int alarm(unsigned int);
extern unsigned int sleep(unsigned int);
extern int close(int);
extern int read(int, char *, unsigned int);
extern int write(int, char const *, unsigned int);
extern int grantpt(int);
extern char *ptsname(int);
extern int unlockpt(int);
extern char *getenv(char const *);
#else
extern int creat();
extern void exit();
extern int printf();
extern int sprintf();
extern int sscanf();
extern int atoi();
extern int access();
extern unsigned int alarm();
extern unsigned int sleep();
extern int close();
extern int read();
extern int write();
extern int grantpt();
extern char *ptsname();
extern int unlockpt();
extern char *getenv();
#endif

/* use char * also for __STDC__, the sources want it */ 
extern char *malloc(), *realloc(), *calloc();

#include <string.h>

extern char *sys_errlist[];
extern int sys_nerr;

extern void qsort();
extern int _flsbuf();
extern int _filbuf();
extern int ioctl();
extern int getmsg();
extern int putmsg();

extern unsigned short ntohs(), htons();
extern unsigned long ntohl(), htonl();
extern unsigned long inet_addr();
extern int connect();
extern int gethostname();
extern int setsockopt();
extern int socket();
extern int writev();
extern int gettimeofday();
" >> ${LIB}/${LIB}.h

echo "
/* Functions with ambiguous names */

#ifdef __STDC__
extern int close(int);
extern void free(void *);
#include <sys/time.h>
#include <sys/bsdtypes.h>
extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
#else
extern int (*_${LIB}_close)();
extern void (*_${LIB}_free)();
extern int (*_${LIB}_select)();
#endif

#endif
#endif
" >> ${LIB}/${LIB}.h
echo Done.


# create globals.c
# -------------------------------------------------------------------

echo -n "Creating ${LIB}/${LIB}.c (this is globals.c or part of globals.c)... "
echo "/* ${LIB} : ${LIB}.c
 * exported data
 * Template created by $0 $version
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 */

#ifdef SVR3SHLIB

#define VoidInit(var) void *var = 0
" >${LIB}/${LIB}.c
nawk '{ print "VoidInit(_" LIB "_" $2 ");" }' LIB=${LIB} \
 ${LIB}/imports.out >>${LIB}/${LIB}.c
echo "

#ifndef __GNUC__
/*
 * If we are working with floating point aritmetic, stock AT&T cc generates
 * an unresolved reference to __fltused. But we want to make a shared lib from
 * this here and don't want to reference /lib/libc_s.a, just define this sym as
 * (shared lib) static.
 * The trick is that while building the shared lib all references to this
 * symbol are resolved internally. But the symbol will be outside only visible
 * as a static one, so preventing a name conflict with other shared libs.
 */
long __fltused = 0;
#endif

/*
 * A free place for free branchtab-slots.
 * 
 * This would be a great place for an error-check mechanism for shared libs ...
 */
extern void _${LIB}_dummy() {};

#endif

" >>${LIB}/${LIB}.c
echo Done.


# create lib*.def
# -------------------------------------------------------------------

echo Creating ${LIB}/${LIB}.def...
echo -n "Creating #address statements... "
TEXT_ADR=`grep ${LIB} ${SVR3SHLIBADDR} | cut -f2 -d:`
DATA_ADR=`grep ${LIB} ${SVR3SHLIBADDR} | cut -f3 -d:`
echo "/* ${LIB} : ${LIB}.def
 * shared library description file
 * Template created by $0 $version
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 * Conception derived from work of Thomas Roell
 */

/**/#address .text 0x${TEXT_ADR}
/**/#address .data 0x${DATA_ADR}

" > ${LIB}/${LIB}.def

if [ -s ${TMPDIR}/${LIB}.nol ] ; then
	echo "/**/#objects noload" >> ${LIB}/${LIB}.def
	cat ${TMPDIR}/${LIB}.nol >> ${LIB}/${LIB}.def
fi

echo "
/**/#branch" >> ${LIB}/${LIB}.def
echo Done.

echo -n "Creating #branch table from exported .text symbols... "
nawk '{ print $1, ++BRANCH }; END { print ; \
print "_" LIB "_dummy " ++BRANCH "-???" }' LIB=${LIB} ${TMPDIR}/${LIB}.T >> ${LIB}/${LIB}.def
echo "/* ^^^must be edited^^^ */" >> ${LIB}/${LIB}.def
echo Done.

echo -n "Creating #objects table... "
echo "

/**/#objects
DOWN/globals.o  /* All objects with global data must be at come first!*/" \
 >> ${LIB}/${LIB}.def
ar t $1 | egrep -v '^globals.o$' | sort >${TMPDIR}/${LIB}.objs
nawk '{ print "DOWN/" $1 }' ${TMPDIR}/${LIB}.objs >> ${LIB}/${LIB}.def
echo Done.

# put initialization code of indirection defines to global.o if it is
# used more than once, if only once put it to respective object
# file (arbitrary algorithm)
echo -n "Creating #init tables... "
echo "

/* init sections needs editing!
 */

/**/#init globals.o" >> ${LIB}/${LIB}.def
nawk '$1 > 1 { print "_" LIB "_" $2 "  " $2 }' LIB=${LIB} \
 ${LIB}/imports.out >> ${LIB}/${LIB}.def
# sort by object name
sort +2 -o${LIB}/imports.out ${LIB}/imports.out
nawk '$1 == 1 { if ($3 != OBJ) { OBJ=$3 ; print "" ; \
print "/**/#init " $3 } ; print "_" LIB "_" $2 "  " $2 }' \
LIB=${LIB}  ${LIB}/imports.out >> ${LIB}/${LIB}.def
echo Done.

# here just guess...
echo -n "Creating #hide/#export linker section... "
echo "

/* This section needs editing! Check which
 * symbols must be really extern.
 */

/**/#hide linker *
/**/#export linker" >> ${LIB}/${LIB}.def
cat ${TMPDIR}/${LIB}.CD >>${LIB}/${LIB}.def
echo Done.


echo -n "Creating dummy trailer... "
echo "

/**/#libraries
/* Here can objects be specified, which will extracted from
 * an specified archive library and then linked to the shared
 * library. This can avoid unwanted referencing of other
 * shared libraries.
 * NOTE: this objects must be listed also under "#objects"!
 * e.g.:
/lib/libc.a memcpy.o
 * or e.g.:
#ifdef __GNUC__
GNULIB  _fixdfsi.o
#endif
 *
 * NOT USED YET
 *
 */

/**/#externals
/* Here you can specify objects which will only linked to the
 * host shared library.
 * That are e.g. objects which contain only big data you want
 * to seperate from the text section. (See mit/util/mksv3shlib/README.)
 */

/* There must be a comment as last line */" >> ${LIB}/${LIB}.def
echo Done.

#rm -f ${TMPDIR}/${LIB}.nol
#rm -f ${TMPDIR}/${LIB}.A
#rm -f ${TMPDIR}/${LIB}.T
#rm -f ${TMPDIR}/${LIB}.CD
#rm -f ${TMPDIR}/${LIB}.e
#rm -f ${TMPDIR}/${LIB}.objs

echo READY.

