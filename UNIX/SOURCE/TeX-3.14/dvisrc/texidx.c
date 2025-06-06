/* <BEEBE.GNU>TEXID3.C.18, 28-Nov-86 16:28:02, Edit by BEEBE */
/* Fix incorrect isalnum() comparison in compare_prepared */
/* <BEEBE.GNU>TEXID3.C.12,  5-Nov-86 12:12:51, Edit by BEEBE */
/* (1) Modify indexify() to support pagenumber range output */
/* (2) Complete forward function declarations */
/* (3) Change type of xmalloc() and xrealloc() from int to char* */
/*     for portability (eventually, they will be void* with ANSI C) */
/* <BEEBE.GNU>TEXIDX.C.11, 26-Sep-86 18:38:51, Edit by BEEBE */
/* (1) Change num_keyfields-1 in comparisons to num_keyfields, so this */
/* works for secondary entries too. */
/* (2) Add initializations of 0 keyfield[] fields. */
/* (3) Replace 5 instances of "if (keyfields->xxx)" with */
/* "if (keyfields[0].xxx)" */
/* <BEEBE.GNU>TEXIDX.C.3, 20-Aug-86 11:58:19, Edit by BEEBE */
/* All local changes under control of "tops20" preprocessor symbol.*/
/* Added #define's for 6-char unique externals. */
/* Changed union text member to keytext in order to have unique */
/* structure names (this required a careful global edit). */
/* Changed struct keyfield to struct keyfld for uniqueness. */
/* Changed open() call to old PCC form */

/* Prepare Tex index dribble output into an actual index.
   Copyright (C) Richard M. Stallman 1984

   Permission is granted to anyone to make or distribute
   verbatim copies of this program
   provided that the copyright notice and this permission notice are preserved;
   and provided that the recipient is not asked to waive or limit his right to
   redistribute copies as permitted by this permission notice;
   and provided that anyone possessing a machine-executable copy
   is granted access to copy the source code, in machine-readable form,
   in some reasonable manner.

   Permission is granted to distribute derived works or enhanced versions of
   this program under the above conditions with the additional condition
   that the entire derivative or enhanced work
   must be covered by a permission notice identical to this one.

   Anything distributed as part of a package containing portions derived
   from this program, which cannot in current practice perform its function
   usefully in the absence of what was derived directly from this program,
   is to be considered as forming, together with the latter,
   a single work derived from this program,
   which must be entirely covered by a permission notice identical to this one
   in order for distribution of the package to be permitted.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include <stdio.h>
#include <ctype.h>

#define READ read
#define EXIT exit

#ifdef KCC_20
#include <file.h>
#define tops20
#else
#ifdef OS_VAXVMS
#include <file.h>
#undef READ
#define READ vms_read
#undef EXIT
#define EXIT vms_exit
#else
#ifdef OS_ATARI
#else
#ifdef IBM_PC_MICROSOFT
#include <fcntl.h>
#include <io.h>
#else
#include <sys/types.h>
#include <sys/file.h>
#endif
#endif
#endif
#endif

#ifdef tops20

/* variables */

#define	lastsecondarylength	lstsln
#define	lengthptr	lengp
#define	primarylength	prilen
#define compare_field	cmpfld
#define compare_general	cmpgen
#define compare_prepared cmppre
#define keyfields	keyfls
#define lastinitial	lstini
#define lastinitial1	lstin1
#define lastinitiallength	lstiln
#define lastprimary	lstpri
#define lastprimarylength	lstpln
#define lastsecondary	lstsec
#define length1		leng1
#define length2		leng2
#define secondarylength	seclen
#define start_tempcount	strtmc
#define	tempcount	tmpcnt

/* functions */

#define compare_full	cmpful
#define find_braced_end	fndben
#define find_braced_pos	fndbpo
#define find_field	fndfld
#define find_pos	fndpos
#define flush_tempfiles	flstmp
#define maketempname	makete
#define merge_direct	mrgdir
#define merge_files	mrgfil
#define parsefile	prsfil
#define pfatal_with_name	pfawna
#define readline	rdline
#define sort_in_core	srtcor
#define sort_offline	srtoff
#define writelines	writln

int sys_nerr = 0;
char *sys_errlist[] = {"?"};

void
bzero (s,n)
register char *s;
register int n;
{
    while (n--)
	*s++ = '\0';
}

#endif

#ifdef VMS
void
bzero (s,n)
register char *s;
register int n;
{
    while (n--)
	*s++ = '\0';
}
#endif

#ifdef IBM_PC_MICROSOFT
void
bzero (s,n)
register char *s;
register int n;
{
    while (n--)
	*s++ = '\0';
}
#endif

#ifdef OS_ATARI
void
bzero (s,n)
register char *s;
register int n;
{
    while (n--)
	*s++ = '\0';
}
#endif

#ifndef L_XTND
#define L_XTND 2
#endif

/* When sorting in core, this structure describes one line
 and the position and length of its first keyfield.  */

struct lineinfo
  {
    char *text;		/* The actual text of the line */
    union
      {			/* The start of the key (for textual comparison) */
	char *keytext;
	long number;	/* or the numeric value (for numeric comparison) */
      } key;
    long keylen;	/* Length of key field */
  };

/* This structure describes a field to use as a sort key */

struct keyfld
  {
    int startwords;		/* # words to skip  */
    int startchars;		/*  and # additional chars to skip, to start */
				/* of field */
    int endwords;		/* similar, from beg (or end) of line, to */
				/* find end of field */
    int endchars;
    char ignore_blanks;		/* Ignore spaces and tabs within the field */
    char fold_case;		/* Convert upper case to lower before */
				/* comparing */
    char reverse;		/* Compare in reverse order */
    char numeric;		/* Parse text as an integer and compare the */
				/* integers */
    char positional;		/* Sort according to position within the file */
    char braced;		/* Count balanced-braced groupings as fields */
  };

