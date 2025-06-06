/* -*-C-*- LPTOPS.C */
/*-->lptops*/
/**********************************************************************/
/******************************* lptops *******************************/
/**********************************************************************/

/***********************************************************************

NB: In order to handle overprinting of input of the form

...text...<CR>...overtext...<CR><LF>

this must be loaded with a special version of the C library I/O  routine
(PCC:IOCR.REL on the DEC-20) which permits reading and writing of  <CR>.
No special  action need  be taken  on Unix  systems, since  <CR> is  not
filtered out, and is normally absent.	On PC-DOS, open options of  "rb"
will usually  be necessary  (this  is the  default  on all  systems  but
TOPS-20).
***********************************************************************/

/***********************************************************************
	       Line printer file to PostScript converter
			      [03-Jul-85]
			      [12-Nov-85]
			      [07-Apr-86]
                              [03-May-88]

lptops converts normal text  files into PostScript  for printing on  the
Apple LaserWriter, or any  other PostScript-compatible printing  device.
It supports selection of  a variety of fonts  at arbitrary point  sizes,
margin specification, portrait and landscape page orientation, automatic
page numbering, page outlining, and multi-column printing.  It can  also
handle overstruck text used by many document formatters for underlining.

Usage:

	lptops [switches] <infile >outfile
	lptops [switches] filelist >outfile

In the switch descriptions below, physical dimensions described as # may
be specified as a single number with an optional fractional part, and  a
required two-letter unit designator.  Letter case is ignored.	Possible
forms are:

#.##bp  big point (1in = 72bp)	     [Courier: 120.000/Pbp cpi]
#.##cc  cicero (1cc = 12dd)	     [Courier:	 9.381/Pcc cpi]
#.##cm  centimeter		     [Courier:	 4.233/Pcm cpi]
#.##dd  didot point (1157dd = 1238pt)[Courier: 112.569/Pdd cpi]
#.##in  inch			     [Courier:	 1.667/Pin cpi]
#.##mm  millimeter (10mm = 1cm)      [Courier:  42.333/Pmm cpi]
#.##pc  pica (1pc = 12pt)	     [Courier:  10.038/Ppc cpi]
#.##pt  point (72.27pt = 1in)	     [Courier: 120.450/Ppt cpi]
#.##sp  scaled point (65536sp = 1pt) [Courier:	 7.893/(Psp/1000000) cpi]

The bracketed values give the number of characters per inch (cpi) for  a
point size of P units with  the fixed-width Courier font.  For  example,
with point size  8bp, there are  120/8 = 15  characters per inch.   With
point size 1000000sp, there are 7.893 characters/inch.

The optional switches are (letter case is ignored):

	-A	Turn Scribe-like bold and italic requests (@b[...] and
		@i[...]) into bold or italic text.

	-B#	Bottom margin of #.

	-Cn	Print n copies of each page.

	-Ffontname
		PostScript fontname.   Possible values  with  acceptable
		short abbreviations formed  from the upper-case  letters
		in the font names, and the ditroff and Adobe  TranScript
		abbreviations  are  as  follows.    The  first  13   are
		available on all PostScript printers; the remainder  are
		available only on enhanced  printers, such as the  Apple
		LaserWriter Plus.

		====================		=====	======	==========
		full name			short	ditroff	TranScript
		====================		=====	======	==========
		Courier				C	C	C
		Courier-Bold			CB	CB	CB
		Courier-BoldOblique		CBO	CD	CD
		Courier-Oblique			CO	CO	CO
		Helvetica			H	he	he
		Helvetica-Bold			HB	He	He
		Helvetica-BoldOblique		HBO	HE	HE
		Helvetica-Oblique		HO	hE	hE
		Symbol				S	S	S
		Times-Bold			TB	R	ti
		Times-BoldItalic		TBI	BI	TI
		Times-Italic			TI	I	tI
		Times-Roman			T	R	ti

		AvantGarde-Book			AGB	ag	ag
		AvantGarde-BookOblique		AGBO	aG	aG
		AvantGarde-Demi			AGD	Ag	Ag
		AvantGarde-DemiOblique		AGDO	AG	AG
		Bookman-Demi			BD	Bo	Bo
		Bookman-DemiItalic		BDI	BO	BO
		Bookman-Light			BL	bo	bo
		Bookman-LightItalic		BLI	bO	bO
		Helvetica-Narrow		HN	hn	hn
		Helvetica-Narrow-Bold		HNB	Hn	Hn
		Helvetica-Narrow-BoldOblique	HNBO	HN	HN
		Helvetica-Narrow-Oblique	HNO	hN	hN
		NewCenturySchlbk-Bold		NCSB	Nc	Nc
		NewCenturySchlbk-BoldItalic	NCSBI	NC	NC
		NewCenturySchlbk-Italic		NCSI	nC	nC
		NewCenturySchlbk-Roman		NCSR	nc	nc
		Palatino-Bold			PB	Pa	Pa
		Palatino-BoldItalic		PBI	PA	PA
		Palatino-Italic			PI	pA	pA
		Palatino-Roman			PR	pa	pa
		ZapfChancery-MediumItalic	ZCMI	ZC	ZC
		ZapfDingbats			ZD	ZD	ZD
		====================		=====	======	==========

		The ditroff and  TranScript  abbreviations are the same,
		except for the Courier and Times fonts.  They  are based
		on two-letter  mnemonics, where upper-case  in the first
		letter means bold, and upper-case  in the second  letter
		means italic.

		Only the Courier fonts  are fixed-width like  typewriter
		and line printer fonts are.  The others are proportional
		spaced  for  improved  readability,  and   consequently,
		tabular material will  not line up  properly with  them.
		The Courier fonts  have a  width equal to  0.6 of  their
		point size, so to fill a line W inches wide with up to N
		characters, one must have  point size <=  (W/(0.6 * N  *
		72)) = (W*120)/N pt.  Equivalently, with a point size  P
		pt, the output spacing is 120/P char/inch.

	-H	Horizontal page orientation (landscape mode) instead  of
		vertical page (portrait mode).

	-I#	Top margin of # for  initial page (for letterheads);  if
		not specified, it  will default to  the value given  for
		the top margin by default or by the -T# command.

	-L#	Left margin of #.

	-Mn	Multiple column output (n columns).  In multiple  column
		mode, formfeeds  cause  column breaks  instead  of  page
		breaks.

	-N[tb][lcr]n
		Number output pages.  The number is placed by default in
		the center  of the  top margin,  but the  margin can  be
		selected explicitly by  t (top) or  b (bottom), and  the
		position of the page number can be further specified  by
		l (left),  c (center),  or r  (right).  If  n is  given,
		start numbering with that  value, and otherwise with  1.
		If an initial  top margin  value is given  with the  -I#
		option, numbering will be omitted  on the first page  if
		it is at the top.   Pages are unnumbered if this  option
		is not specified.

	-O[#]	Outline pages  (and  columns) with  a  # units  wide.  A
		narrow line of 0.4pt width  is assumed if # is  omitted;
		this particular width is what TeX uses in typesetting.

	-P#	Font point  size  (default  10bp).   The  baseline  skip
		(spacing  between  successive  character  baselines)  is
		conventionally chosen to be 6/5  of this value, so  that
		with 10bp  type,  we  have  12bp  between  lines,  or  6
		lines/inch, the same as standard printer output.

	-R#	Right margin of #.

	-S[n]	special handling  (i.e. manual paper feed) for the first
		n pages  of the  output.  If  N multiple  copies are  in
		effect, then N*n sheets must  be fed manually.  If n  is
		omitted, then all  pages are  to be  fed manually.   For
		example, -S1 is convenient  to get the  first page of  a
		letter on letterhead paper fed manually, with  remaining
		pages from the paper cassette.

	-T#	Top margin of #.

	-U	Output pages in unsorted  (first to last) order  instead
		of in  sorted  (page reversed)  order;  some  PostScript
		printers have page handlers that stack the paper in this
		order.	 The   default  is   sorted  because   this   is
		appropriate for the most common PostScript printer,  the
		Apple LaserWriter.

Defaults are:

	-B1.0in -C1 -FCourier -L1.0in -M1 -P10bp -R1.0in -S0 -T1.0in


Revision history (reverse time order):

[03-May-88]	Add ditroff and Adobe TranScript font abbreviations.

[19-Apr-86]	Change page/line status report from output in End_Page()
		only to output in Beg_Page() and End_Column().

[07-Apr-86]	Fix some  incorrect  font  names, put  font  names  into
		table, add font abbreviations and new font names, change
		default font to Courier  (because most printing  expects
		fixed-width fonts).  Add usage  message if bad  options,
		and print  fontnames if  bad font  switch given.   Allow
		multiple filenames on command line instead of just using
		stdin -- this added  new function Do_File() and  changed
		argument  lists  of  inchar()  and  Copy_Plist().    Add
		save/restore around each page  image following new  1986
		PostScript Language Reference Manual recommendation, and
		change multiple  copies  sequence from  repeat  loop  to
		setting of standard variable #copies.  Add -U option for
		unsorted output.

[03-Feb-86]	Fix setting of final  entry of page_table[].  It  should
		only be done when End_Page() is called, since  otherwise
		Beg_Page() has already set the final entry, and we  just
		have an empty page to  be discarded.  Without this  fix,
		this left a hole (0), so  the entire file got copied  in
		its entirety to the last page of the sorted file.

[18-Jan-86]	Add (void) to fprintf()/printf(), test for disk  storage
		full at end  of page, add  OUTSTR() macro using  fputs()
		instead of fprintf() for efficiency.

[20-Dec-85]	Change getchar() to inchar(); the latter supplies a
		trailing LF at EOF in the event it is missing.
		Otherwise, the output PostScript has two tokens
		jammed together (P 1 {copypage} --> P1 {copypage})

[20-Dec-85]	Following recommendation of Allan Hetzel on LASER-LOVERS
		ARPA BBOARD, added "() pop" at top of every page.  This
		is selected by the switch FIX31OCT85:

		--------------------------------------------------------
		From: Allan Hetzel  <SYSAL%UKCC.BITNET@WISCVM.ARPA>
		Subject: Re: Apple Laserwriter XON/XOFF problem
		To:  <LASER-LOVERS@WASHINGTON.ARPA>

		The note  posted to  Laser-Lovers on  Oct 28  by  Joseph
		Goldstone of  Symbolics  Cambridge Research  Center  was
		helpful but I didn't try his suggested fix correctly and
		so I called Adobe.  They were very helpful (much more so
		than Apple) and explained the problem and how to  bypass
		it.

		My  apologies  to   Adobe  if  this   not  an	accurate
		description of the problem.   The problem apparently  is
		due to the  PostScript interpreter  getting confused  by
		what it thinks is an excess of white space characters.

		The bypass is to place a special character (parenthesis,
		bracket, brace, slash, etc.) to the right of some token,
		but without an intervening white space character. As the
		PostScript scanner  reads the  token it  also reads  the
		special character which acts as a delimiter. The scanner
		then has  to  back  up  one character  so  that  it  can
		reprocess the special character  after it has  processed
		the preceding token.  During  this backing up process  a
		number of  internal values  are recalculated,  including
		some pointer  into the  input buffer.	This causes  the
		XON/XOFF protocol to work properly.  Doing this once per
		page seems to keep everybody happy.

		Each page  in  the PostScript  file  built by  our  word
		processing program  is surrounded  by a  "save  restore"
		sequence.  We  changed	 the  beginning  save	sequence
		"/saveobj save def" to  read "/saveobj save def()  pop".
		The "() pop" is effectively  a no-op and we are  assured
		that the necessary recalculations are done on each page.
		This seems to have corrected the problem completely.

		Allan Hetzel (sysal@ukcc.bitnet)
		--------------------------------------------------------

[11-Dec-85]	Fix problem with extra () around overstruck text.

[13-Nov-85]	Insert FASTIO conditional compilation option to speed up
		page reversal.  Under VAX  4.2BSD, fread() and  fwrite()
		are already fast enough, and need not be replaced.

[12-Nov-85]	Add reversal of  pages, storing  output on  intermediate
		file which is deleted before exiting.

[02-Nov-85]	Add code to write output on temporary file, then
		retrieve in reverse page order and output on stdout.

[30-Oct-85]	Add code to handle overstruck lines:
		...text...<CR>...overtext...<CR><LF>
		will appear as
		(...text...) O
		(...overtext...) P

		Macro definitions and font selection now repeated
		on each page to allow selective reprinting.

[07-Oct-85]	Add code to discard input NUL's from word-padded files,
		since the LaserWriter prints them as blanks.

[27-Sep-85]	Added missing 'N' switch, fixed Page_Switch() (it scanned
		beyond last digit of page number)

[11-Jul-85]	Original version.


***********************************************************************/


