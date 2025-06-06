/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/dvi/RCS/dviconcat.c,v 3.1 89/08/22 17:23:15 chris Exp $";
#endif

/*
 * DVI page concatenation program.
 */

#include "libtex/types.h"
#include "libtex/dviclass.h"
#include "libtex/dvicodes.h"
#include "libtex/error.h"
#include "libtex/fio.h"
#include "libtex/gripes.h"
#include "libtex/search.h"
#include <stdio.h>
#include <ctype.h>

#ifdef __svr4__
/* Greg Lehey, LEMIS, 16 May 1993 */
#define bcmp memcmp
#endif

char *ProgName;
extern char *optarg;
extern int optind;

/*
 * We use the following structure to keep track of fonts we have seen.
 * The final DVI file lists all the fonts used in each of the input
 * files.
 */
struct fontinfo {
	struct	fontinfo *fi_next;/* next in list */
	i32	fi_index;	/* font number in output file */
	i32	fi_checksum;	/* the checksum */
	i32	fi_mag;		/* the magnification */
	i32	fi_designsize;	/* the design size */
	short	fi_n1;		/* the name header length */
	short	fi_n2;		/* the name body length */
	char	*fi_name;	/* the name itself */
} *fonts;

struct	search *FontFinder;	/* input indicies to ptr-to-fontinfo */
i32	NextOutputFontIndex;	/* generates output indicies */
i32	OutputFontIndex;	/* current (new) index in ouput */

char	*DVIFileName;		/* the current input file name */
FILE	*inf;			/* the current input DVI file */
FILE	*outf;			/* the output DVI file */

int	errs;			/* counts non-fatal errors */

long	StartOfLastPage;	/* The file position just before we started
				   the last page (this is later written to
				   the output file as the previous page
				   pointer). */
long	CurrentPosition;	/* The current position of the file */

int	NumberOfOutputPages;	/* number of pages in new DVI file */

i32	Numerator;		/* numerator from current DVI file */
i32	Denominator;		/* denominator from current DVI file */
i32	DVIMag;			/* magnification from current DVI file */

i32	OutputNumerator;	/* numerator from first DVI file */
i32	OutputDenominator;	/* denominator from first DVI file */
i32	OutputMag;		/* magnification from first DVI file or arg */

i32	TallestPageHeight;	/* max of all tallest-page-height values */
i32	WidestPageWidth;	/* max of all widest-page-widths */
i16	DVIStackSize;		/* max of all stack sizes */

/* save some string space: we use this a lot */
char	writeerr[] = "error writing DVI file";

char	*malloc(), *realloc();
/*
 * You may get lint warnings about sprintf's return value.
 * Older versions of 4BSD have `char *sprintf()'.  ANSI and
 * SysV use `int sprintf()'; so ignore the warnings.
 */

/*
 * Lint gets somewhat confused over putc.
 */
#ifdef lint
#define putc(c, f) fputc((int) c, f)
#endif

/*
 * Start a page (process a DVI_BOP).
 */
void
BeginPage()
{
	register i32 t;
	register int i;

	OutputFontIndex = -1;	/* new page requires respecifying font */
	putbyte(outf, DVI_BOP);
	/* copy the count registers */
	for (i = 10; --i >= 0;) {
		fGetLong(inf, t);
		PutLong(outf, t);
	}
	(void) GetLong(inf);	/* previous page pointer */
	PutLong(outf, StartOfLastPage);
	if (ferror(outf))
		error(1, -1, writeerr);

	StartOfLastPage = CurrentPosition;
	CurrentPosition += 45;	/* we just wrote this much */
}

/*
 * End a page (process a DVI_EOP).
 */
void
EndPage()
{

	putbyte(outf, DVI_EOP);
	if (ferror(outf))
		error(1, -1, writeerr);
	CurrentPosition++;
	NumberOfOutputPages++;
}

/*
 * Write a font definition to the output file.
 */