/* Vector of keyfields to use */

struct keyfld keyfields[3];

/* Number of keyfields stored in that vector.  */

int num_keyfields = 3;

/* Vector of input file names, terminated with a zero (null pointer) */

char **infiles;

/* Vector of corresponding output file names, or zero meaning default it */

char **outfiles;

/* Length of `infiles' */

int num_infiles;

/* Pointer to the array of pointers to lines being sorted */

char **linearray;

/* The allocated length of `linearray'. */

long lines;

/* Directory to use for temporary files */

char *tempdir;

/* Start of filename to use for temporary files.  It starts with a slash.  */

char *tempbase;

/* Number of last temporary file.  */

int tempcount;

/* Number of last temporary file already deleted.
 Temporary files are deleted by `flush_tempfiles' in order of creation.  */

int last_deleted_tempcount;

/* During in-core sort, this points to the base of the data block
 which contains all the lines of data.  */

char *text_base;

/* Additional command switches */

int keep_tempfiles;	/* Nonzero means do not delete tempfiles -- for */
			/* debugging */

/* Forward declarations of functions in this file (alphabetical by function */
/* name) */

int	classify_arg();
int	compare_field();
int     compare_full();
int	compare_general();
int	compare_prepared();
char*   concat();
void    decode_command();
void	error();
void	fatal();
char*   find_braced_end();
char*   find_braced_pos();
char*   find_field();
char*   find_pos();
long    find_value();
void	finish_index();
void    flush_tempfiles();
void	indexify();
void	initbuffer();
void	init_index();
int	main();
char*   maketempname();
int     merge_direct();
int     merge_files();
extern char* mktemp();
char**  parsefile();
void	pfatal_with_name();
void    put_range();
long    readline();
void    sort_in_core();
void    sort_offline();
char*	strchr();
void    writelines();
char*	xmalloc();
char*	xrealloc();

#define MAX_IN_CORE_SORT 500000

#ifdef KCC_20
#undef MAX_IN_CORE_SORT
#define MAX_IN_CORE_SORT  4*512*128
#endif

#ifdef IBM_PC_MICROSOFT
#undef MAX_IN_CORE_SORT
#define MAX_IN_CORE_SORT  64000
#endif

#ifdef PCC_20
#undef MAX_IN_CORE_SORT
#define MAX_IN_CORE_SORT  512*128
#endif

#ifdef OS_ATARI
#undef MAX_IN_CORE_SORT
#define MAX_IN_CORE_SORT  512*128
long _stksize = 20000L;
#endif

int
main (argc, argv)
     int argc;
     char **argv;
{
  int i;

  tempcount = 0;
  last_deleted_tempcount = 0;

  /* Describe the kind of sorting to do. */
  /* The first keyfield uses the first braced field and folds case */
  keyfields[0].braced = 1;
  keyfields[0].fold_case = 1;
  keyfields[0].endwords = -1;
  keyfields[0].endchars = -1;

  keyfields[0].ignore_blanks = 0;
  keyfields[0].numeric = 0;
  keyfields[0].positional = 0;
  keyfields[0].reverse = 0;
  keyfields[0].startchars = 0;
  keyfields[0].startwords = 0;

  /* The second keyfield uses the second braced field, numerically */
  keyfields[1].braced = 1;
  keyfields[1].numeric = 1;
  keyfields[1].startwords = 1;
  keyfields[1].endwords = -1;
  keyfields[1].endchars = -1;

  keyfields[1].fold_case = 0;
  keyfields[1].ignore_blanks = 0;
  keyfields[1].positional = 0;
  keyfields[1].reverse = 0;
  keyfields[1].startchars = 0;

  /* The third keyfield (which is ignored while discarding duplicates)
     compares the whole line */
  keyfields[2].endwords = -1;
  keyfields[2].endchars = -1;

  keyfields[2].braced = 0;
  keyfields[2].fold_case = 0;
  keyfields[2].ignore_blanks = 0;
  keyfields[2].numeric = 0;
  keyfields[2].positional = 0;
  keyfields[2].reverse = 0;
  keyfields[2].startchars = 0;
  keyfields[2].startwords = 0;

  decode_command (argc, argv);

#ifdef OS_ATARI
  tempbase = mktemp (concat ("\\TXIXXX\.", "", ""));
#else
  tempbase = mktemp (concat ("/txiXXXXXX", "", ""));
#endif

  /* Process input files completely, one by one.  */

  for (i = 0; i < num_infiles; i++)
    {
      int desc;
      long ptr;
      char *outfile;

#ifdef VMS
      FILE* fp;
#endif
      desc = open (infiles[i], 0, 0);
      if (desc < 0) pfatal_with_name (infiles[i]);
#ifdef VMS
      fp = fdopen(desc,"r");
#endif
      lseek (desc, 0L, L_XTND);
#ifdef VMS
      ptr = ftell(fp);
#else
#ifdef OS_ATARI
      ptr = ftell(desc);
#else
      ptr = tell (desc);
#endif
#endif
      close (desc);

      outfile = outfiles[i];
      if (!outfile)
	{
	  outfile = concat (infiles[i], "s", "");
	}

      if (ptr < MAX_IN_CORE_SORT)
        /* Sort a small amount of data */
        sort_in_core (infiles[i], ptr, outfile);
      else
        sort_offline (infiles[i], ptr, outfile);
    }

  flush_tempfiles (tempcount);
  EXIT (0);
}

/* This page decodes the command line arguments to set the parameter variables
 and set up the vector of keyfields and the vector of input files */