#ifndef OS_TOPS20
#define OS_TOPS20	 0
#endif

#ifndef KCC_20
#define KCC_20		 0
#endif

#ifndef PCC_20
#define PCC_20		 0
#endif

#if    (KCC_20 | PCC_20)
#ifndef OS_TOPS20
#define OS_TOPS20	 1
#endif
#endif

#ifndef OS_VAXVMS
#define OS_VAXVMS	 0
#endif

#ifndef OS_ATARI
#define OS_ATARI	 0
#endif

#ifndef OS_PCDOS
#define OS_PCDOS	 0
#endif

#ifndef IBM_PC_AZTEC
#define IBM_PC_AZTEC	 0
#endif

#ifndef IBM_PC_LATTICE
#define IBM_PC_LATTICE	 0
#endif

#ifndef IBM_PC_MICROSOFT
#define IBM_PC_MICROSOFT 0
#endif

#ifndef IBM_PC_WIZARD
#define IBM_PC_WIZARD	 0
#endif

#if    (IBM_PC_AZTEC | IBM_PC_LATTICE | IBM_PC_MICROSOFT | IBM_PC_WIZARD)
#ifndef OS_PCDOS
#define OS_PCDOS	 1
#endif
#endif

#if    (OS_ATARI | OS_TOPS20 | OS_PCDOS | OS_VAXVMS)
#define OS_UNIX		 0
#else
#define OS_UNIX		 1
#endif