void
WriteFont(fi)
	register struct fontinfo *fi;
{
	register int l;
	register char *s;

	if (fi->fi_index < 256) {
		putbyte(outf, DVI_FNTDEF1);
		putbyte(outf, fi->fi_index);
		CurrentPosition += 2;
	} else if (fi->fi_index < 65536) {
		putbyte(outf, DVI_FNTDEF2);
		PutWord(outf, fi->fi_index);
		CurrentPosition += 3;
	} else if (fi->fi_index < 16777216) {
		putbyte(outf, DVI_FNTDEF3);
		Put3Byte(outf, fi->fi_index);
		CurrentPosition += 4;
	} else {
		putbyte(outf, DVI_FNTDEF4);
		PutLong(outf, fi->fi_index);
		CurrentPosition += 5;
	}
	PutLong(outf, fi->fi_checksum);
	PutLong(outf, fi->fi_mag);
	PutLong(outf, fi->fi_designsize);
	putbyte(outf, fi->fi_n1);
	putbyte(outf, fi->fi_n2);
	l = fi->fi_n1 + fi->fi_n2;
	CurrentPosition += 14 + l;
	s = fi->fi_name;
	while (--l >= 0)
		putbyte(outf, *s++);
}

/*
 * Write the postamble for the concatenation of all the input files.
 */
void
WritePostAmble()
{
	register struct fontinfo *fi;
	register i32 postpos = CurrentPosition;	/* remember for later */

	/* POST p n d m h w s pages */
	putbyte(outf, DVI_POST);
	PutLong(outf, StartOfLastPage);
	PutLong(outf, OutputNumerator);
	PutLong(outf, OutputDenominator);
	PutLong(outf, OutputMag);
	PutLong(outf, TallestPageHeight);
	PutLong(outf, WidestPageWidth);
	PutWord(outf, DVIStackSize);
	PutWord(outf, NumberOfOutputPages);
	CurrentPosition += 29;			/* count all those `put's */

	for (fi = fonts; fi != NULL; fi = fi->fi_next)
		WriteFont(fi);

	putbyte(outf, DVI_POSTPOST);
	PutLong(outf, postpos);
	putbyte(outf, DVI_VERSION);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	putbyte(outf, DVI_FILLER);
	CurrentPosition += 10;
	while (CurrentPosition & 3) {
		putbyte(outf, DVI_FILLER);
		CurrentPosition++;
	}
	if (ferror(outf))
		error(1, -1, writeerr);
}

/*
 * Read the information we need from the postamble for the current DVI file.
 */
void
HandlePostAmble()
{
	register i32 t;
	register i16 w;

	(void) GetLong(inf);	/* previous page pointer */
	if (GetLong(inf) != Numerator) {
		error(0, 0,
		    "%s: postamble's numerator does not match preamble's",
		    DVIFileName);
		errs++;
	}
	if (GetLong(inf) != Denominator) {
		error(0, 0,
		    "%s: postamble's denominator does not match preamble's",
		    DVIFileName);
		errs++;
	}
	if (GetLong(inf) != DVIMag) {
		error(0, 0,
		    "%s: postamble's magnification does not match preamble's",
		    DVIFileName);
		errs++;
	}

	/*
	 * Find maximum of tallest page height, widest page width, and
	 * stack size.
	 */
	t = GetLong(inf);
	if (t > TallestPageHeight)
		TallestPageHeight = t;
	t = GetLong(inf);
	if (t > WidestPageWidth)
		WidestPageWidth = t;
	w = GetWord(inf);
	if (w > DVIStackSize)
		DVIStackSize = w;

	/*
	 * The remainder of the file---number of pages, list of fonts,
	 * postpost, pointer, version, and filler---we simply ignore.
	 */
}

/*
 * Handle a preamble.
 * `firstone' is true if this is the first input file.
 * Return true iff something is wrong with the file.
 */