void
decode_command (argc, argv)
     int argc;
     char **argv;
{
  int i;
  char **ip;
  char **op;

  /* Store default values into parameter variables */

#ifdef OS_ATARI
  tempdir = "\\TMP";
#else
  tempdir = "/tmp";
#endif

  keep_tempfiles = 0;

  /* Allocate argc input files, which must be enough.  */

  infiles = (char **) xmalloc (argc * sizeof (char *));
  outfiles = (char **) xmalloc (argc * sizeof (char *));
  ip = infiles;
  op = outfiles;

  /* First find all switches that control the default kind-of-sort */

  for (i = 1; i < argc; i++)
    {
      int tem = classify_arg (argv[i]);
      char c;
      char *p;

      if (tem <= 0)
	{
	  *ip++ = argv[i];
	  *op++ = 0;
	  continue;
	}
      if (tem > 1)
	{
	  if (i + 1 == argc)
	    fatal ("switch %s given with no argument following it", argv[i]);
	  else if (!strcmp (argv[i], "-T"))
	    tempdir = argv[i + 1];
	  else if (!strcmp (argv[i], "-o"))
	    *(op - 1) = argv[i + 1];
	  i += tem - 1;
	  continue;
	}

      p = &argv[i][1];
      while (c = *p++)
	switch (c)
	  {
	  case 'k':
	    keep_tempfiles = 1;
	    break;

	  default:
	    fatal ("invalid command switch %c", c);
	  }
    }

  /* Record number of keyfields, terminate list of filenames */

  num_infiles = ip - infiles;
  *ip = 0;
}

/* Return 0 for an argument that is not a switch;
 for a switch, return 1 plus the number of following arguments that the switch
 swallows.
*/

int
classify_arg (arg)
     char *arg;
{
  if (!strcmp (arg, "-T") || !strcmp (arg, "-o"))
    return 2;
  if (arg[0] == '-')
    return 1;
  return 0;
}

/* Create a name for a temporary file */

char *
maketempname (count)
     int count;
{
  char tempsuffix[10];
  (void)sprintf (tempsuffix, "%d", count);
  return concat (tempdir, tempbase, tempsuffix);
}

/* Delete all temporary files up to the specified count */

void
flush_tempfiles (to_count)
     int to_count;
{
  if (keep_tempfiles) return;
  while (last_deleted_tempcount < to_count)
    unlink (maketempname (++last_deleted_tempcount));
}


/* Compare two lines, provided as pointers to pointers to text,
 according to the specified set of keyfields */

int
compare_full (line1, line2)
     char **line1, **line2;
{
  int i;

  /* Compare using the first keyfield;
     if that does not distinguish the lines, try the second keyfield; and so
     on. */

  for (i = 0; i < num_keyfields; i++)
    {
      long length1, length2;
      char *start1 = find_field (&keyfields[i], *line1, &length1);
      char *start2 = find_field (&keyfields[i], *line2, &length2);
      int tem = compare_field (&keyfields[i],
	 start1, length1, *line1 - text_base,
	 start2, length2, *line2 - text_base);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return - tem;
          return tem;
	}
    }

  return 0;    /* Lines match exactly */
}

/* Compare two lines described by structures
 in which the first keyfield is identified in advance.
 For positional sorting, assumes that the order of the lines in core
 reflects their nominal order.  */

int
compare_prepared (line1, line2)
     struct lineinfo *line1, *line2;
{
  int i;
  int tem;
  char *text1, *text2;

  /* Compare using the first keyfield, which has been found for us already */
  if (keyfields[0].positional)
    {
      if (line1->text - text_base > line2->text - text_base)
	tem = 1;
      else
	tem = -1;
    }
  else if (keyfields[0].numeric)
    tem = line1->key.number - line2->key.number;
  else
    tem = compare_field (keyfields, line1->key.keytext, line1->keylen, 0,
	line2->key, line2->keylen, 0);
  if (tem)
    {
      if (keyfields[0].reverse)
	return - tem;
      return tem;
    }

  text1 = line1->text;
  text2 = line2->text;

  /* Compare using the second keyfield;
     if that does not distinguish the lines, try the third keyfield; and so
     on. */

  for (i = 1; i < num_keyfields; i++)
    {
      long length1, length2;
      char *start1 = find_field (&keyfields[i], text1, &length1);
      char *start2 = find_field (&keyfields[i], text2, &length2);
      int tem = compare_field (&keyfields[i],
	start1, length1, text1 - text_base,
	start2, length2, text2 - text_base);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return - tem;
          return tem;
	}
    }

  return 0;    /* Lines match exactly */
}

/* Like compare_full but more general.
 You can pass any strings, and you can say how many keyfields to use.
 `pos1' and `pos2' should indicate the nominal positional ordering of
 the two lines in the input.  */

int
compare_general (str1, str2, pos1, pos2, use_keyfields)
     char *str1, *str2;
     long pos1, pos2;
     int use_keyfields;
{
  int i;

  /* Compare using the first keyfield;
     if that does not distinguish the lines, try the second keyfield; and so
     on. */

  for (i = 0; i < use_keyfields; i++)
    {
      long length1, length2;
      char *start1 = find_field (&keyfields[i], str1, &length1);
      char *start2 = find_field (&keyfields[i], str2, &length2);
      int tem = compare_field (&keyfields[i], start1, length1, pos1, start2,
	length2, pos2);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return - tem;
          return tem;
	}
    }

  return 0;    /* Lines match exactly */
}

/* Find the start and length of a field in `str' according to `keyfield'.
 A pointer to the starting character is returned, and the length
 is stored into the int that `lengthptr' points to.  */

