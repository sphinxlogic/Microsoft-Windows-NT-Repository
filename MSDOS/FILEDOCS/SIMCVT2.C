/*
I have uploaded a new version of simcvt.c for <msdos.filedocs>.
It makes the file grep.sim (also written by me) unnecessary.
It uses regex to reduce the display only to files of interest.
It can be made to automagically call your pager, and can do
case-insensitive searches.  It allows the input filename to be
specified on the command line, and can use a default.

My changes are public domain.  -David-

# david@wubios.wustl.edu             ^     Mr. David J. Camp
# david%wubios@wugate.wustl.edu    < * >   +1 314 382 0584
# ...!uunet!wugate!wubios!david      v     "God loves material things."
# abs (investment#1 - investment#2) << abs (anyinvestment - anydebt)
*/

/*****************************************************************************

Written by reynolds@sun.com                                     01/21/90
Minor corrections to instructions and portability
		davidsen@crdos1.crd.ge.com			02/23/90

This SIMCVT.C filter should convert Simtel-20's "SIMIBM.IDX" file into a
readable "SIMIBM.LST" that is compatible with the other convert programs,
except for the run-date at the top of the output file.

This program, written in "C" should compile on both 4.3BSD Unix machines,
as well as IBM/pc compatible machines.  It works on both VAXen, and Suns.

To Compile on Unix, type "cc -o simcvt SIMCVT.C" creating simcvt.
To Compile on IBM/pcs, see your C manual that came with the compiler.

To run, type "simcvt < simibm.idx > simibm.lst

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

char * re_comp ();
int re_exec ();

void
main (argc, argv)
int argc;
char * argv [];

{
char  fs[10],dir[60],name[15],descr[60]; /* input variables */
char  inputline[257];                    /* for initial read */
char  lowerline[257];                    /* inputline converted to lowercase */
int   rev,bits;                          /* input variables */
long  length,date;                       /* input variables */
char  lfs[10],ldir[60];                  /* stores last filesystem/directory */
char  type;                              /* output variable for 'A' or 'B' */
char  c;                                 /* picks off EOF,",linefeed */
FILE * output_file;                      /* output file handle */
char * pager_name;                       /* name of pager */
char * error_message;                    /* re_comp return value */
char * pattern;                          /* pattern to match */
int   arg_flag;                          /* command line flag */
extern char *optarg;                     /* optional argument string */
extern int optind;                       /* arg number being scanned */
int    insensitive;                      /* do search case-insensitively */
char * ptr;                              /* temporary ptr for scanning */
char * input_filename;                   /* name of input file */
FILE * input_file;                       /* handle for input file */

input_filename = NULL;
insensitive = 0;
output_file = stdout;
pager_name = NULL;
while ((arg_flag = getopt (argc, argv, "Ff:ip")) != -1)
    {
    switch (arg_flag) 
        {
    case 'F':
        input_filename = getenv ("IDX");
        break;
    case 'f':
        input_filename = optarg;
        break;
    case 'i':
        insensitive = 1;
        break;
    case 'p':
        pager_name = getenv ("PAGER");
        if (pager_name == NULL)
            pager_name = "more";    
        break;
        }
    }
if (optind < argc)
    {
    pattern = argv [optind];    
    optind ++;
    }
else
    pattern = ".*";
if (input_filename)
    {
    input_file = fopen (input_filename, "r");
    if (input_file == NULL)
        {
        fprintf (stderr, "Cannot open file: %s\n", input_filename);
        exit (3);
        }
    }
else if (isatty (fileno (stdin)))
    {
    fprintf (stderr, 
"Usage: simcvt [-f filename] [-Fip] [regex-pattern] [ < /path/simibm.idx]\n");
    fprintf (stderr,
"The simibm.idx file is taken from the standard input, \n");
    fprintf (stderr, 
"or from the file specified by the -f option.\n");
    fprintf (stderr, 
"The -F option selects the input file identied by the IDX variable.\n");
    fprintf (stderr, 
"The -i option makes pattern matching case-insensitive.\n");
    fprintf (stderr,
"The -p option will gate the printout to your pager.\n");
    fprintf (stderr,
"For example: simcvt -Fip \"pattern\"\n");
    pclose (output_file);
    exit (2);
    }
else
    input_file = stdin;
if (insensitive)
    {
    for (ptr = pattern; *ptr; ptr++)
        {
        if (isalpha (*ptr))
            *ptr = tolower (*ptr);
        }
    }
error_message = re_comp (pattern);
if (error_message != NULL)
    fprintf (stderr, "Regular Expression Error: %s\n", error_message);
if (pager_name != NULL)
    {
    output_file = popen (pager_name, "w");
    if (output_file == NULL)
        output_file = stdout;
    }
fprintf (output_file, "WSMR-SIMTEL20.ARMY.MIL PUBLIC DOMAIN LISTING\n\n");
fprintf (output_file, "NOTE: Type B is Binary; Type A is ASCII\n");
inputline[256] = 0;
while (fgets (inputline, 256, input_file) != NULL)
    {
    strcpy (lowerline, inputline);
    if (insensitive)
        {
        for (ptr = lowerline; *ptr; ptr++)
	    {
            if (isalpha (*ptr))
                *ptr = tolower (*ptr);
	    }
        }
    if (re_exec (lowerline))
        {
        sscanf(inputline, 
                "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",%d,%ld,%d,%ld,\"%[^\"]\"",
                fs, dir, name, &rev, &length, &bits, &date, descr);
        type = 'B';                           /* Binary 8-bit */
        if (bits == 7) type = 'A';            /* ASCII  7-bit */
        if (strcmp(ldir,dir) || strcmp(lfs,fs)) 
            {  /* New Directory */
            fprintf (output_file, "\nDirectory %s%s\n",fs,dir);
            fprintf (output_file, 
                    " Filename   Type Length   Date    Description\n");
            fprintf (output_file, 
                    "==============================================\n");
            strcpy(ldir, dir);        /* Remember last directory with ldir  */
            strcpy(lfs,fs);           /* Remember last file system with lfs */
            }                         /* End of the New Directory routine   */
        fprintf (output_file, 
                "%-12.12s  %c %7ld  %6ld  %s\n",name,type,length,date,descr);
        }
    }
if (output_file != stdout)
    pclose (output_file);
} /* end of main() program by Ray */

/*****************************************************************************

   This filter takes data in the following format:
"PD1:","<MSDOS.ADA>","ADA-LRM2.ARC",1,320086,8,890411,"The Ada Language Reference Manual reader (2/4)"

   And converts it to the following format:
ADA-LRM1.ARC  B  231947  890411  The Ada Language Reference Manual reader (1/4)

*****************************************************************************/