#if    (OS_ATARI | PCC_20)
#define DISKFULL(fp)	ferror(fp)	/* PCC-20 does not always set errno */
#else
#define DISKFULL(fp)	(ferror(fp) && (errno == ENOSPC))
#endif

#if    OS_VAXVMS
#define EXIT		vms_exit
#else
#define EXIT		exit
#endif

#if    IBM_PC_LATTICE
typedef int void;		/* Lattice C does not have this */
#endif

#if    IBM_PC_MICROSOFT
#define ANSI_PROTOTYPES		1
#endif

#ifndef ANSI_PROTOTYPES
#define ANSI_PROTOTYPES		0
#endif

#ifndef ANSI_LIBRARY
#define ANSI_LIBRARY		0
#endif

/*
************************************************************************
On  TOPS-20,   fread()/fwrite()   are  implemented   as   loops   around
getc()/putc() calls, and these in turn also do CR/LF translation.  Calls
to _read() and _write() translate into  direct calls to the Monitor  for
fast efficient block I/O.   FASTIO selects this option.   It is used  in
Copy_Block when the temporary  file is output in  reverse page order  to
stdout.
************************************************************************
*/

#if    OS_TOPS20
#if    PCC_20
#define FASTIO 1			/* TOPS-20 use only */
#undef DISKFULL
#define DISKFULL(fp)	ferror(fp)	/* PCC-20 does not always set errno */
extern	int strcmp();
#endif
#endif

#define FIX31OCT85 1			/* Allan Hetzel's XON/XOFF fix */

typedef int BOOLEAN;

#define FALSE 0
#define TRUE 1

#ifndef FASTIO
#define FASTIO		0
#endif

#define PXLID		0		/* used in inch.h */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#if    IBM_PC_MICROSOFT
/* int errno; */		/* not in errno.h */
#include <stdlib.h>
#include <time.h>
#endif

#if    OS_TOPS20
#include <time.h>
#endif

#if    FASTIO
#include <file.h>
#include <jsys.h>
#include <mon_s.h>
#endif

#if    (OS_TOPS20 | OS_UNIX | OS_VAXVMS | IBM_PC_AZTEC)
#define FOPEN_W "w"
#define FOPEN_R "r"
#else
#define FOPEN_W "wb"
#define FOPEN_R "rb"
#endif

/***********************************************************************
PostScript's basic units  are big  points (72bp =  1in).  Everything  in
this routine works in terms of "Quite Small Units": 1 inch = QSU qsu.
***********************************************************************/


#define NUL '\0'

#define QSU 100000L		/* quite small units per inch -- */
				/* should be large to avoid */
				/* truncation error accumulation */

#define INCH_TO_QSU(x) ((COORDINATE)(x * (float)QSU))
				/* inch to QSU conversion */

#define LEFT_X (left_margin + col_margin)
#define BOT_Y  (bottom_margin + col_margin)
#define TOP_Y(margin) (page_height - (margin) - baseline_skip - col_margin)

#if    OS_TOPS20	/* want <CR><LF> */
#define NEWLINE(fp) {(void)putc((char)'\r',fp);(void)putc((char)'\n',fp);}
#else
#define NEWLINE(fp) (void)putc((char)'\n',fp)	/* want bare <LF> */
#endif

#define OUTCHAR(c) (void)putc((char)(c),tf)
#define OUTOCTAL(c) (void)fprintf(tf,"\\%03o",(c))
#define OUTSTR(s) (void)fputs(s,tf)

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define HEIGHT (11L * QSU)	/* page height in QSU */
#define MAXBLOCK 4096L		/* block size for file copy */
#define MAXLEN 256		/* font name length */
#define MAXPAGE 1024		/* size of page location table */
#define WIDTH ((85L * QSU)/10L)	/* page width in QSU */
#define COL_TWIDDLE 40L		/* 1/fraction of column width for */
				/* column margin */

#define PAGE_T 001		/* flag bits for page_flag */
#define PAGE_B 002
#define PAGE_L 010
#define PAGE_R 020
#define PAGE_C 040

#define TEMPFILE "lptops.tmp"	/* temporary output file */


				/* global functions */
#if    ANSI_PROTOTYPES
int  main(int ,char *[]);
void Beg_Column(void);
void Beg_Line(void);
void Beg_Page(void);
void Beg_Text(char);
void Copy_Block(long ,long);
void Copy_Plist(char, FILE *);
void Do_File(FILE *);
void End_Column(void);
void End_Page(void);
void End_Line(void);
void End_Text(char);
float inch(char *);
int inchar(FILE *);
void Page_Switch(char *);

#if    ANSI_LIBRARY
int     atoi(const char *);
char*   ctime(const time_t *);
#else /* NOT ANSI_LIBRARY */
int     atoi(char *);
char*   ctime(long *);
#endif /* ANSI_LIBRARY */

char *cuserid(char *);
void EXIT(int);
char *getlogin(void);

#if    ANSI_LIBRARY
void perror(const char *);
char *strcpy(char *,const char *);
int strcm2(char *,char *);
size_t strlen(const char *);
char *strncpy(char *,const char *,size_t);
time_t time(time_t *);
int unlink(const char *);
#else /* NOT ANSI_LIBRARY */
void perror(char *);
char *strcpy(char *,char *);
int strcm2(char *,char *);
int strlen(char *);
char *strncpy(char *,char *,int);
long time(long *);
int unlink(char *);
#endif /* ANSI_LIBRARY */

#else
void Beg_Column();
void Beg_Line();
void Beg_Page();
void Beg_Text();
void Copy_Block();
void Copy_Plist();
void Do_File();
void End_Column();
void End_Page();
void End_Line();
void End_Text();
float inch();
int  inchar();
int  main();
void Page_Switch();

int  atoi();
char *ctime();
char *cuserid();
void EXIT();
char *getlogin();
void perror();
char *strcpy();
int  strcm2();
int  strlen();
char* strncpy();
long time();
int  unline();
#endif

typedef long COORDINATE;	/* variables in QSU need >=32 bits */


				/* global variables */

FILE *tf;			/* temporary output file */
BOOLEAN bad_option;		/* bad option flag */
COORDINATE baseline_skip;	/* inter-line spacing */
COORDINATE bottom_margin;	/* bottom margin width in QSU */
int  column;			/* current text column (1..numcol) */
COORDINATE col_margin;		/* column margin (extra space */
				/* between column borders in */
				/* multi-column mode) */
COORDINATE col_width;		/* column width in QSU */
int  copies;			/* number of copies of each page */
BOOLEAN  do_page_number;	/* page numbering flag */
char font_name[MAXLEN+1];	/* font name -- normal */
char font_bold[MAXLEN+1];	/* font name -- bold */
char font_italic[MAXLEN+1];	/* font name -- italic */
BOOLEAN found;			/* search flag */
int  in_col;			/* input line column */
int  init_page_number;		/* initial page number */
COORDINATE init_top_margin;	/* initial page top margin in QSU */
BOOLEAN landscape_mode;		/* horizontal landscape mode output */
COORDINATE left_margin;		/* left margin in QSU */
int  level;			/* parenthesis level */
int  line_number;		/* current line number */
int  manual_feed;		/* manual feed page count */
int  numcol;			/* number of text columns */
COORDINATE outline;		/* page and column outline linewidth */
int  page_flag;			/* page number location flag */
COORDINATE page_height;		/* page height in QSU */
int  page_number;		/* current page number */
long page_table[MAXPAGE+1];	/* page location table (with space */
				/* for extra entry marking EOF) */