char *
find_field (keyfield, str, lengthptr)
     struct keyfld *keyfield;
     char *str;
     long *lengthptr;
{
  char *start;
  char *end;
  char *(*fun) ();

  if (keyfield->braced) fun = find_braced_pos;
  else fun = find_pos;

  start = ( *fun )(str, keyfield->startwords, keyfield->startchars,
	       keyfield->ignore_blanks);
  if (keyfield->endwords < 0)
    {
      if (keyfield->braced)
	end = find_braced_end (start);
      else
	{
	  end = start;
	  while (*end && *end != '\n') end++;
	}
    }
  else
    {
      end = ( *fun )(str, keyfield->endwords, keyfield->endchars, 0);
      if (end - str < start - str) end = start;
    }
  *lengthptr = end - start;
  return start;
}

/* Find a pointer to a specified place within `str',
 skipping (from the beginning) `words' words and then `chars' chars.
 If `ignore_blanks' is nonzero, we skip all blanks
 after finding the specified word.  */

char *
find_pos (str, words, chars, ignore_blanks)
     char *str;
     int words, chars;
     int ignore_blanks;
{
  int i;
  char *p = str;

  for (i = 0; i < words; i++)
    {
      char c;
      /* Find next bunch of nonblanks and skip them. */
      while ((c = *p) == ' ' || c == '\t') p++;
      while ((c = *p) && c != '\n' && !(c == ' ' || c == '\t')) p++;
      if (!*p || *p == '\n') return p;
    }

  while (*p == ' ' || *p == '\t') p++;

  for (i = 0; i < chars; i++)
    {
      if (!*p  || *p == '\n') break;
      p++;
    }
  return p;
}

/* Like find_pos but assumes that each field is surrounded by braces
 and that braces within fields are balanced. */

char *
find_braced_pos (str, words, chars, ignore_blanks)
     char *str;
     int words, chars;
     int ignore_blanks;
{
  int i;
  int bracelevel;
  char *p = str;
  char c;

  for (i = 0; i < words; i++)
    {
      bracelevel = 1;
      while ((c = *p++) != '{' && c != '\n' && c);
      if (c != '{')
	return p - 1;
      while (bracelevel)
	{
	  c = *p++;
	  if (c == '{') bracelevel++;
	  if (c == '}') bracelevel--;
	  if (c == '\\') c = *p++;	/* \ quotes braces and \ */
	  if (c == 0 || c == '\n') return p-1;
	}
    }

  while ((c = *p++) != '{' && c != '\n' && c);

  if (c != '{')
    return p-1;

  if (ignore_blanks)
    while ((c = *p) == ' ' || c == '\t') p++;

  for (i = 0; i < chars; i++)
    {
      if (!*p  || *p == '\n') break;
      p++;
    }
  return p;
}

/* Find the end of the balanced-brace field which starts at `str'.
  The position returned is just before the closing brace. */

char *
find_braced_end (str)
     char *str;
{
  int bracelevel;
  char *p = str;
  char c;

  bracelevel = 1;
  while (bracelevel)
    {
      c = *p++;
      if (c == '{') bracelevel++;
      if (c == '}') bracelevel--;
      if (c == '\\') c = *p++;
      if (c == 0 || c == '\n') return p-1;
    }
  return p - 1;
}

long
find_value (start, length)
     char *start;
     long length;
{
  while (length != 0L) {
    if (isdigit(*start))
      return atol(start);
    length--;
    start++;
  }
  return 0l;
}

/* Compare two fields (each specified as a start pointer and a character count)
 according to `keyfield'.  The sign of the value reports the relation between
 the fields */

int
compare_field (keyfield, start1, length1, pos1, start2, length2, pos2)
     struct keyfld *keyfield;
     char *start1;
     long length1;
     long pos1;
     char *start2;
     long length2;
     long pos2;
{
  if (keyfields[0].positional)
    {
      if (pos1 > pos2)
	return 1;
      else
	return -1;
    }
  if (keyfield->numeric)
    {
      long value = find_value (start1, length1) - find_value (start2, length2);
      if (value > 0) return 1;
      if (value < 0) return -1;
      return 0;
    }
  else
    {
      char *p1 = start1;
      char *p2 = start2;
      char *e1 = start1 + length1;
      char *e2 = start2 + length2;

      int fold_case = keyfield->fold_case;

      while (1)
	{
	  char c1, c2;

	  if (p1 == e1) c1 = 0;
	  else c1 = *p1++;
	  if (p2 == e2) c2 = 0;
	  else c2 = *p2++;

	  /* Ignore case of both if desired */

	  if (fold_case)
	    {
	      if (c1 >= 'A' && c1 <= 'Z') c1 = c1 + 040;
	      if (c2 >= 'A' && c2 <= 'Z') c2 = c2 + 040;
	    }

	  /* Actually compare */

	  /* Anything but a letter or digit is less than any letter or digit */
	  if ((isalnum (c1) == 0) != (isalnum (c2) == 0))
	    return (isalnum (c1)) ? 1 : -1;
	  if (c1 != c2) return c1 - c2;
	  if (!c1) break;
	}
      return 0;
    }
}

/* A `struct linebuffer' is a structure which holds a line of text.
 `readline' reads a line from a stream into a linebuffer
 and works regardless of the length of the line.  */

struct linebuffer
  {
    long size;
    char *buffer;
  };

/* Initialize a linebuffer for use */

void
initbuffer (linebuffer)
     struct linebuffer *linebuffer;
{
  linebuffer->size = 200;
  linebuffer->buffer = (char *) xmalloc (200);
}

/* Read a line of text from `stream' into `linebuffer'.
 Return the length of the line.  */