int
HandlePreAmble(firstone)
	int firstone;
{
	register int n, c;
	static char warn1[] = "%s: Warning: preamble %s of %ld";
	static char warn2[] = "does not match first file's value of %ld";

	if (getc(inf) != DVI_PRE) {
		error(0, 0, "%s does not begin with a preamble", DVIFileName);
		error(0, 0, "(are you sure it is a DVI file?)");
		errs++;
		return (1);
	}
	if (getc(inf) != DVI_VERSION) {
		error(0, 0, "%s is not a DVI version %d file", DVIFileName,
		    DVI_VERSION);
		errs++;
		return (1);
	}

	/* committed to DVI file: now most errors are fatal */
	if (firstone) {
		OutputNumerator = Numerator = GetLong(inf);
		OutputDenominator = Denominator = GetLong(inf);
	} else {
		Numerator = GetLong(inf);
		if (Numerator != OutputNumerator) {
			error(0, 0, warn1, DVIFileName, "numerator",
			    (long)Numerator);
			error(0, 0, warn2, (long)OutputNumerator);
			errs++;
		}
		Denominator = GetLong(inf);
		if (Denominator != OutputDenominator) {
			error(0, 0, warn1, DVIFileName, "denominator",
			    (long)Denominator);
			error(0, 0, warn2, (long)OutputDenominator);
			errs++;
		}
	}
	DVIMag = GetLong(inf);
	if (OutputMag == 0)
		OutputMag = DVIMag;
	else if (DVIMag != OutputMag) {
		error(0, 0,
		    "%s: Warning: magnification of %ld changed to %ld",
		    DVIFileName, (long)DVIMag, (long)OutputMag);
		errs++;
	}
	n = UnSign8(GetByte(inf));	/* comment length */
	if (firstone) {
		putbyte(outf, DVI_PRE);
		putbyte(outf, DVI_VERSION);
		PutLong(outf, Numerator);
		PutLong(outf, Denominator);
		PutLong(outf, OutputMag);

		CurrentPosition = 15 + n;	/* well, almost */
		putbyte(outf, n);
		while (--n >= 0) {
			c = GetByte(inf);
			putbyte(outf, c);
		}
	} else {
		while (--n >= 0)
			(void) GetByte(inf);
	}
	return (0);
}

/*
 * Read one DVI file, concatenating it with the previous one (if any)
 * or starting up the output file (otherwise).
 */
void
doit(name, fp)
	char *name;
	FILE *fp;
{
	static int started;

	DVIFileName = name;
	inf = fp;
	if (HandlePreAmble(started ? 0 : 1))
		return;
	SClear(FontFinder);
	HandleDVIFile();
	HandlePostAmble();
	started = 1;
}

main(argc, argv)
	int argc;
	register char **argv;
{
	register int c;
	register char *s;
	FILE *f;

	ProgName = *argv;

	/*
	 * Handle arguments.
	 */
	while ((c = getopt(argc, argv, "m:o:")) != EOF) {
		switch (c) {

		case 'm':
			if (OutputMag)
				goto usage;
			OutputMag = atoi(optarg);
			break;

		case 'o':
			if (outf != NULL)
				goto usage;
			if ((outf = fopen(optarg, "w")) == NULL)
				error(1, -1, "cannot write %s", optarg);
			break;

		case '?':
usage:
			(void) fprintf(stderr,
			    "Usage: %s [-m mag] [-o outfile] [files]\n",
			    ProgName);
			(void) fflush(stderr);
			exit(1);
		}
	}

	/* setup */
	if (outf == NULL)
		outf = stdout;
	if ((FontFinder = SCreate(sizeof(struct fontinfo *))) == 0)
		error(1, 0, "cannot create font finder (out of memory?)");
	StartOfLastPage = -1;

	/*
	 * Concatenate the named input file(s).
	 * We write a preamble based on the first input file.
	 */
	if (optind >= argc)
		doit("`stdin'", stdin);
	else {
		for (c = optind; c < argc; c++) {
			s = argv[c];
			if (*s == '-' && s[1] == 0)
				doit("`stdin'", stdin);
			else if ((f = fopen(s, "r")) == NULL) {
				error(0, -1, "cannot read %s", s);
				errs++;
			} else {
				doit(s, f);
				(void) fclose(f);
			}
		}
	}
	if (CurrentPosition)
		WritePostAmble();

	(void) fprintf(stderr, "Wrote %d page%s, %ld bytes\n",
	    NumberOfOutputPages, NumberOfOutputPages == 1 ? "" : "s",
	    (long)CurrentPosition);
	exit(errs ? 2 : 0);
	/* NOTREACHED */
}

/*
 * Handle a font definition.
 */