COORDINATE page_width;		/* page width in QSU */
COORDINATE point_size;		/* font point size in big points */
BOOLEAN reverse;		/* page reversal flag */
COORDINATE right_margin;	/* right margin in QSU */
BOOLEAN scribe;			/* recognize bold/italic sequences */
COORDINATE top_margin;		/* top margin in QSU */
COORDINATE x;			/* current horizontal coordinate */
COORDINATE y;			/* current vertical coordinate */

/***********************************************************************
Built-in PostScript font tables.  The first column in each list contains
the full (case sensitive) PostScript  names, the second column  contains
convenient short abbreviations  which can  be used in  the command  line
-ffontname option,  the third  and fourth  columns contain  ditroff  and
Adobe TranScript abbreviations.

The  first  four  lines  of   entries  are  available  on  the   regular
LaserWriter, and the  remainder are  available only  on the  LaserWriter
Plus.
***********************************************************************/

char* ps_normal[] =
{
    /* fullname				short	ditroff	TranScript */
    "Courier",				"C",	"C",	"C",
    "Helvetica",			"H",	"he",	"he",
    "Times-Roman",			"T",	"R",	"ti",
    "Symbol",				"S",	"S",	"S",
    "AvantGarde-Book",			"AGB",	"ag",	"ag",
    "AvantGarde-Demi",			"AGD",	"Ag",	"Ag",
    "Bookman-Demi",			"BD",	"Bo",	"Bo",
    "Bookman-Light",			"BL",	"bo",	"bo",
    "Helvetica-Narrow",			"HN",	"hn",	"hn",
    "NewCenturySchlbk-Roman",		"NCSR",	"nc",	"nc",
    "Palatino-Roman",			"PR",	"pa",	"pa",
    "ZapfChancery-MediumItalic",	"ZCMI",	"ZC",	"ZC",
    "ZapfDingbats",			"ZD",	"ZD",	"ZD"
};

char* ps_bold[] =
{
    /* fullname				short	ditroff	TranScript */
    "Courier-Bold",			"CB",	"CB",	"CB",
    "Helvetica-Bold",			"HB",	"He",	"He",
    "Times-Bold",			"TB",	"B",	"Ti",
    "Symbol",				"S",	"S",	"S",
    "AvantGarde-Book",			"AGB",	"ag",	"ag",
    "AvantGarde-Demi",			"AGD",	"Ag",	"Ag",
    "Bookman-Demi",			"BD",	"Bo",	"Bo",
    "Bookman-Light",			"BL",	"bo",	"bo",
    "Helvetica-Narrow-Bold",		"HNB",	"Hn",	"Hn",
    "NewCenturySchlbk-Bold",		"NCSB",	"Nc",	"Nc",
    "Palatino-Bold",			"PB",	"Pa",	"Pa",
    "ZapfChancery-MediumItalic",	"ZCMI",	"ZC",	"ZC",
    "ZapfDingbats",			"ZD",	"ZD",	"ZD"
};

char* ps_italic[] =
{
    /* fullname				short	ditroff	TranScript */
    "Courier-Oblique",			"CO",	"CO",	"CO",
    "Helvetica-Oblique",		"HO",	"hE",	"hE",
    "Times-Italic",			"TI",	"I",	"tI",
    "Symbol",				"S",	"S",	"S",
    "AvantGarde-BookOblique",		"AGBO",	"aG",	"aG",
    "AvantGarde-DemiOblique",		"AGDO",	"AG",	"AG",
    "Bookman-DemiItalic",		"BDI",	"BO",	"BO",
    "Bookman-LightItalic",		"BLI",	"bO",	"bO",
    "Helvetica-Narrow-Oblique",		"HNO",	"hN",	"hN",
    "NewCenturySchlbk-Italic",		"NCSI",	"nC",	"nC",
    "Palatino-Italic",			"PI",	"pA",	"pA",
    "ZapfChancery-MediumItalic",	"ZCMI",	"ZC",	"ZC",
    "ZapfDingbats",			"ZD",	"ZD",	"ZD"
};

char* ps_both[] =
{
    /* fullname				short	ditroff	TranScript */
    "Courier-BoldOblique",		"CBO",	"CD",	"CD",
    "Helvetica-BoldOblique",		"HBO",	"HE",	"HE",
    "Times-BoldItalic",			"TBI",	"BI",	"TI",
    "Symbol",				"S",	"S",	"S",
    "AvantGarde-BookOblique",		"AGBO",	"aG",	"aG",
    "AvantGarde-DemiOblique",		"AGDO",	"AG",	"AG",
    "Bookman-DemiItalic",		"BDI",	"BO",	"BO",
    "Bookman-LightItalic",		"BLI",	"bO",	"bO",
    "Helvetica-Narrow-BoldOblique",	"HNBO",	"HN",	"HN",
    "NewCenturySchlbk-BoldItalic",	"NCSBI","NC",	"NC",
    "Palatino-BoldItalic",		"PBI",	"PA",	"PA",
    "ZapfChancery-MediumItalic",	"ZCMI",	"ZC",	"ZC",
    "ZapfDingbats",			"ZD",	"ZD",	"ZD"
};


#define PS_DEFAULT 0	/* index of default font in ps_normal[] */
#define MAXFONT (sizeof(ps_normal)/sizeof(char *)) /* size of font table */