long
readline (linebuffer, stream)
     struct linebuffer *linebuffer;
     FILE *stream;
{
  char *buffer = linebuffer->buffer;
  char *p = linebuffer->buffer;
  char *end = p + linebuffer->size;

  while (1)
    {
      int c = getc (stream);
      if (p == end)
	{
	  buffer = (char *) xrealloc (buffer, linebuffer->size *= 2);
	  p += buffer - linebuffer->buffer;
	  end += buffer - linebuffer->buffer;
	  linebuffer->buffer = buffer;
	}
      if (c < 0 || c == '\n')
	{
	  *p = 0;
	  break;
	}
      *p++ = c;
    }

  return p - buffer;
}

/* Sort the input files together when they are too big to sort in core */

void
sort_offline (infile, nfiles, total, outfile)
     char *infile;
     long total;
     char *outfile;
{
  int ntemps = 2 * (total + MAX_IN_CORE_SORT - 1) / MAX_IN_CORE_SORT;  /* More
	than enough */
  char **tempfiles = (char **) xmalloc (ntemps * sizeof (char *));
  FILE *istream = fopen (infile, "r");
  int i;
  struct linebuffer lb;
  long linelength;

  initbuffer (&lb);

  /* Read in one line of input data.  */

  linelength = readline (&lb, istream);

  /* Split up the input into `ntemps' temporary files, or maybe fewer,
    and put the new files' names into `tempfiles' */

  for (i = 0; i < ntemps; i++)
    {
      char *outname = maketempname (++tempcount);
      FILE *ostream = fopen (outname, "w");
      long tempsize = 0;

      if (!ostream) pfatal_with_name (outname);
      tempfiles[i] = outname;

      /* Copy lines into this temp file as long as it does not make file "too
	big" or until there are no more lines.  */

      while (tempsize + linelength + 1 <= MAX_IN_CORE_SORT)
	{
	  tempsize += linelength + 1;
	  fputs (lb.buffer, ostream);
	  putc ('\n', ostream);

	  /* Read another line of input data.  */

	  linelength = readline (&lb, istream);
	  if (!linelength && feof (istream)) break;
	}
      fclose (ostream);
      if (feof (istream)) break;
    }

  free (lb.buffer);

  /* Record number of temp files we actually needed.  */

  ntemps = i;

  /* Sort each tempfile into another tempfile.
    Delete the first set of tempfiles and put the names of the second into
    `tempfiles' */

  for (i = 0; i < ntemps; i++)
    {
      char *newtemp = maketempname (++tempcount);
      sort_in_core (&tempfiles[i], 1, MAX_IN_CORE_SORT, newtemp);
      if (!keep_tempfiles)
        unlink (tempfiles[i]);
      tempfiles[i] = newtemp;
    }

  /* Merge the tempfiles together and indexify */

  merge_files (tempfiles, ntemps, outfile);
}

/* Sort `infile', whose size is `total',
 assuming that is small enough to be done in-core,
 then indexify it and send the output to `outfile' (or to stdout).  */

void
sort_in_core (infile, total, outfile)
     char *infile;
     long total;
     char *outfile;
{
  char **nextline;
  char *data = (char *) xmalloc (total + 1);
  char *file_data;
  long file_size;
  FILE *ostream = stdout;
  struct lineinfo *lineinfo;

  /* Read the contents of the file into the moby array `data' */

  int desc = open (infile, 0, 0);

  if (desc < 0)
    fatal ("failure reopening %s", infile);
  file_size = READ (desc, data, total);
  file_data = data;
  data[file_size] = 0;

  close (desc);

  /* Sort routines want to know this address */

  text_base = data;

  /* Create the array of pointers to lines, with a default size frequently
     enough.  */

  lines = total / 50;
  if (!lines) lines = 2;
  linearray = (char **) xmalloc (lines * sizeof (char *));

  /* `nextline' points to the next free slot in this array.
     `lines' is the allocated size.  */

  nextline = linearray;

  /* Parse the input file's data, and make entries for the lines.  */

  nextline = parsefile (infile, nextline, file_data, file_size);

  /* Sort the lines */

  /* If we have enough space, find the first keyfield of each line in advance.
    Make a `struct lineinfo' for each line, which records the keyfield
    as well as the line, and sort them.  */

  lineinfo = (struct lineinfo *) malloc ((nextline - linearray) * sizeof
	(struct lineinfo));

  if (lineinfo)
    {
      struct lineinfo *lp;
      char **p;

      for (lp = lineinfo, p = linearray; p != nextline; lp++, p++)
	{
	  lp->text = *p;
	  lp->key.keytext = find_field (keyfields, *p, &lp->keylen);
	  if (keyfields[0].numeric)
	    lp->key.number = find_value (lp->key.keytext, lp->keylen);
	}

      qsort (lineinfo, nextline - linearray, sizeof (struct lineinfo),
	compare_prepared);

      for (lp = lineinfo, p = linearray; p != nextline; lp++, p++)
	*p = lp->text;

      free (lineinfo);
    }
  else
    qsort (linearray, nextline - linearray, sizeof (char *), compare_full);

  /* Open the output file */

  if (outfile)
    {
      ostream = fopen (outfile, "w");
      if (!ostream)
	pfatal_with_name (outfile);
    }

  writelines (linearray, nextline - linearray, ostream);
  if (outfile) fclose (ostream);

  free (linearray);
  free (data);
}

/* Parse an input string in core into lines.
 `data' is the input string, and `size' is its length.
 Data goes in `linearray' starting at `nextline'.
 The value returned is the first entry in `linearray' still unused.  */

