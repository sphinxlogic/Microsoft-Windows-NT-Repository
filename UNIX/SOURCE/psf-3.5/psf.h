/* ta=4
				p s f . h
		general configuration for psf.
*/
/*
 * $Id: psf.h,v 3.4 1992/02/24 06:48:59 ajf Exp ajf $
*/
/**** Configuration information:  if not defined in the makefile *****
 *
 *	DOS users:  modify these defaults, no other #defines are necessary
 * 				for msdos users.
 */

/* default paper size */

#ifndef PAPERTYPE
#define PAPERTYPE "letter"
#endif

/* location of binaries: so psf can find psfdoub */

#ifndef BINDIR
#if defined(MSDOS) || defined(__MSDOS__)
#define BINDIR "c:/bin"
#else
#define BINDIR "/local/bin"
#endif
#endif

/* MSDOS users may wish to always generate CTL/D at end of print job */
/* 
#define CTLDALWAYS 
*/
#if defined(MSDOS)  ||  defined(__MSDOS__)
#ifndef BINDIR
#define BINDIR "c:/bin"
#endif
#ifndef PDEF
#define PDEF "c:/lib/psfprint.def"
#endif
#endif

#ifndef PSFDOUB
#define PSFDOUB "psfdoub"
#endif


/* if two paper trays are available, and you want to use them for
 * double sided printing, then make HOPPER defined.
 */

/* #define HOPPER  */

/* prefix name for "book" temporary file for reversed or book print
 */

#ifndef TEMPFILE
#if defined(MSDOS) || defined(__MSDOS__)
#define TEMPFILE	"Psf"
#else
#define TEMPFILE	"/tmp/Psf"
#endif
#endif

/* bookwork file name if multiple hoppers not available.
 */
#ifndef BOOKDFILE
#define BOOKFILE "psfbook.psd"
#endif

/* change the following names only if you have reason to change
 * the default names of psfdoub or psfbanner
 */

#ifndef PSFDOUB
#define PSFDOUB "psfdoub"
#endif

#ifndef PSFBANNER
#define PSFBANNER "psfbanner"
#endif

/*********  end configuration **********/

/* 	the following are 'escape' sequences used by psf, psfbs & psfmail
	used to generate bold, underline, etc.  What should be used
	for EBCDIC???  change if needed....
*/

#define	ESCAPE				0x05		/* CTL/E */

#define BEGIN_BOLD			"\005B"
#define END_BOLD			"\005b"

#define BEGIN_UNDERLINE		"\005U"
#define END_UNDERLINE		"\005u"

#define BEGIN_ITALICS		"\005I"
#define END_ITALICS			"\005i"

/* for psfmbox/psfmail */

#define	BEGIN_NAME			"\005F1\005B\005P13"	/* Helvetica bold 13 point */
#define END_NAME			"\005p\005b\005f"		

#define BEGIN_SUBJECT		"\005P13\005F1\005B\005I"	/*	Helvetica bold italic 13 point */
#define END_SUBJECT			"\005i\005b\005f\005p"		/*	return to normal point, font */

/* other stuff */

#define	min(x,y)		((x) < (y) ? (x) : (y))
#define	max(x,y)		((x) > (y) ? (x) : (y))

#define MAX_PAGES	1000		/* max pages in a book */
#define LONG_STR	1000		/* max bytes in an input text line */

#define	NSLOTS		6
#define NPAGE		30

/*	exit codes */

#define JOB_IS_DONE 	0
#define REPEAT_JOB 		1
#define DISCARD_JOB 	2
#define ERROR 			-2
