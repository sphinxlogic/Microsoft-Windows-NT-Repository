/*
 *  Copyright (c) 1994 John E. Davis  (davis@amy.tch.harvard.edu)
 *  All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>

/* For isatty */
#ifdef msdos
#include <io.h>
#endif

#include "slang.h"
#include "vfile.h"

static int Case_Sensitive = 1;
static int File_Name_Only;
static int Do_Recursive = 0;
static int Recursive_Match = 0;
static int Highlight = 0;
static int Output_Match_Only = 0;
static int Count_Matches = 0;
static int Line_Numbers = 0;
static int Follow_Links = 0;
static int Debug_Mode = 0;
static char *Match_This_Extension;

#define HON_STR "\033[1m"
#define HON_STR_LEN 4
#define HOFF_STR "\033[0m"
#define HOFF_STR_LEN 4

void usage(void)
{
   fputs("rgrep (v1.0)\nUsage: rgrep [options..] pattern [files ...]\n\
Options:\n\
  -?        additional help (use '-?' to avoid shell expansion on some systems)\n\
  -c        count matches\n\
  -h        highlight match (ANSI compatable terminal assumed)\n\
  -H        Output match instead of entire line containing match\n\
  -i        ignore case\n\
  -l        list filename only\n\
  -n        print line number of match\n\
  -F        follow links\n\
  -r        recursively scan through directory tree\n\
  -R 'pat'  like '-r' except that only those files matching 'pat' are checked\n\
  -x 'ext'  checks only files with extension given by 'ext'.\n\
\n\
'pattern' is a valid 'ex' type of regular expression.  See the man page for ex.\n\
It is best enclosed in single quotes to avoid shell expansion.\n", stderr);

   exit(1);
}

void additional_help (void)
{
   char buf[3];
   fputs("Supported Regular Expressions:\n\
   .                  match any character except newline\n\
   *                  matches zero or more occurences of previous RE\n\
   +                  matches one or more occurences of previous RE\n\
   ?                  matches zero or one occurence of previous RE\n\
   ^                  matches beginning of line\n\
   $                  matches end of line\n\
   [ ... ]            matches any single character between brackets.\n\
                      For example, [-02468] matches `-' or any even digit.\n\
		      and [-0-9a-z] matches `-' and any digit between 0 and 9\n\
		      as well as letters a through z.\n\
   \\{ ... \\}\n\
   \\( ... \\)\n\
   \\1, \\2, ..., \\9    matches match specified by nth \\( ... \\) expression.\n\
                      For example, '\\([ \\t][a-zA-Z]+\\)\\1[ \\t]' matches any\n\
		      word repeated consecutively.\n", stderr);
   if (isatty(fileno(stderr)) && isatty(fileno(stdin)))
     {
	fputs("\nPress RETURN for examples>", stderr);
	fgets(buf, 2, stdin);
	putc('\n', stderr);
     }
   fputs("Examples:\n\
\n\
 Look in all files with a 'c' extension in current directory and all its\n\
 subdirectories looking for matches of 'int ' at the beginning of a line,\n\
 printing the line containing the match with its line number: (two methods)\n\
    rgrep -n -R '*.c' '^int ' .\n\
    rgrep -n -x c '^int ' .\n\
\n\
 Highlight all matches of repeated words in file 'paper.tex':\n\
    rgrep -h '[ \\t]\\([a-zA-Z]+\\)[ \\t]+\\1[ \\t\\n]' paper.tex\n\
    rgrep -h '^\\([a-zA-Z]+\\)[ \\t]+\\1[ \\t\\n]' paper.tex\n\
   (Note that this version of rgrep requires two passes for this example)\n", stderr);
   exit (-1);
}

   
void parse_flags(char *f)
{
   char ch;
   while ((ch = *f++) != 0)
     {
	switch (ch)
	  {
	   case 'i': Case_Sensitive = 0; break;
	   case 'l': File_Name_Only = 1; break;
	   case 'r': Do_Recursive = 1; break;
	   case 'H':
	     Highlight = 1;	       /* does not cause highlight for this case */
	     Output_Match_Only = 1;
	     break;
	   case 'h':
#ifndef pc_system
	     Highlight = 1;
#endif
	     break;
	   case 'c': Count_Matches = 1; break;
	   case 'n': Line_Numbers = 1; break;
	   case 'F': Follow_Links = 1; break;
	   case 'D': Debug_Mode = 1; break;
	   case '?': additional_help (); break;
	   default: usage ();
	  }
     }
}