char **
parsefile (filename, nextline, data, size)
     char *filename;
     char **nextline;
     char *data;
     long size;
{
  char *p, *end;
  char **line = nextline;

  p = data;
  end = p + size;
  *end = 0;

  while (p != end)
    {
      *line = p;
      while (*p && *p != '\n') p++;
      if (p != end) p++;

  /* This feature will be installed later.  */
  /*      if (discard_empty_lines && p == *line + 1) continue;  */

      line++;
      if (line == linearray + lines)
	{
	  char **old = linearray;
	  linearray = (char **) xrealloc (linearray, sizeof (char *) * (lines
	      *= 4));
	  line += linearray - old;
	}
    }

  return line;
}

/* Indexification is a filter applied to the sorted lines
 as they are being written to the output file.
 Multiple entries for the same name, with different page numbers,
 get combined into a single entry with multiple page numbers.
 The first braced field, which is used for sorting, is discarded.
 However, its first character is examined, folded to lower case,
 and if it is different from that in the previous line fed to us
 a \initial line is written with one argument, the new initial.

 If an entry has four braced fields, then the second and third
 constitute primary and secondary names.
 In this case, each change of primary name
 generates a \primary line which contains only the primary name,
 and in between these are \secondary lines which contain
 just a secondary name and page numbers.
*/

/* The last primary name we wrote a \primary entry for.
 If only one level of indexing is being done, this is the last name seen */
char *lastprimary;
int lastprimarylength;  /* Length of storage allocated for lastprimary */

/* Similar, for the secondary name. */
char *lastsecondary;
int lastsecondarylength;

/* Zero if we are not in the middle of writing an entry.
 One if we have written the beginning of an entry but have not
  yet written any page numbers into it.
 Greater than one if we have written the beginning of an entry
  plus at least one page number. */
int pending;

/* The initial (for sorting purposes) of the last primary entry written.
 When this changes, a \initial {c} line is written */

char * lastinitial;

int lastinitiallength;

/* When we need a string of length 1 for the value of lastinitial,
   store it here.  */

char lastinitial1[2];

/* Initialize static storage for writing an index */

void
init_index ()
{
  pending = 0;
  lastinitial = lastinitial1;
  lastinitial1[0] = 0;
  lastinitial1[1] = 0;
  lastinitiallength = 0;
  lastprimarylength = 100;
  lastprimary = (char *) xmalloc (lastprimarylength + 1);
  bzero (lastprimary, lastprimarylength + 1);
  lastsecondarylength = 100;
  lastsecondary = (char *) xmalloc (lastsecondarylength + 1);
  bzero (lastsecondary, lastsecondarylength + 1);
}

static int pnr_begin = -1;	/* current page number range */
static int pnr_end = -1;

/* Indexify.  Merge entries for the same name,
 insert headers for each initial character, etc.  */

void
indexify (line, ostream)
     char *line;
     FILE *ostream;
{
  char *primary, *secondary, *pagenumber;
  int number;
  int primarylength, secondarylength, pagelength;
  int nosecondary;
  int initiallength;
  char *initial;
  char initial1[2];
  register char *p;

  /* First, analyze the parts of the entry fed to us this time */

  p = find_braced_pos (line, 0, 0, 0);
  if (*p == '{')
    {
      initial = p;
      /* Get length of inner pair of braces starting at p,
	 including that inner pair of braces.  */
      initiallength = find_braced_end (p + 1) + 1 - p;
    }
  else
    {
      initial = initial1;
      initial1[0] = *p;
      initial1[1] = 0;
      initiallength = 1;

      if (initial1[0] >= 'a' && initial1[0] <= 'z')
	initial1[0] -= 040;
    }

  pagenumber = find_braced_pos (line, 1, 0, 0);
  pagelength = find_braced_end (pagenumber) - pagenumber;
  if (pagelength == 0)
    abort ();

  primary = find_braced_pos (line, 2, 0, 0);
  primarylength = find_braced_end (primary) - primary;

  secondary = find_braced_pos (line, 3, 0, 0);
  nosecondary = !*secondary;
  if (!nosecondary)
    secondarylength = find_braced_end (secondary) - secondary;

  /* If the primary is different from before, make a new primary entry */
  if (strncmp (primary, lastprimary, primarylength))
    {
      /* Close off current secondary entry first, if one is open */
      if (pending)
	{
	  put_range(ostream);
	  fputs ("}\n", ostream);
	  pending = 0;
	}

      /* If this primary has a different initial, include an entry for the
	initial */
      if (initiallength != lastinitiallength || strcmp (initial, lastinitial))
	{
	  (void)fprintf (ostream, "\n\n\\initial {");
	  if (strchr("^~\\",*initial) != (char*)NULL)
	      (void)fprintf(ostream,"\verb|%c|",*initial);
	  else
	  {
		if (strchr("&$#%_{}",*initial) != (char*)NULL)
	            putc('\\',ostream);	/* protect special characters */
		fwrite (initial, 1, initiallength, ostream);
	  }
	  (void)fprintf (ostream, "}\n", initial);
	  if (initial == initial1)
	    {
	      lastinitial = lastinitial1;
	      *lastinitial1 = *initial1;
	    }
	  else
	    {
	      lastinitial = initial;
	    }
	  lastinitiallength = initiallength;
	}

      /* Make the entry for the primary.  */
      if (nosecondary)
	fputs ("\\entry {", ostream);
      else
	fputs ("\\primary {", ostream);
      fwrite (primary, primarylength, 1, ostream);
      if (nosecondary)
	{
	  fputs ("}{", ostream);
	  pending = 1;
	}
      else
	fputs ("}\n", ostream);

      /* Record name of most recent primary */
      if (lastprimarylength < primarylength)
	{
          lastprimarylength = primarylength + 100;
	  lastprimary = (char *) xrealloc (lastprimary,
					   1 + lastprimarylength);
	}
      strncpy (lastprimary, primary, primarylength);
      lastprimary[primarylength] = 0;

      /* There is no current secondary within this primary, now */
      lastsecondary[0] = 0;
    }

  /* Should not have an entry with no subtopic following one with a subtopic */

  if (nosecondary && *lastsecondary)
    error ("entry %s follows an entry with a secondary name", line);

  /* Start a new secondary entry if necessary */
  if (!nosecondary && strncmp (secondary, lastsecondary, secondarylength))
    {
      if (pending)
	{
	  put_range(ostream);
	  fputs ("}\n", ostream);
	  pending = 0;
	}

      /* Write the entry for the secondary.  */
      fputs ("\\secondary {", ostream);
      fwrite (secondary, secondarylength, 1, ostream);
      fputs ("}{", ostream);
      pending = 1;

      /* Record name of most recent secondary */
      if (lastsecondarylength < secondarylength)
	{
          lastsecondarylength = secondarylength + 100;
	  lastsecondary = (char *) xrealloc (lastsecondary,
					   1 + lastsecondarylength);
	}
      strncpy (lastsecondary, secondary, secondarylength);
      lastsecondary[secondarylength] = 0;
    }

  /* Here to add one more page number to the current entry */
  if (isdigit(*pagenumber))		/* then non-negative arabic page
  					   number */
  {
      number = atoi(pagenumber);
      if (pnr_begin < 0)		/* start new range */
      {
          pnr_begin = number;
	  pnr_end = number;
      }
      else if (number == (pnr_end + 1))	/* extend range */
          pnr_end = number;
      else /* output current range and start new one */
      {
          put_range(ostream);
	  pnr_begin = number;
	  pnr_end = number;
	  pending++;
      }
  }
  else	/* non-arabic page number (probably roman numerals) */
  {
    if (pnr_begin >= 0)			/* output current page range */
    {
      put_range(ostream);
      pending++;
    }
    if (pending > 1)
      fputs (", ", ostream);	/* Punctuate first, if this is not the first */
    pending++;
    fwrite (pagenumber, pagelength, 1, ostream);
  }
}