HandleFontDef(index)
	i32 index;
{
	register struct fontinfo *fi;
	register int i;
	register char *s;
	register i32 checksum, mag, designsize;
	register int n1, n2;
	struct fontinfo **p;
	char *name;
	int d = S_CREATE | S_EXCL;

	if ((p = (struct fontinfo **)SSearch(FontFinder, index, &d)) == NULL)
		if (d & S_COLL)
			error(1, 0, "font %ld already defined", (long)index);
		else
			error(1, 0, "cannot stash font %ld (out of memory?)",
			    (long)index);

	/* collect the font information */
	checksum = GetLong(inf);
	mag = GetLong(inf);
	designsize = GetLong(inf);
	n1 = UnSign8(GetByte(inf));
	n2 = UnSign8(GetByte(inf));
	i = n1 + n2;
	if ((s = malloc((unsigned)i)) == NULL)
		GripeOutOfMemory(i, "font name");
	for (name = s; --i >= 0;)
		*s++ = GetByte(inf);
	s = name;

	/*
	 * We have not seen the font before in this input file,
	 * but we may have seen it in a previous file, so we
	 * must search.
	 */
	i = n1 + n2;
	for (fi = fonts; fi != NULL; fi = fi->fi_next) {
		if (fi->fi_designsize == designsize &&
		    fi->fi_mag == mag &&
		    fi->fi_n1 == n1 && fi->fi_n2 == n2 &&
		    bcmp(fi->fi_name, s, i) == 0) {
			if (fi->fi_checksum == 0)
				fi->fi_checksum = checksum;
			else if (checksum && fi->fi_checksum != checksum) {
				error(0, 0, "\
%s: Warning: font checksum mismatch for %.*s detected",
				    DVIFileName, i, s);
				errs++;
			}
			*p = fi;
			return;
		}
	}
	/* it is really new; add it to the list */
	if ((fi = (struct fontinfo *)malloc(sizeof *fi)) == NULL)
		GripeOutOfMemory(sizeof *fi, "font information");
	fi->fi_next = fonts;
	fi->fi_index = NextOutputFontIndex++;
	fi->fi_checksum = checksum;
	fi->fi_mag = mag;
	fi->fi_designsize = designsize;
	fi->fi_n1 = n1;
	fi->fi_n2 = n2;
	fi->fi_name = s;
	fonts = fi;
	WriteFont(fi);
	*p = fi;
}

/*
 * Handle a \special.
 */
HandleSpecial(c, l, p)
	int c;
	register int l;
	register i32 p;
{
	register int i;

	putbyte(outf, c);
	switch (l) {

	case DPL_UNS1:
		putbyte(outf, p);
		CurrentPosition += 2;
		break;

	case DPL_UNS2:
		PutWord(outf, p);
		CurrentPosition += 3;
		break;

	case DPL_UNS3:
		Put3Byte(outf, p);
		CurrentPosition += 4;
		break;

	case DPL_SGN4:
		PutLong(outf, p);
		CurrentPosition += 5;
		break;

	default:
		panic("HandleSpecial l=%d", l);
		/* NOTREACHED */
	}
	CurrentPosition += p;
	while (--p >= 0) {
		i = getc(inf);
		putbyte(outf, i);
	}
	if (feof(inf))
		GripeUnexpectedDVIEOF();
	if (ferror(outf))
		error(1, -1, writeerr);
}

UseFont(index)
	register i32 index;
{
	struct fontinfo *fi, **p;
	int look = S_LOOKUP;

	p = (struct fontinfo **)SSearch(FontFinder, index, &look);
	if (p == NULL)
		error(1, 0, "%s requested font %ld without defining it",
		    (long)index);
	if ((fi = *p) == NULL)
		panic("null entry in FontFinder for %ld", (long)index);
	index = fi->fi_index;
	if (index < 64) {
		putbyte(outf, index + DVI_FNTNUM0);
		CurrentPosition++;
	} else if (index < 256) {
		putbyte(outf, DVI_FNT1);
		putbyte(outf, index);
		CurrentPosition += 2;
	} else if (index < 65536) {
		putbyte(outf, DVI_FNT2);
		PutWord(outf, index);
		CurrentPosition += 3;
	} else if (index < 16777216) {
		putbyte(outf, DVI_FNT3);
		Put3Byte(outf, index);
		CurrentPosition += 4;
	} else {
		putbyte(outf, DVI_FNT4);
		PutLong(outf, index);
		CurrentPosition += 5;
	}
}

/*
 * The following table describes the length (in bytes) of each of the DVI
 * commands that we can simply copy, starting with DVI_SET1 (128).
 */