/* 8bit clean upper and lowercase macros */
unsigned char Chg_LCase_Lut[256];
unsigned char Chg_UCase_Lut[256];

void SLang_define_case(int *u, int *l)
{
   unsigned char up = (unsigned char) *u, dn = (unsigned char) *l;
   
   Chg_LCase_Lut[up] = dn;
   Chg_UCase_Lut[dn] = up;
}

void init_lut(void)
{
   int i,j;
   
   for (i = 0; i < 256; i++) 
     {
	Chg_UCase_Lut[i] = i;
	Chg_LCase_Lut[i] = i;
     }
   
   for (i = 'A'; i <= 'Z'; i++) 
     {
	j = i + 32;
	Chg_UCase_Lut[j] = i;
	Chg_LCase_Lut[i] = j;
     }
#ifdef msdos
   /* Initialize for DOS code page 437. */
   Chg_UCase_Lut[135] = 128; Chg_LCase_Lut[128] = 135;
   Chg_UCase_Lut[132] = 142; Chg_LCase_Lut[142] = 132;
   Chg_UCase_Lut[134] = 143; Chg_LCase_Lut[143] = 134;
   Chg_UCase_Lut[130] = 144; Chg_LCase_Lut[144] = 130;
   Chg_UCase_Lut[145] = 146; Chg_LCase_Lut[146] = 145;
   Chg_UCase_Lut[148] = 153; Chg_LCase_Lut[153] = 148;
   Chg_UCase_Lut[129] = 154; Chg_LCase_Lut[154] = 129;
   Chg_UCase_Lut[164] = 165; Chg_LCase_Lut[165] = 164;
#else
   /* ISO Latin */
   for (i = 192; i <= 221; i++) 
     {
	j = i + 32;
	Chg_UCase_Lut[j] = i;
	Chg_LCase_Lut[i] = j;
     }
   Chg_UCase_Lut[215] = 215; Chg_LCase_Lut[215] = 215;
   Chg_UCase_Lut[223] = 223; Chg_LCase_Lut[223] = 223;
   Chg_UCase_Lut[247] = 247; Chg_LCase_Lut[247] = 247;
   Chg_UCase_Lut[255] = 255; Chg_LCase_Lut[255] = 255;
#endif

}


#define UPPER_CASE(x) (Chg_UCase_Lut[(unsigned char) (x)])
#define upcase(ch) (cs ? ch : UPPER_CASE(ch))

static int ind[256];

unsigned char *forw_search_region
    (register unsigned char *beg, unsigned char *end, unsigned char *key, register int key_len)
{
   register unsigned char char1;
   unsigned char *pos;
   int j, str_len;
   register unsigned char ch;
   register int db;
   int cs = Case_Sensitive;
   

   str_len = (int) (end - beg);
   if (str_len < key_len) return (NULL);

   char1 = key[key_len - 1];
   beg += (key_len - 1);

   while(1)
     {
	if (cs) while (beg < end)
	  {
	     ch = *beg;
	     db = ind[(unsigned char) ch];
	     if ((db < key_len) && (ch == char1)) break;
	     beg += db; /* ind[(unsigned char) ch]; */
	  }
	else while (beg < end)
	  {
	     ch = *beg;
	     db = ind[(unsigned char) ch];
	     if ((db < key_len) && 
		 (UPPER_CASE(ch) == char1)) break;
	     beg += db; /* ind[(unsigned char) ch]; */
	  }
	
	if (beg >= end) return(NULL);
	
	pos = beg - (key_len - 1);
	for (j = 0; j < key_len; j++)
	  {
	     ch = upcase(pos[j]);
	     if (ch != (unsigned char) key[j]) break;
	  }
	
	if (j == key_len) return(pos);
	beg += 1;
     }
}