void
put_range(ostream)		/* output current page number range */
    FILE *ostream;
{
  if (pnr_begin > 0)		/* output pending range */
  {
    if (pending > 1)
      fputs (", ", ostream);	/* Punctuate first, this is not the first */
    if (pnr_end > pnr_begin)
      (void)fprintf(ostream,"%d--%d",pnr_begin,pnr_end);
    else
      (void)fprintf(ostream,"%d",pnr_begin);
    pnr_begin = -1;
    pnr_end = -1;
  }
}


/* Close out any unfinished output entry */

void
finish_index (ostream)
     FILE *ostream;
{
  put_range(ostream);
  if (pending)
    fputs ("}\n", ostream);
  free (lastprimary);
  free (lastsecondary);
}

/* Copy the lines in the sorted order.
 Each line is copied out of the input file it was found in. */

void
writelines (linearray, nlines, ostream)
     char **linearray;
     int nlines;
     FILE *ostream;
{
  char **stop_line = linearray + nlines;
  char **next_line;

  init_index ();

  /* Output the text of the lines, and free the buffer space */

  for (next_line = linearray; next_line != stop_line; next_line++)
    {
      /* If -u was specified, output the line only if distinct from previous
	one.  */
      if (next_line == linearray
	  /* Compare previous line with this one, using only the explicitly
             specd keyfields */
	  || compare_general (*(next_line - 1), *next_line, 0L, 0L,
	     num_keyfields))
	{
	  char *p = *next_line;
	  char c;
	  while ((c = *p++) && c != '\n');
	  *(p-1) = 0;
	  indexify (*next_line, ostream);
	}
    }

  finish_index (ostream);
}

/* Assume (and optionally verify) that each input file is sorted;
 merge them and output the result.
 Returns nonzero if any input file fails to be sorted.

 This is the high-level interface that can handle an unlimited number of
 files.  */

#define MAX_DIRECT_MERGE 10

int
merge_files (infiles, nfiles, outfile)
     char **infiles;
     int nfiles;
     char *outfile;
{
  char **tempfiles;
  int ntemps;
  int i;
  int value = 0;
  int start_tempcount = tempcount;

  if (nfiles <= MAX_DIRECT_MERGE)
    return merge_direct (infiles, nfiles, outfile);

  /* Merge groups of MAX_DIRECT_MERGE input files at a time,
     making a temporary file to hold each group's result.  */

  ntemps = (nfiles + MAX_DIRECT_MERGE - 1) / MAX_DIRECT_MERGE;
  tempfiles = (char **) xmalloc (ntemps * sizeof (char *));
  for (i = 0; i < ntemps; i++)
    {
      int nf = MAX_DIRECT_MERGE;
      if (i + 1 == ntemps)
	nf = nfiles - i * MAX_DIRECT_MERGE;
      tempfiles[i] = maketempname (++tempcount);
      value |= merge_direct (&infiles[i * MAX_DIRECT_MERGE], nf, tempfiles[i]);
    }

  /* All temporary files that existed before are no longer needed
     since their contents have been merged into our new tempfiles.
     So delete them.  */
  flush_tempfiles (start_tempcount);

  /* Now merge the temporary files we created.  */

  merge_files (tempfiles, ntemps, outfile);

  free (tempfiles);

  return value;
}

/* Assume (and optionally verify) that each input file is sorted;
 merge them and output the result.
 Returns nonzero if any input file fails to be sorted.

 This version of merging will not work if the number of
 input files gets too high.  Higher level functions
 use it only with a bounded number of input files.  */