int
main(argc,argv)
int argc;
char* argv[];
{
    register int k;			/* loop index */
    register char *p;			/* temporary pointer */
    register int m,n;			/* loop indices */
    FILE *fp;				/* input file pointer */
    char timestr[27];			/* result of ctime() */
    long timeval;			/* result of time() */
    long last,next;			/* result of fseek() for */
					/* page_table[] construction */
    int (*compare)();

    /* establish defaults for all parameters */
    bottom_margin = 1L * QSU;
    column = 1;
    col_margin = 0L;
    copies = 1;
    do_page_number = FALSE;
    init_page_number = 1;
    init_top_margin = -1;	/* negative value flags unset state */
    in_col = 0;
    landscape_mode = FALSE;
    left_margin = 1L * QSU;
    manual_feed = 0;
    numcol = 1;
    outline = 0L;
    page_height = HEIGHT;
    page_number = init_page_number;
    page_width = WIDTH;
    point_size = INCH_TO_QSU(inch("10bp"));
    reverse = TRUE;
    right_margin = 1L * QSU;
    scribe = FALSE;
    top_margin = 1L * QSU;

    strcpy(font_name,ps_normal[PS_DEFAULT]);
    strcpy(font_bold,ps_bold[PS_DEFAULT]);
    strcpy(font_italic,ps_italic[PS_DEFAULT]);

    bad_option = FALSE;
    for (k = 1; k < argc; ++k)
    {
	p = argv[k];
	if (argv[k][0] == '-')
	{
	    switch(argv[k][1])
	    {
	    case 'a':
	    case 'A':
		scribe = TRUE;
		break;

	    case 'b':
	    case 'B':
		bottom_margin = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 'c':
	    case 'C':
		copies = atoi(&argv[k][2]);
		break;

	    case 'f':
	    case 'F':
		(void)strncpy(font_name,&argv[k][2],MAXLEN);
		font_name[MAXLEN] = '\0';
		found = FALSE;
		for (n = 0; n < MAXFONT; n += 4)
		{
		    for (m = 3; m >= 0; --m)
		    {
			/* compare case-sensitive ditroff and TranScript
			   names before case-insensitive ones */
			compare = (m > 1) ? strcmp : strcm2;
			if ((*compare)(font_name,ps_normal[n+m]) == 0)
			{
			    strcpy(font_name,ps_normal[n]);
			    strcpy(font_bold,ps_bold[n]);
			    strcpy(font_italic,ps_italic[n]);
			    found = TRUE;
			    break;
			}
			else if ((*compare)(font_name,ps_bold[n+m]) == 0)
			{
			    strcpy(font_name,ps_bold[n]);
			    strcpy(font_bold,ps_bold[n]);
			    strcpy(font_italic,ps_italic[n]);
			    found = TRUE;
			    break;
			}
			else if ((*compare)(font_name,ps_italic[n+m]) == 0)
			{
			    strcpy(font_name,ps_italic[n]);
			    strcpy(font_bold,ps_both[n]);
			    strcpy(font_italic,ps_normal[n]);
			    found = TRUE;
			    break;
			}
			else if ((*compare)(font_name,ps_both[n+m]) == 0)
			{
			    strcpy(font_name,ps_both[n]);
			    strcpy(font_bold,ps_italic[n]);
			    strcpy(font_italic,ps_normal[n]);
			    found = TRUE;
			    break;
			}
		    }
		}
		if (!found)
		{
		    NEWLINE(stderr);
    		    (void)fprintf(stderr,
			"?Illegal font name [%s].  Possible values are:",
			font_name);
		    NEWLINE(stderr);

		    NEWLINE(stderr);
		    (void)fprintf(stderr,"Normal fonts");
		    NEWLINE(stderr);
		    for (m = 0; m < MAXFONT; m += 4)
		    {
			(void)fprintf(stderr,"\t%-30s %-10s %-10s %-10s",
			    ps_normal[m],ps_normal[m+1],ps_normal[m+2],
			    ps_normal[m+3]);
			NEWLINE(stderr);
		    }

		    NEWLINE(stderr);
		    (void)fprintf(stderr,"Bold fonts");
		    NEWLINE(stderr);
		    for (m = 0; m < MAXFONT; m += 4)
		    {
			(void)fprintf(stderr,"\t%-30s %-10s %-10s %-10s",
			    ps_bold[m],ps_bold[m+1],ps_bold[m+2],
			    ps_bold[m+3]);
			NEWLINE(stderr);
		    }

		    NEWLINE(stderr);
		    (void)fprintf(stderr,"Oblique/Italic fonts");
		    NEWLINE(stderr);
		    for (m = 0; m < MAXFONT; m += 4)
		    {
			(void)fprintf(stderr,"\t%-30s %-10s %-10s %-10s",
			    ps_italic[m],ps_italic[m+1],ps_italic[m+2],
			    ps_italic[m+3]);
			NEWLINE(stderr);
		    }

		    NEWLINE(stderr);
		    (void)fprintf(stderr,"BoldOblique/BoldItalic fonts");
		    NEWLINE(stderr);
		    for (m = 0; m < MAXFONT; m += 4)
		    {
			(void)fprintf(stderr,"\t%-30s %-10s %-10s %-10s",
			    ps_both[m],ps_both[m+1],ps_both[m+2],
			    ps_both[m+3]);
			NEWLINE(stderr);
		    }
		    NEWLINE(stderr);
		    bad_option = TRUE;
		}
		break;

	    case 'h':
	    case 'H':
		landscape_mode = TRUE;
		break;

	    case 'i':
	    case 'I':
		init_top_margin = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 'l':
	    case 'L':
		left_margin = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 'm':
	    case 'M':
		numcol = atoi(&argv[k][2]);
		break;

	    case 'n':
	    case 'N':
		Page_Switch(&argv[k][1]);
		break;

	    case 'o':
	    case 'O':
		if (argv[k][2])		/* have -O# */
		    outline = INCH_TO_QSU(inch(&argv[k][2]));
		else			/* have -O, so give default */
		    outline = INCH_TO_QSU(inch("0.4pt"));  /* from TeX */
		break;

	    case 'p':
	    case 'P':
		point_size = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 'r':
	    case 'R':
		right_margin = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 's':
	    case 'S':
		if (argv[k][2]) /* have count field */
		    manual_feed = atoi(&argv[k][2]);
		else		/* omitted count ==> large number */
		    manual_feed = 32767;
		break;

	    case 't':
	    case 'T':
		top_margin = INCH_TO_QSU(inch(&argv[k][2]));
		break;

	    case 'u':
	    case 'U':
		reverse = FALSE;
		break;

	    default:
		NEWLINE(stderr);
		(void)fprintf(stderr,"?Unrecognized option [%s]",p);
		NEWLINE(stderr);
		bad_option = TRUE;
	    }
	}
    }

    if (bad_option)
    {
	(void)fprintf(stderr,
	    "Usage: %s -A -B# -Cn -Ffontname -H -I# -L# -Mn -N[tb][lcr]n \
-O[#] -P# -R# -S[n] -T# -U filelist >outfile",argv[0]);
	NEWLINE(stderr);

	(void)fprintf(stderr,"Default options: -B1.0in -C1 -FCourier -L1.0in \
-M1 -P10bp -R1.0in -S0 -T1.0in");
	NEWLINE(stderr);

	EXIT(1);
    }


    if (reverse)
    {
	if ((tf = fopen(TEMPFILE,FOPEN_W)) == (FILE *)NULL)
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,"?Cannot open temporary file [%s]",TEMPFILE);
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}
    }
    else
	tf = stdout;

    /*******************************************************************
    Make the PostScript File Structure header according to Version  1.0,
    First Edition  (January 1985)  of  the PostScript  File  Structuring
    Conventions manual.  We add  one additional field  at the end  after
    the %%Pages: n field:

    %%PageTable: m1 n1 p1 ... mk nk pk

    where the triples (mj  nj pj) contain the  page number pair (mj  nj)
    like the field

    %%Page: mj nj

    and pj is the  byte position in  the file of  the beginning of  that
    "%%Page" command.  A spooler can  then easily position to  arbitrary
    pages in the file for reprinting.
    *******************************************************************/

    OUTSTR("%!PS-Adobe-1.0");		/* magical file header */
    NEWLINE(tf);

    OUTSTR("%%DocumentFonts: ");
    OUTSTR(font_name);			/* only use one font */
    NEWLINE(tf);

    OUTSTR("%%Dimensions: 0 0 612 792");	/* 8.5 x 11 inch page size */
    NEWLINE(tf);

    OUTSTR("%%Title:");			/* this contains our command line */
    for (k = 0; k < argc; ++k)
    {
	OUTCHAR(' ');
	OUTSTR(argv[k]);
    }
    NEWLINE(tf);		       /* end of %%Title line */

    timeval = time((long *)NULL);
    strcpy(timestr,ctime(&timeval));
    k = strlen(timestr) - 1;	/* ctime has its own \n */
    timestr[k] = NUL;		/* so kill it */
    OUTSTR("%%CreationDate: ");
    OUTSTR(timestr);
    NEWLINE(tf);

    /* Under TOPS-20, JSYS GFUST% cannot be used to obtain file owner string
    of a terminal file, which stdout always is, so just use job login name */

    OUTSTR("%%Creator: ");
    if (cuserid((char*)NULL) != (char*)NULL)
        OUTSTR(cuserid((char*)NULL));
    NEWLINE(tf);

    OUTSTR("%%Pages: (atend)");
    NEWLINE(tf);
    OUTSTR("%%EndComments");
    NEWLINE(tf);
    OUTSTR("save");			/* has matching "restore" at end */
    NEWLINE(tf);
    OUTSTR("%%EndProlog");
    NEWLINE(tf);

		     /* validate input settings */
    copies = MAX(1,copies);
    numcol = MAX(1,numcol);
    point_size = MAX(INCH_TO_QSU(inch("3bp")),point_size);
    if (point_size > INCH_TO_QSU(0.25))
    {
	NEWLINE(stderr);
	(void)fprintf(stderr,
	    "%warning -- font point size > 18pt is unusually large!");
	NEWLINE(stderr);
    }
    outline = MAX(0L,outline);

    if (landscape_mode)		/* switch sizes for landscape mode */
    {
	page_height = WIDTH;
	page_width = HEIGHT;
    }
    if (((left_margin + right_margin) >= page_width) ||
	((top_margin + bottom_margin) >= page_height) ||
	((init_top_margin + bottom_margin) >= page_height))
    {
	NEWLINE(stderr);
	(void)fprintf(stderr,"?Margin values too large for page");
	NEWLINE(stderr);
	EXIT(1);
    }

    col_width = (page_width - left_margin - right_margin)/numcol;
    col_margin = ((numcol == 1) && (outline == 0)) ?
	0L : (col_width/COL_TWIDDLE);

    /* TeX's baselineskip is 12pt for 10pt type; we preserve the same ratio */
    baseline_skip = (point_size * 12L) / 10L;

    m = 0;				/* count of file names */
    for (k = 1; k < argc; ++k)
    {
	if (*argv[k] != '-')
	{
	    m++;			/* count file name */
	    if ((fp = fopen(argv[k],FOPEN_R)) != (FILE *)NULL)
	    {
		Do_File(fp);
		(void)fclose(fp);
	    }
	    else
	    {
		(void)fprintf(stderr,"?Open failure on [%s] -- file skipped",
		    argv[k]);
		NEWLINE(stderr);
	    }
	}
    }
    if (m == 0)
	Do_File(stdin);

    if (reverse)
    {
	/* Now close the temporary file and reopen for input, then transfer */
	/* complete pages in reverse order to stdout using large blocks */

	if (fclose(tf))
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,"?Close failure on temporary file [%s]",
		TEMPFILE);
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}

	if ((tf = fopen(TEMPFILE,FOPEN_R)) == (FILE *)NULL)
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,"?Cannot reopen temporary file [%s]",TEMPFILE);
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}

	NEWLINE(stderr);
	(void)fprintf(stderr,"[Reversing pages");
	Copy_Block(0L,page_table[0]-1);	/* first page is file header stuff */
	last = page_table[MIN(MAXPAGE,page_number-init_page_number+1)];
	for (k = MIN(MAXPAGE,page_number-init_page_number+1)-1; k >= 0; --k)
	{				/* copy remainder in reverse order */
	    (void)putc('.',stderr);
	    next = (long)ftell(stdout);
	    Copy_Block(page_table[k],last-1);
	    last = page_table[k];
	    page_table[k] = next;
	}
	(void)putc(']',stderr);
	if (fclose(tf))
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,"?Close failure on temporary file [%s]",
		TEMPFILE);
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}

	unlink(TEMPFILE);		/* delete the temporary file */
    }
    /* add file trailer stuff */

    tf = stdout;			/* so OUTSTR() macro works */

    OUTSTR("%%Trailer");
    NEWLINE(stdout);
    OUTSTR("restore");			/* matches "save" in prolog */
    NEWLINE(stdout);
    (void)printf("%%%%Pages: %d",page_number-init_page_number);
    NEWLINE(stdout);
    OUTSTR("%%PageTable: ");
    for (k = 0; k < MIN(MAXPAGE,page_number-init_page_number+1); ++k)
	(void)printf(" %d %d %ld",init_page_number+k,init_page_number+k,
	    page_table[k]);
    NEWLINE(stdout);
    if (putchar('\004') == EOF)		/* CTL-D for EOF signal */
    {
	NEWLINE(stderr);
	(void)fprintf(stderr,"?Output error -- disk storage probably full");
	NEWLINE(stderr);
	(void)perror("?perror() says");
	EXIT(1);
    }
    (void)fprintf(stderr," [ok]");
    NEWLINE(stderr);
    EXIT(0);
    return (0);				/* keep lint and compilers happy */
}