static int key_len;
static unsigned char search_buf[256];


static void upcase_search_word(unsigned char *str)
{
   int i, maxi;
   int cs = Case_Sensitive;
   register int max = strlen((char *) str);
   char *w;
   register int *indp, *indpm;
   
   w = (char *) search_buf;
   indp = ind; indpm = ind + 256; while (indp < indpm) *indp++ = max;
   
   i = 0;
   while (i++ < max)
     {
	maxi = max - i;
	if (cs)
	  {
	     *w = *str;
	     ind[(unsigned char) *str] = maxi;
	  }
	else
	  {
	     *w = UPPER_CASE(*str);
	     ind[(unsigned char) *w] = maxi;
	     ind[(unsigned char) LOWER_CASE(*str)] = maxi;
	  }
	str++; w++;
     }
   search_buf[max] = 0;
   key_len = max;
}



void msg_error(char *str)
{
   fputs(str, stderr);
   putc('\n', stderr);
}

void exit_error(char *s)
{
   fprintf(stderr, "rgrep: %s\n", s);
   exit(1);
}

static SLRegexp_Type reg;
static SLRegexp_Type recurse_reg;
static int Must_Match;
static int print_file_too;

void output_line(unsigned char *s, unsigned int n, unsigned char *p, unsigned char *pmax)
{
   if (Highlight == 0)
     {
	fwrite(s, 1, n, stdout);
     }
   else
     {
	if (Output_Match_Only == 0)
	  {
	     fwrite (s, 1, (int) (p - s), stdout);
	     fwrite (HON_STR, 1, HON_STR_LEN, stdout);
	  }
	
	fwrite (p, 1, (int) (pmax - p), stdout);
	if (Output_Match_Only == 0)
	  {
	     fwrite (HOFF_STR, 1, HOFF_STR_LEN, stdout);
	     fwrite (pmax, 1, (int) n - (int) (pmax - s), stdout);
	  }
	else if (*(pmax - 1) != '\n') fwrite("\n", 1, 1, stdout);
     }
}


static VFILE *vfile_vp;

void grep(char *file)
{
   unsigned char *buf, *p, *pmax;
   unsigned int n;
   int line = 0, n_matches = 0;
   
   while (NULL != (buf = (unsigned char *) vgets(vfile_vp, &n)))
     {
	line++;
	if (Must_Match)
	  {
	     if (key_len > n) continue;
	     if (NULL == (p = forw_search_region(buf, buf + n, search_buf, key_len)))
	       {
		  continue;
	       }
	     if (reg.osearch) 
	       {
		  pmax = p + key_len;
		  goto match_found;
	       }
	  }
	
	if (!SLang_regexp_match(buf, (int) n, &reg)) continue;
	p = buf + reg.beg_matches[0];
	pmax = p + reg.end_matches[0];
	
	match_found:
	n_matches++;
	
	if (Count_Matches) continue;
	if (File_Name_Only)
	  {
	     puts(file);
	     return;
	  }
	if (print_file_too)
	  {
	     fputs(file, stdout);
	     putc(':', stdout);
	  }
	if (Line_Numbers)
	  {
	     fprintf(stdout, "%d:", line);
	  }
	     
	output_line(buf, n, p, pmax);
     }
   if (n_matches && Count_Matches)
     {
	if (print_file_too || File_Name_Only)
	  {
	     fputs(file, stdout);
	     putc(':', stdout);
	  }
	fprintf(stdout, "%d\n", n_matches);
     }
}

#ifdef msdos
#include <dir.h>
#endif

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#ifdef sequent
# include <sys/dir.h>
# define NEED_D_NAMLEN
#else
# include <dirent.h>
#endif
#endif


#ifdef msdos
#define MAX_PATH_LEN 128
#else
#define MAX_PATH_LEN 512
#endif

#ifdef msdos
typedef struct Dos_DTA_Type
{
   unsigned char undoc[21];
   unsigned char attr;
   unsigned int time;
   unsigned int date;
   unsigned char low_size[2];
   unsigned char high_size[2];
   char name[13];
} DOS_DTA_Type;
#endif

