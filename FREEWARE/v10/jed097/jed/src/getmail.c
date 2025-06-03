/* This routine is written in C to avoid possible race condition
   when moving mail from a maildir. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/file.h>
#include <time.h>
#ifdef SYSV
#include <sys/fcntl.h>
#endif

#include "config.h"
static char *Program_Name;

/* A lock file is just file.lock */

extern void exit_error(char *, int);

void usage()
{
   fprintf(stderr, "%s: usage: %s infile outfile\n", Program_Name, Program_Name);
   exit (-1);
}

char *Lock_Filep;
void make_lock_name(char *file, char *lockfile)
{
   sprintf(lockfile, "%s.lock", file);
   Lock_Filep = lockfile;
}


int lock_mail_file(char *f, int attempts)
{
   struct stat s;
   char buf[80];
   int fd;
      
   while (attempts-- > 0)
     {
	if (stat(f, &s) < 0)
	  {
	     if (errno != ENOENT) exit_error("stat failed.", 0);
	     fd = open(f, O_EXCL | O_CREAT, 0666);
	     if (fd >= 0) 
	       {
		  sprintf(buf, "%d", getpid());
		  write(fd, buf, strlen(buf));
		  close(fd);
		  return (fd);
	       }
	  }
	else if (time((time_t *) NULL) - s.st_ctime > 60) (void) unlink(f);
	
	sleep(1);
     }
   return (-1);
}

void mv_mail(char *from, char *to)
{
   char buf[8192];
   int fdfrom, fdto;
   int n;
   
   if ((fdfrom = open(from, O_RDONLY, 0666)) < 0) exit_error("Unable to open input file.", 1);
   if ((fdto = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) exit_error("Unable to create output file.", 1);
   while ((n = read(fdfrom, buf, sizeof(buf))) > 0)
     {
	if (n != write(fdto, buf, n)) 
	  {	     
	     exit_error("write failed.", 1);
	  }
     }
   
   close(fdto);
   close(fdfrom);
   if (unlink(from))
     {
	if ((fdfrom = open(from, O_WRONLY | O_CREAT | O_TRUNC, 0600)) >= 0) close(fdfrom);
     }
}


void unlock_mail_file(char *f)
{
   unlink(f);
}

void exit_error(char *s, int l)
{
   fprintf(stderr, "%s: %s\n", Program_Name, s);
   if (l) unlock_mail_file(Lock_Filep);
   exit(-1);
}


   
int main(int argc, char **argv)
{
   char *in, *out;
   char lockfile[256];
   
   Program_Name = argv[0];
   if (argc != 3) usage();
   in = argv[1];
   out = argv[2];
   
   make_lock_name(in, lockfile);
   
   if (lock_mail_file (lockfile, 60) < 0)
     {
	exit_error("Unable to lock file.", 0);
     }
   
   mv_mail(in, out);
   unlock_mail_file(lockfile);
   return(0);
}

   
   
