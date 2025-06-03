/* Splits a large file into smaller ones  --- 
   This file also creates a descriptor file describing what happened. */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#ifndef sequent
#include <stdlib.h>
#endif

#ifdef VMS
#include <file.h>
#endif

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#ifdef SYSV
#include <sys/fcntl.h>
#endif
#endif

#ifdef msdos
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#endif

#include "vfile.h"
void usage(char *prog)
{
   char *str = "[-l max_lines] -f infile -o out_file_fmt -d dsc_name";
   fprintf(stderr, "%s: Usage: %s\n", prog, str);
   exit(1);
}

void error(char *fmt, char *str)
{
   fprintf(stderr, fmt, str);
   putc('\n', stderr);
   exit(1);
}

#ifndef msdos
#define MAX_LEN 65536
int Max_Lines = 2000;
#else
#define MAX_LEN 4096
int Max_Lines = 1000;
#endif

int Quiet = 0;
char *Dsc_File;
char *Split_Fmt;

FILE *open_descriptor_file()
{
   FILE *fp; 
   
#ifdef msdos
   _fmode = O_TEXT;
#endif
   if (NULL == (fp = fopen(Dsc_File, "w")))
     {
	error("Unable to create descriptor file %s", Dsc_File);
     }
#ifdef msdos
   _fmode = O_BINARY;
#endif
   return (fp);
}

void split(char *file)
{
   VFILE *vp;
   unsigned int total, len;
   register int n = Max_Lines + 1;
   int fp_split = -1, ext = -1;
   char *buf;
   char fsplit[256];
   FILE *fd;
   register int max_low = Max_Lines - 50;
   register int max_hi = Max_Lines + 50;
   register int flag;
   
   if (max_low < 0) max_low = Max_Lines;
#ifdef msdos
   _fmode = O_BINARY;
#endif
   if (NULL == (vp = vopen(file, MAX_LEN)))
     {
	error("Error opening %s\n", file);
     }
   
   fd = open_descriptor_file();
   fprintf(fd, "%s\n%s\n", file, Split_Fmt);
   while (NULL != (buf = vgets(vp, &len)))
     {	
	if ((n >= max_hi) || flag)
	  {  
	     if (fp_split != -1)
	       {
		  close(fp_split);
		  fprintf(fd, "%s : %d lines\n", fsplit, n);
	       }
	     sprintf(fsplit, Split_Fmt, ++ext);
#ifdef VMS
	     fp_split = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, 0, "rat=cr", "rfm=var");
#else
#ifdef unix
	     fp_split = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#else
	     fp_split = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
#endif
#endif
	     if (fp_split < 0)
	       {
		  error("Error creating %s", fsplit);
	       }
	     else if (!Quiet)
	       {
		  fprintf(stderr, "Writing %s\n", fsplit);
	       }
	     
	     n = 0;
	  }
	write(fp_split, buf, len);
	flag = 0;
	if ((n > max_low) && (*buf == '}')) flag = 1;
	n++;
     }
   
   if (fp_split != -1)
     {
	close(fp_split);
	fprintf(fd, "%s : %d lines\n", fsplit, n);
     }
   fclose(fd);
   vclose(vp);
}



int main(int argc, char **argv)
{
   char *file = NULL;
   int i;
   
   /* if (argc < 2) usage(argv[0]); */
   
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-quiet")) Quiet = 1;
	else if (!strcmp(argv[i], "-l"))
	  {
	     i++;
	     if ((i >= argc) || (1 != sscanf(argv[i], "%d", &Max_Lines))) usage(argv[0]);
	  }
	else if (!strcmp(argv[i], "-f"))
	  {
	     i++;
	     if (i >= argc) usage(argv[0]);
	     file = argv[i];
	  }
	else if (!strcmp(argv[i], "-o"))
	  {
	     i++;
	     if (i >= argc) usage(argv[0]);
	     Split_Fmt = argv[i];
	  }
	else if (!strcmp(argv[i], "-d"))
	  {
	     i++;
	     if (i >= argc) usage(argv[0]);
	     Dsc_File = argv[i];
	  }
	else usage(argv[0]);
     }
   
   if ((file == NULL) || (Split_Fmt == NULL) || (Dsc_File == NULL)) usage(argv[0]);
   split(file);
   return(0);
}

	