typedef struct
{
   char dir[MAX_PATH_LEN];
   int dir_len;
   char *file;			       /* pointer to place in dir */
   int isdir;
#ifdef msdos
   DOS_DTA_Type *dta;
   char pattern[16];
#endif
#ifdef unix
   DIR *dirp;
#endif
} Sys_Dir_Type;
   

#ifdef msdos 
void dos_set_dta (DOS_DTA_Type *dta)
{
   asm mov ah, 0x1A
   asm push ds
   asm lds dx, dword ptr dta
   asm int 21h
   asm pop ds
}
#endif

#ifdef unix
int unix_is_dir(char *dir)
{
/* AIX requires this */
#ifdef _S_IFDIR
#ifndef S_IFDIR
#define S_IFDIR _S_IFDIR
#endif
#endif
   struct stat buf;
   int mode;

#ifdef S_IFLNK
   if (Follow_Links)
     {
#endif
	if (stat(dir, &buf)) return -1;
#ifdef S_IFLNK
     }
   else if (lstat(dir, &buf) < 0) return -1;
#endif
   
   mode = buf.st_mode & S_IFMT;
   
#ifdef S_IFLNK
   if (mode == S_IFLNK) return (-1);
#endif
   if (mode == S_IFDIR) return (1);
   if (mode != S_IFREG) return (-1);
   
   return(0);
}
#endif  /* unix */

Sys_Dir_Type *sys_opendir(char *dir, Sys_Dir_Type *x)
{
#ifdef msdos
   char slash = '\\';
   char *pat = "*.*";
   dos_set_dta (x->dta);
   
   if ((dir[1] == ':') && (dir[2] == '\\'))
     {
	strcpy (x->dir, dir);
     }
   else
     {
	/* must have drive/dirpath/filename */
	getcwd(x->dir, MAX_PATH_LEN);
	if (*dir == slash) 
	  {
	     strcpy (x->dir + 2, dir);
	  }
	else
	  {
	     if (x->dir[strlen (x->dir) - 1] != slash) strcat (x->dir, "\\");
	     strcat(x->dir, dir);
	  }
     }
   
   dir = x->dir + strlen (x->dir);
   /* check for a pattern already as part of the dirspec */
   while (dir > x->dir)
     {
	if (*dir == '\\') break;
	if (*dir == '*')
	  {
	     while (*dir != '\\') dir--;
	     *dir = 0;
	     pat = dir + 1;
	     break;
	  }
	dir--;
     }
   strcpy (x->pattern, pat);
   
#else
#ifdef unix
   char *p;
   char slash = '/';
   DIR *dirp;
   if (NULL == (dirp = (DIR *) opendir(dir)))
     {
	fprintf (stderr, "rgrep: dir %s not readable.\n", dir);
	return NULL;
     }
   x->dirp = dirp;
   strcpy(x->dir, dir);   
#endif /* unix */
#endif /* msdos */
   x->dir_len = strlen(x->dir);
   if (x->dir[x->dir_len - 1] != slash) 
     {
	x->dir[x->dir_len++] = slash;
	x->dir[x->dir_len] = 0;
     }
   return (x);
}



void sys_closedir(Sys_Dir_Type *x)
{
#ifdef msdos
   (void) x;
#else
#ifdef unix
   DIR *dirp;
   dirp = x->dirp;
   if (dirp != NULL) closedir(dirp);
   x->dirp = NULL;
#endif
#endif
}

#ifdef msdos
char *dos_dta_fixup_name (Sys_Dir_Type *x)
{
   x->file = x->dir + x->dir_len;
   strcpy(x->file, x->dta->name);
   /* sub directory */
   if (x->dta->attr & 0x10) x->isdir = 1; else x->isdir = 0;
   return x->file;
}
#endif

