/* Merges small files back to original using descriptor file. */

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

#ifndef msdos
#define MAX_LEN 65536
#else
#define MAX_LEN 4096
#endif

#include "vfile.h"
int Quiet = 0;

void usage(char *prog)
{
   char *str = "filename";
   
   fprintf(stderr, "%s: Usage: %s [-quiet]\n", prog, str);
   exit(1);
}

void error(char *fmt, char *str)
{
   fprintf(stderr, fmt, str);
   putc('\n', stderr);
   exit(1);
}



int Max_Lines = 2000;


FILE *open_descriptor_file(char *file)
{
   FILE *fp; 
   
#ifdef msdos
   _fmode = O_TEXT;
#endif
   if (NULL == (fp = fopen(file, "r")))
     {
	error("Unable to open descriptor file %s", file);
     }
#ifdef msdos
   _fmode = O_BINARY;
#endif
   return (fp);
}

/* file is name of dsc file */
void merge(char *file)
{
   VFILE *vp;
   unsigned int total, len;
   int fp;
   char *buf, *p;
   char buffer[256], fsplit[256];
   FILE *fd;
   
   fd = open_descriptor_file(file);
   fgets(fsplit, 255, fd);
   
   /* remove newline char */
   p = fsplit;
   while (*p > ' ') p++;  
   *p = 0;
   
   /* next line is format--- ignore it. */
   fgets(buffer, 255, fd);
   
#ifdef VMS
   fp = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, 0, "rat=cr", "rfm=var");
#else
#ifdef unix
   fp = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#else
   _fmode = O_BINARY;
   fp = open(fsplit, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
#endif
#endif
   if (fp < 0)
     {
	error("Error creating %s", fsplit);
     }
   
   while (1)
     {
	if (buf == NULL)
	  {
	     if (vp != NULL) vclose(vp);
	     
	     if (fgets(fsplit, 255, fd) == NULL) break;
	     p = fsplit;
	     while (*p > ' ') p++;
	     *p = 0;
	     
	     if (NULL == (vp = vopen(fsplit, MAX_LEN)))
	       {
		  error("Error opening %s", fsplit);
	       }
	     if (!Quiet) fprintf(stdout, "Processing %s.\n", fsplit);
	  }
	else write(fp, buf, len);
	
	buf = vgets(vp, &len);
     }
   
   if (vp != NULL)
     {
	vclose(vp);
     }
   fclose(fd);
   close(fp);
}


int main(int argc, char **argv)
{
   char *file;
   switch (argc)
     {
      case 3: Quiet = 1;
      case 2: file = argv[1];
	break;
      default: usage(argv[0]);
     }
   merge(file);
   return(0);
}

	