void
Beg_Column()
{
    (void)fprintf(tf,"%ld %ld moveto",x,y);
    NEWLINE(tf);
}

void
Beg_Line()
{
    if (scribe)
    {
	OUTCHAR('S');
	OUTCHAR(' ');
    }
}

void
Beg_Page()
{
    int k;

    column = 1;
    (void)fflush(tf);
    k = page_number-init_page_number;
    if (k < MAXPAGE)
	page_table[k] = (long)ftell(tf);
    (void)fprintf(tf,"%%%%Page: %d %d",k+1,page_number);
    NEWLINE(tf);
    OUTSTR("save");	/* will be matched by "restore" from End_Page() */
    NEWLINE(tf);
    if (scribe)
    {
	(void)fprintf(tf,"/B {/%s findfont %ld scalefont setfont} def",
	    font_bold,point_size);
	NEWLINE(tf);

	(void)fprintf(tf,"/E {grestore 0 %ld rmoveto} def",
	    -baseline_skip);		/* End of line */
	NEWLINE(tf);

	(void)fprintf(tf,"/I {/%s findfont %ld scalefont setfont} def",
	    font_italic,point_size);
	NEWLINE(tf);

	(void)fprintf(tf,"/N {/%s findfont %ld scalefont setfont} def",
	    font_name,point_size);
	NEWLINE(tf);

	OUTSTR("/O {gsave show grestore} def");	/* OverPrint macro */
	NEWLINE(tf);

	OUTSTR("/S {gsave} def");	/* Start of line */
	NEWLINE(tf);

	OUTSTR("/W {show} def");	/* show text */
	NEWLINE(tf);
    }
    else
    {
	OUTSTR("/O {gsave show grestore} def");	/* OverPrint macro */
	NEWLINE(tf);

	(void)fprintf(tf,"/P {gsave show grestore 0 %ld rmoveto} def",
	    -baseline_skip);  /* Print macro */
	NEWLINE(tf);

	(void)fprintf(tf,"/N {/%s findfont %ld scalefont setfont} def",
	    font_name,point_size);
	NEWLINE(tf);
    }
    (void)fprintf(tf,"72 %ld div 72 %ld div scale",QSU,QSU);
    NEWLINE(tf);
    OUTCHAR('N');			/* select normal font */

#if    FIX31OCT85
    OUTSTR("()pop");			/* no-op for XON/XOFF bug workaround */
#endif

    NEWLINE(tf);

    if (landscape_mode)
    {
	(void)fprintf(tf,"%ld %ld translate",0L,HEIGHT);
	NEWLINE(tf);
	OUTSTR("-90 rotate");
	NEWLINE(tf);
    }
    (void)fprintf(stderr,
	" [%d",page_number);	/* begin [p.l.l...] status report */
}