char *sys_dir_findnext(Sys_Dir_Type *x)
{
   char *file;
#ifdef msdos
   asm mov ah, 0x4F
   asm int 21h
   asm jnc L1
   return NULL;
   
   L1:
   file = dos_dta_fixup_name (x);
#else
   
#ifdef unix
#  ifdef NEED_D_NAMLEN
#    define dirent direct
#  endif
   struct dirent *dp;
   DIR *d;
   d = x->dirp;
   
   if (NULL == (dp = readdir(d))) return(NULL);
#  ifdef NEED_D_NAMLEN
     dp->d_name[dp->d_namlen] = 0;
#  endif
   file = dp->d_name;
   x->file = x->dir + x->dir_len;
   strcpy (x->file, dp->d_name);
   x->isdir = unix_is_dir(x->dir);
#endif /* unix */
#endif /* msdos */
   /* exclude '.' and '..' */
   if (*file++ == '.')
     {
	if ((*file == 0) || 
	    ((*file == '.') && (*(file + 1) == 0))) x->isdir = -1;
     }
   return (x->dir);
}

char *sys_dir_findfirst(Sys_Dir_Type *x)
{
#ifdef msdos
   unsigned int attr = 0x1 | 0x10;     /* read only + sub directory */
   char pat[MAX_PATH_LEN], *patp, *file;
   
   attr |= 0x2 | 0x4;		       /* hidden and system */
   
   strcpy (pat, x->dir);
   strcat (pat, x->pattern);
   patp = pat;
   
   asm mov ah, 0x4e
   asm mov cx, attr
   asm push ds
   asm lds dx, dword ptr patp
   asm int 21h
   asm pop ds
   asm jc L1
   
   file = dos_dta_fixup_name (x);
   /* exclude '.' and '..' */
   if (*file++ == '.')
     {
	if ((*file == 0) || 
	    ((*file == '.') && (*(file + 1) == 0))) x->isdir = -1;
     }
   return x->dir;
   
   L1:  return NULL;
#else
#ifdef unix
   return (sys_dir_findnext(x));
#endif
#endif
}

#define BUF_SIZE 4096

void grep_file(char *file, char *filename)
{
   char *p;
   if (Debug_Mode) return;
   if (Recursive_Match)
     {
	if (Match_This_Extension != NULL)
	  {
	     p = filename + strlen(filename);
	     while ((p >= filename) && (*p != '.')) p--;
	     if ((*p != '.') ||
#ifdef msdos
		 stricmp(Match_This_Extension, p + 1)
#else
		 strcmp(Match_This_Extension, p + 1)
#endif
		 )
	       return;
	  }
	else
	  if (!SLang_regexp_match((unsigned char *) filename, strlen(filename), &recurse_reg)) return;
     }
   
   vfile_vp = vopen (file, BUF_SIZE, 0);
   if (vfile_vp != NULL)
     {
	grep(file);
	vclose(vfile_vp);
     }
   else fprintf(stderr, "rgrep: unable to read %s\n", file);
}

#define MAX_DEPTH 25
void grep_dir(char *dir)
{
   static int depth;
   Sys_Dir_Type x;
   char *file;
#ifdef msdos
   DOS_DTA_Type dta;
   x.dta = &dta;
#endif
   
   if (NULL == sys_opendir(dir, &x)) return;
   if (depth >= MAX_DEPTH)
     {
	fprintf(stderr, "Maximum search depth exceeded.\n");
	return;
     }
   
   depth++;
   if (Debug_Mode) fprintf(stderr, "%s\n", dir);
   
   for (file = sys_dir_findfirst(&x); 
	file != NULL; file = sys_dir_findnext(&x))
     {
	if (x.isdir == 0) grep_file(file, x.file);
	else if (x.isdir == 1) grep_dir(file);
#ifdef msdos
	dos_set_dta (&dta);	       /* something might move it */
#endif
     }
   
   sys_closedir(&x);
   depth--;
}

   