int
merge_direct (infiles, nfiles, outfile)
     char **infiles;
     int nfiles;
     char *outfile;
{
  struct linebuffer *lb1, *lb2;
  struct linebuffer **thisline, **prevline;
  FILE **streams;
  int i;
  int nleft;
  int lossage = 0;
  int *file_lossage;
  struct linebuffer *prev_out = 0;
  FILE *ostream = stdout;

  if (outfile)
    ostream = fopen (outfile, "w");
  if (!ostream) pfatal_with_name (outfile);

  init_index ();

  if (nfiles == 0)
    {
      if (outfile)
        fclose (ostream);
      return 0;
    }

  /* For each file, make two line buffers.
     Also, for each file, there is an element of `thisline'
     which points at any time to one of the file's two buffers,
     and an element of `prevline' which points to the other buffer.
     `thisline' is supposed to point to the next available line from the file,
     while `prevline' holds the last file line used,
     which is remembered so that we can verify that the file is properly
     sorted. */

  /* lb1 and lb2 contain one buffer each per file */
  lb1 = (struct linebuffer *) xmalloc (nfiles * sizeof (struct linebuffer));
  lb2 = (struct linebuffer *) xmalloc (nfiles * sizeof (struct linebuffer));

  /* thisline[i] points to the linebuffer holding the next available line in
     file i, or is zero if there are no lines left in that file.  */
  thisline = (struct linebuffer **) xmalloc (nfiles * sizeof (struct
	linebuffer *));
  /* prevline[i] points to the linebuffer holding the last used line from file
     i.  This is just for verifying that file i is properly sorted.  */
  prevline = (struct linebuffer **) xmalloc (nfiles * sizeof (struct
	linebuffer *));
  /* streams[i] holds the input stream for file i.  */
  streams = (FILE **) xmalloc (nfiles * sizeof (FILE *));
  /* file_lossage[i] is nonzero if we already know file i is not properly
     sorted.  */
  file_lossage = (int *) xmalloc (nfiles * sizeof (int));

  /* Allocate and initialize all that storage */

  for (i = 0; i < nfiles; i++)
    {
      initbuffer (&lb1[i]);
      initbuffer (&lb2[i]);
      thisline[i] = &lb1[i];
      prevline[i] = &lb2[i];
      file_lossage[i] = 0;
      streams[i] = fopen (infiles[i], "r");
      if (!streams[i])
	pfatal_with_name (infiles[i]);

      (void)readline (thisline[i], streams[i]);
    }

  /* Keep count of number of files not at eof */
  nleft = nfiles;

  while (nleft)
    {
      struct linebuffer *best = 0;
      struct linebuffer *exch;
      int bestfile = -1;
      int i;

      /* Look at the next avail line of each file; choose the least one.  */

      for (i = 0; i < nfiles; i++)
	{
	  if (thisline[i] &&
	      (!best ||
	       0 < compare_general (best->buffer, thisline[i]->buffer,
				    (long) bestfile, (long) i, num_keyfields)))
	    {
	      best = thisline[i];
	      bestfile = i;
	    }
	}

      /* Output that line, unless it matches the previous one and we don't
	want duplicates */

      if (!(prev_out &&
	    !compare_general (prev_out->buffer, best->buffer, 0L, 1L,
	num_keyfields)))
	indexify (best->buffer, ostream);
      prev_out = best;

      /* Now make the line the previous of its file, and fetch a new line from
	that file */

      exch = prevline[bestfile];
      prevline[bestfile] = thisline[bestfile];
      thisline[bestfile] = exch;

      while (1)
	{
	  /* If the file has no more, mark it empty */

	  if (feof (streams[bestfile]))
	    {
	      thisline[bestfile] = 0;
	      nleft--;		/* Update the number of files still not empty */
	      break;
	    }
	  (void)readline (thisline[bestfile], streams[bestfile]);
	  if (thisline[bestfile]->buffer[0] || !feof (streams[bestfile])) break;
	}
    }

  finish_index (ostream);

  /* Free all storage and close all input streams */

  for (i = 0; i < nfiles; i++)
    {
      fclose (streams[i]);
      free (lb1[i].buffer);
      free (lb2[i].buffer);
    }
  free (file_lossage);
  free (lb1);
  free (lb2);
  free (thisline);
  free (prevline);
  free (streams);

  if (outfile)
    fclose (ostream);

  return lossage;
}

/* Print error message and exit.  */

void
fatal (s1, s2)
     char *s1, *s2;
{
  error (s1, s2);

  EXIT (1);
}

/* Print error message.  `s1' is printf control string, `s2' is arg for it. */

void
error (s1, s2)
     char *s1, *s2;
{
  (void)printf ("texi: ");
  (void)printf (s1, s2);
  (void)printf ("\n");
}

void
pfatal_with_name (name)
     char *name;
{

#ifdef OS_VAXVMS
  extern noshare int errno;
  extern noshare int sys_nerr;
  extern noshare char *sys_errlist[];
#else
  extern int errno, sys_nerr;
  extern char *sys_errlist[];
#endif

  char *s;

  if (errno < sys_nerr)
    s = concat ("", sys_errlist[errno], " for %s");
  else
    s = "cannot open %s";
  fatal (s, name);
}

/* Return a newly-allocated string whose contents concatenate those of s1, s2,
   s3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  char *result = (char *) xmalloc (len1 + len2 + len3 + 1);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  *(result + len1 + len2 + len3) = 0;

  return result;
}

/* Like malloc but get fatal error if memory is exhausted.  */

char*
xmalloc (size)
     int size;
{
  char* result = (char *)malloc (size);
  if (!result)
    fatal ("virtual memory exhausted", 0);
  return result;
}


char*
xrealloc (ptr, size)
     char *ptr;
     int size;
{
  char* result = (char *)realloc (ptr, size);
  if (!result)
    fatal ("virtual memory exhausted", 0);
  return result;
}

#include "strchr.h"