void
Beg_Text(c)
char c;
{
    if (c)
    {
	OUTCHAR(' ');
	OUTCHAR(c);
	OUTCHAR(' ');
    }
    OUTCHAR('(');
    level++;
}

void
Copy_Block(b1,b2)	/* copy bytes b1..b2 from tf to stdout */
long b1,b2;				/* start,end byte positions */
{
    static char block[MAXBLOCK];	/* static to save stack space */
    long k;				/* loop index */
    long length;			/* number of bytes to copy */

#if    FASTIO
    (void)fflush(stdout);		/* empty any buffered output */
#endif
    for (k = b1; k <= b2; k += MAXBLOCK)
    {
	length = MIN(MAXBLOCK,b2-k+1);

	(void)fseek(tf,k,0);		/* position to desired block */

#if    FASTIO
	if (_read(jfnof(fileno(tf)),block,(int)length) != (int)length)
#else
	if (fread(block,1,(int)length,tf) != (int)length)
#endif
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,
	      "?Input block length error on temporary file [%s]",TEMPFILE);
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}
#if    FASTIO
	if (_write(jfnof(fileno(stdout)),block,(int)length) != (int)length)
#else
	if (fwrite(block,1,(int)length,stdout) != (int)length)
#endif
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,
	      "?Output block length error on stdout");
	    NEWLINE(stderr);
	    (void)perror("?perror() says");
	    EXIT(1);
	}
    }
    (void)fflush(stdout);
}

void
Copy_Plist(fontc,fp)		/* copy parenthesized list to tf */
char fontc;
FILE *fp;
{
    static char p_open[] = {'(','[','<','{',NUL};
    static char p_close[] = {')',']','>','}',NUL};
    register int match;
    register int c;

    c = inchar(fp);
    match = 0;
    while (p_open[match] && (p_open[match] != (char)c))
	match++;

    if (!p_open[match])
    {
	NEWLINE(stderr);
	(void)fprintf(stderr,"?@b or @i not followed by open parenthesis!");
	NEWLINE(stderr);
	EXIT(1);
    }

    Beg_Text(fontc);
    while (((c = inchar(fp)) != EOF) && ((char)c != p_close[match]))
    {
	if (c < ' ')
	{
	    NEWLINE(stderr);
	    (void)fprintf(stderr,
		"?@b[...] or @i[...] contains control character!");
	    NEWLINE(stderr);
	    EXIT(1);
	}
	in_col++;
	switch(c)
	{
	case '(':
	    OUTCHAR('\\');
	    OUTCHAR(c);
	    break;

	case ')':
	    OUTCHAR('\\');
	    OUTCHAR(c);
	    break;

	case '\\':
	    OUTCHAR('\\');
	    OUTCHAR(c);
	    break;

	default:
	    OUTCHAR(c);
	    break;
	}
    }
    End_Text('N');
    OUTCHAR(' ');
}

void
Do_File(fp)
FILE *fp;
{
    register int c;			/* character */

/**********************************************************************/
/*		       PostScript Output Section		      */
/**********************************************************************/

    x = LEFT_X;
    if (init_top_margin > 0)
	y = TOP_Y(init_top_margin);
    else
	y = TOP_Y(top_margin);

    Beg_Page();
    Beg_Column();
    level = 0;
    in_col = 0;
    line_number = 1;

    if ((c = inchar(fp)) != '\f')	 /* discard any initial FF */
	(void)ungetc(c,fp);

    while ((c = inchar(fp)) != EOF)
    {
	switch (c)
	{
	case NUL:
	    break;	/* discard NUL's from word-padded files */

	case '\b':
	    if (in_col > 0)
	    {
		in_col--;
		OUTCHAR('\\');
		OUTCHAR('b');
	    }
	    break;

	case '\f':
	    in_col = 0;
	    End_Text(NUL);
	    NEWLINE(tf);
	    End_Column();
	    if (column > numcol)
		Beg_Page();
	    Beg_Column();
	    line_number = 1;
	    break;

	case '\n':
	    line_number++;
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    End_Text(NUL);
	    End_Line();
	    in_col = 0;
	    y -= baseline_skip;
	    if (y < BOT_Y)
	    {
		End_Column();
		if (column > numcol)
		    Beg_Page();
		Beg_Column();
		line_number = 1;
	    }
	    break;


	case '\r':	/* check for special case of overprinting */
	    if ((c = inchar(fp)) != '\n') /* peek ahead one character */
	    {			/* and if not LF, have overstrike */
		while (level > 0)
		{
		    OUTCHAR(')');
		    level--;
		}
		in_col = 0;
		OUTCHAR('O');
		NEWLINE(tf);	/* this lines up under previous line */
	    }
	    (void)ungetc(c,fp);  /* put back the peeked-at character */
	    break;

	case '\t':
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    do
	    {
		in_col++;
		OUTCHAR(' ');
	    }
	    while (in_col & 07); /* blank fill to multiple of 8 columns */
	    break;

	case '(':
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    in_col++;
	    OUTCHAR('\\');
	    OUTCHAR('(');
	    break;

	case ')':
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    in_col++;
	    OUTCHAR('\\');
	    OUTCHAR(')');
	    break;

	case '\\':
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    in_col++;
	    OUTCHAR('\\');
	    OUTCHAR('\\');
	    break;

	case '@':
	    if (scribe)
	    {
		c = inchar(fp);
		if ((c == 'b') || (c == 'B'))
		{
		    if (in_col == 0)
			Beg_Line();
		    else
    			End_Text(NUL);
		    Copy_Plist('B',fp);
		    Beg_Text(NUL);
		    break;
		}
		else if ((c == 'i') || (c == 'I'))
		{
		    if (in_col == 0)
			Beg_Line();
		    else
    			End_Text(NUL);
		    Copy_Plist('I',fp);
		    Beg_Text(NUL);
		    break;
		}
		(void)ungetc(c,fp);	/* put back the peeked-at character */
		c = (int)'@';		/* and restore original character */
	    }
	    /* ELSE fall through to default! */

	default:
	    if (in_col == 0)
	    {
		Beg_Line();
		Beg_Text(NUL);
	    }
	    in_col++;
	    if ((c < 040) || (0176 < c))
		OUTOCTAL(c);
	    else
		OUTCHAR(c);
	    break;
	}
    }
    if (in_col > 0)
	End_Line();
    End_Text(NUL);
    column = numcol;			/* to force End_Page() action */
    End_Column();

    /* Normally, Beg_Page() sets the page_table[] entry.  Since this  is
    the last output  for this file,  we must set  it here manually.   If
    more files are  printed, this  entry will be  overwritten (with  the
    same value) at the next call to Beg_Page(). */
    page_table[page_number-init_page_number] = (long)ftell(tf);
}