int main(int argc, char **argv)
{
   unsigned char buf[256];
   unsigned char recurse_buf[256];
   char *file;
   
   argv++;
   argc--;
   
   while (argc && (**argv == '-') && *(*argv + 1))
     {
	if (!strcmp(*argv, "-R"))
	  {
	     argc--;
	     argv++;
	     if (!argc) usage();
	     recurse_reg.pat = (unsigned char *) *argv;
	     recurse_reg.buf = recurse_buf;
	     recurse_reg.buf_len = 256;
	     recurse_reg.case_sensitive = 1;
   
	     if (SLang_regexp_compile (&recurse_reg)) exit_error("Error compiling pattern.");
	     Do_Recursive = 1;
	     Recursive_Match = 1;
	  }
	else if (!strcmp(*argv, "-x"))
	  {
	     argc--;
	     argv++;
	     if (!argc) usage();
	     Recursive_Match = 1;
	     Match_This_Extension = *argv;
	  }
	else
	  {
	     parse_flags(*argv + 1);
	  }
	argv++; argc--;
     }
   
   if (!argc) usage();
   init_lut();
   
   reg.pat = (unsigned char *) *argv;
   reg.buf = buf;
   reg.buf_len = 256;
   reg.case_sensitive = Case_Sensitive;
   
   if (SLang_regexp_compile (&reg)) exit_error("Error compiling pattern.");
   argc--; argv++;

   Must_Match = 1;
   
   if (reg.osearch)
     {
        upcase_search_word(reg.pat);
     }
   else if (reg.must_match)
     {
	upcase_search_word(reg.must_match_str);
     }
   else Must_Match = 0;
   
	
   if (argc == 0)
     {
	vfile_vp = vstream(fileno(stdin), BUF_SIZE, 0);
	if (vfile_vp == NULL)
	  {
	     exit_error("Error vopening stdin.");
	  }
	grep("stdin");
	vclose(vfile_vp);
     }
   else
     {
	if (Do_Recursive || (argc != 1)) print_file_too = 1;
	while (argc--)
	  {
	     if (Do_Recursive
#ifdef msdos
		 && ('\\' == (*argv)[strlen(*argv) - 1])
#else
#ifdef unix
		 && (1 == unix_is_dir (*argv))
#endif
#endif
		 ) grep_dir (*argv);
	     else
#ifdef msdos
	       {
		  file = *argv;
		  while (*file && (*file != '*')) file++;
		  if (*file == '*') 
		    {
		       print_file_too = 1;
		       grep_dir (*argv); 
		    }
		  else grep_file(*argv, *argv);
	       }
#else
	     grep_file(*argv, *argv);
#endif
	     argv++;
	  }
     }
   return (0);
}


/* ------------------------------------------------------------ */

#ifdef VMS

int vms_expand_filename(char *file,char *expanded_file)
{
    unsigned long status;
    static int context = 0;
    static char inputname[256] = "";
    $DESCRIPTOR(file_desc,inputname);
    $DESCRIPTOR(default_dsc,"SYS$DISK:[]*.*;");
    static struct dsc$descriptor_s  result =
	    {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL};

    if (strcmp(inputname, file))
      {
	  if (context)
	    {
		lib$find_file_end(&context);
	    }
	  context = 0;
	  strcpy(inputname, file);
	  file_desc.dsc$w_length = strlen(inputname);
      }

    if (RMS$_NORMAL == lib$find_file(&file_desc,&result,&context,
	           		     &default_dsc,0,0,&Number_Zero))
      {
	  MEMCPY(expanded_file, result.dsc$a_pointer, result.dsc$w_length);
	  expanded_file[result.dsc$w_length] = '\0';
          return (1);
      }
    else
      {
          /* expanded_file[0] = '\0'; */      /* so file comes back as zero width */
          return(0);
      }
}

static int context = 0;

static char inputname[256] = "";
$DESCRIPTOR(file_desc,inputname);
$DESCRIPTOR(default_dsc,"SYS$DISK:[]*.*;");

int sys_findnext(char *file)
{
   unsigned long status;
   static struct dsc$descriptor_s  result = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, NULL};

   if (RMS$_NORMAL == lib$find_file(&file_desc,&result,&context,
				    &default_dsc,0,0,&Number_Zero))
     {
	MEMCPY(file, result.dsc$a_pointer, result.dsc$w_length);
	file[result.dsc$w_length] = 0;
	return (1);
     }
   else return(0);
}

int sys_findfirst(char *file)
{
   char *file;
   strcpy(inputname, file);
   file_desc.dsc$w_length = strlen(inputname);
   if (context) lib$find_file_end(&context);
   context = 0;
   return sys_findnext(file);
}
#endif
/* VMS */