char	oplen[128] = {
	2, 3, 4, 5,		/* DVI_SET1 .. DVI_SET4 */
	9,			/* DVI_SETRULE */
	2, 3, 4, 5,		/* DVI_PUT1 .. DVI_PUT4 */
	9,			/* DVI_PUTRULE */
	1,			/* DVI_NOP */
	0,			/* DVI_BOP */
	0,			/* DVI_EOP */
	1,			/* DVI_PUSH */
	1,			/* DVI_POP */
	2, 3, 4, 5,		/* DVI_RIGHT1 .. DVI_RIGHT4 */
	1,			/* DVI_W0 */
	2, 3, 4, 5,		/* DVI_W1 .. DVI_W4 */
	1,			/* DVI_X0 */
	2, 3, 4, 5,		/* DVI_X1 .. DVI_X4 */
	2, 3, 4, 5,		/* DVI_DOWN1 .. DVI_DOWN4 */
	1,			/* DVI_Y0 */
	2, 3, 4, 5,		/* DVI_Y1 .. DVI_Y4 */
	1,			/* DVI_Z0 */
	2, 3, 4, 5,		/* DVI_Z1 .. DVI_Z4 */
	0,			/* DVI_FNTNUM0 (171) */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 172 .. 179 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 180 .. 187 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 188 .. 195 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 196 .. 203 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 204 .. 211 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 212 .. 219 */
	0, 0, 0, 0, 0, 0, 0, 0,	/* 220 .. 227 */
	0, 0, 0, 0, 0, 0, 0,	/* 228 .. 234 */
	0, 0, 0, 0,		/* DVI_FNT1 .. DVI_FNT4 */
	0, 0, 0, 0,		/* DVI_XXX1 .. DVI_XXX4 */
	0, 0, 0, 0,		/* DVI_FNTDEF1 .. DVI_FNTDEF4 */
	0,			/* DVI_PRE */
	0,			/* DVI_POST */
	0,			/* DVI_POSTPOST */
	0, 0, 0, 0, 0, 0,	/* 250 .. 255 */
};

/*
 * Here we read the input DVI file and copy the pages to the output
 * DVI file, renumbering fonts.  We also keep track of font changes,
 * handle font definitions, and perform some other housekeeping.
 */
HandleDVIFile()
{
	register int c, l;
	register i32 p;
	int doingpage = 0;

	/* Only way out is via "return" statement */
	for (;;) {
		c = getc(inf);	/* getc() returns unsigned values */
		if (DVI_IsChar(c)) {
			putbyte(outf, c);
			CurrentPosition++;
			continue;
		}
		if (DVI_IsFont(c)) {	/* note font change */
			UseFont((i32)(c - DVI_FNTNUM0));
			continue;
		}
		if (c == EOF)
			GripeUnexpectedDVIEOF();
		if ((l = (oplen - 128)[c]) != 0) {	/* simple copy */
			CurrentPosition += l;
			putbyte(outf, c);
			while (--l > 0) {
				c = getc(inf);
				putbyte(outf, c);
			}
			if (ferror(outf))
				error(1, -1, writeerr);
			continue;
		}
		if ((l = DVI_OpLen(c)) != 0) {
			/*
			 * Handle other generics.
			 * N.B.: there should only be unsigned parameters
			 * here (save SGN4), for commands with negative
			 * parameters have been taken care of above.
			 */
			switch (l) {

			case DPL_UNS1:
				p = getc(inf);
				break;

			case DPL_UNS2:
				fGetWord(inf, p);
				break;

			case DPL_UNS3:
				fGet3Byte(inf, p);
				break;

			case DPL_SGN4:
				fGetLong(inf, p);
				break;

			default:
				panic("HandleDVIFile l=%d", l);
			}

			/*
			 * Now that we have the parameter, perform the
			 * command.
			 */
			switch (DVI_DT(c)) {

			case DT_FNT:
				UseFont(p);
				continue;

			case DT_XXX:
				HandleSpecial(c, l, p);
				continue;

			case DT_FNTDEF:
				HandleFontDef(p);
				continue;

			default:
				panic("HandleDVIFile DVI_DT(%d)=%d",
				    c, DVI_DT(c));
			}
			continue;
		}

		switch (c) {	/* handle the few remaining cases */

		case DVI_BOP:
			if (doingpage)
				GripeUnexpectedOp("BOP (during page)");
			BeginPage();
			doingpage = 1;
			break;

		case DVI_EOP:
			if (!doingpage)
				GripeUnexpectedOp("EOP (outside page)");
			EndPage();
			doingpage = 0;
			break;

		case DVI_PRE:
			GripeUnexpectedOp("PRE");
			/* NOTREACHED */

		case DVI_POST:
			if (doingpage)
				GripeUnexpectedOp("POST (inside page)");
			return;

		case DVI_POSTPOST:
			GripeUnexpectedOp("POSTPOST");
			/* NOTREACHED */

		default:
			GripeUndefinedOp(c);
			/* NOTREACHED */
		}
	}
}