void
End_Column()
{
    if (outline > 0)
    {
	NEWLINE(tf);
	OUTSTR("gsave");
	NEWLINE(tf);
	(void)fprintf(tf,"%ld setlinewidth",outline);
	NEWLINE(tf);
	(void)fprintf(tf,"newpath %ld %ld moveto",x - col_margin,bottom_margin);
	(void)fprintf(tf,
	    " %ld %ld rlineto %ld %ld rlineto %ld %ld rlineto closepath stroke",
	    col_width,0,
	    0,page_height-bottom_margin-top_margin,
	    -col_width,0);
	NEWLINE(tf);
	OUTSTR("grestore");
	NEWLINE(tf);
    }
    (void)fprintf(stderr,
	".%d",line_number);	/* continue [p.l.l...] status report */
    if (++column > numcol)	/* columns filled, so start new page */
    {
	(void)putc(']',stderr);	/* close [p.l.l...] report */
	End_Page();
    }
    else			/* just start new column */
    {
	x += col_width;
	y = TOP_Y(top_margin);
    }
}

void
End_Page()
{
    register int k;

    if (do_page_number)
    {
	if ((init_top_margin > 0) && (page_number == init_page_number)
	    && (page_flag | PAGE_T))
	    ;			/* omit leading top pagenumber */
	else
	    switch (page_flag)
	    {
	    case (PAGE_T | PAGE_L):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",left_margin,
		    page_height-top_margin/2-baseline_skip/2);
		(void)fprintf(tf," (%d) show",page_number);
		break;

	    case (PAGE_T | PAGE_C):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",page_width/2,
		    page_height-top_margin/2-baseline_skip/2);
		(void)fprintf(tf,
		    " (- %d -) stringwidth pop 2 div neg 0 rmoveto",
		    page_number);
		(void)fprintf(tf," (- %d -) show",page_number);
		NEWLINE(tf);
		break;

	    case (PAGE_T | PAGE_R):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",page_width-left_margin,
		    page_height-top_margin/2-baseline_skip/2);
		(void)fprintf(tf," (%d) stringwidth pop neg 0 rmoveto",
		    page_number);
		(void)fprintf(tf," (%d) show",page_number);
		NEWLINE(tf);
		break;

	    case (PAGE_B | PAGE_L):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",left_margin,
		    bottom_margin/2-baseline_skip/2);
		(void)fprintf(tf," (%d) show",page_number);
		break;

	    case (PAGE_B | PAGE_C):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",page_width/2,
		    bottom_margin/2-baseline_skip/2);
		(void)fprintf(tf,
		    " (- %d -) stringwidth pop 2 div neg 0 rmoveto",
		    page_number);
		(void)fprintf(tf," (- %d -) show",page_number);
		NEWLINE(tf);
		break;

	    case (PAGE_B | PAGE_R):
		NEWLINE(tf);
		(void)fprintf(tf,"%ld %ld moveto",page_width-left_margin,
		    bottom_margin/2-baseline_skip/2);
		(void)fprintf(tf," (%d) stringwidth pop neg 0 rmoveto",
		    page_number);
		(void)fprintf(tf," (%d) show",page_number);
		NEWLINE(tf);
		break;

	    default:			/* else no numbering */
		break;
	    }
    }
    page_number++;

    if (manual_feed > 0)
    {
	OUTSTR("statusdict begin");
	NEWLINE(tf);
	OUTSTR("statusdict /manualfeed true put");
	NEWLINE(tf);
    }

    if ((fprintf(tf,"/#copies %d def showpage",copies) == EOF) ||
	DISKFULL(tf))
    {
	NEWLINE(stderr);
	(void)fprintf(stderr,"?Output error -- disk storage probably full");
	NEWLINE(stderr);
	(void)perror("?perror() says");
	EXIT(1);
    }
    NEWLINE(tf);

    if (manual_feed > 0)
    {
	OUTSTR("end");
	NEWLINE(tf);
    }
    manual_feed--;

    OUTSTR("restore");
    NEWLINE(tf);

    for (k = 1; k <= copies; ++k)
	OUTCHAR('\f'); /* output FF's for print spooler page accounting */

    x = LEFT_X;
    y = TOP_Y(top_margin);
}

void
End_Line()
{
    if (scribe)
    {
	OUTCHAR(' ');
	OUTCHAR('E');
    }
    NEWLINE(tf);
}


void
End_Text(c)
char c;
{
    register int k;

    k = level;
    while (level > 0)
    {
	OUTCHAR(')');
	level--;
    }
    if (k > 0)
    {
	if (scribe)
	    OUTCHAR('W');
	else
	    OUTCHAR('P');
    }
    if (c)
    {
	OUTCHAR(' ');
	OUTCHAR(c);
    }
}

#include "inch.h"

int
inchar(fp)
FILE *fp;
{
    static int lastc = NUL;	/* memory of previous input character */
    register int c;

    /* output is incorrect if the file does not end with a LF; this
    routine supplies one at EOF if it is lacking */

    c = getc(fp);
    if ((c == EOF) && (lastc != '\n'))
	c = '\n';
    lastc = c;
    return (c);
}

void
Page_Switch(parg)			/* parg -> "Nxxxnnn" */
register char *parg;
{
    register char *porg;

    porg = parg;
    do_page_number = TRUE;
    init_page_number = 1;
    page_flag = (PAGE_T | PAGE_C);    /* default location */
    while (*++parg)
    {
	switch(*parg)
	{
	case 't':
	case 'T':
	    page_flag &= ~(PAGE_B | PAGE_T);
	    page_flag |= PAGE_T;
	    break;

	case 'b':
	case 'B':
	    page_flag &= ~(PAGE_B | PAGE_T);
	    page_flag |= PAGE_B;
	    break;

	case 'l':
	case 'L':
	    page_flag &= ~(PAGE_L | PAGE_C | PAGE_R);
	    page_flag |= PAGE_L;
	    break;

	case 'c':
	case 'C':
	    page_flag &= ~(PAGE_L | PAGE_C | PAGE_R);
	    page_flag |= PAGE_C;
	    break;

	case 'r':
	case 'R':
	    page_flag &= ~(PAGE_L | PAGE_C | PAGE_R);
	    page_flag |= PAGE_R;
	    break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    init_page_number = atoi(parg);
	    while (isdigit(*parg))
		++parg;
	    if (*parg == NUL)
		break;
			    /* else fall through to error exit */
	default:
	    NEWLINE(stderr);
	    (void)fprintf(stderr,
		"?Unrecognized page number field [%s]",porg);
	    NEWLINE(stderr);
	    EXIT(1);
	}
	if (*parg == NUL)
	    break;
    }
}

#include "strcm2.h"

