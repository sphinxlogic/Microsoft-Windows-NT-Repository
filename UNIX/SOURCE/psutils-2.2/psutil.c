/* psutil.c
 * AJCD 29/1/91
 * utilities for PS programs
 */

#define LOCAL
#include "psutil.h"

#include <fcntl.h>
#include <string.h>

static char buffer[BUFSIZ];
static long bytes = 0;
static long pagescmt = 0;
static long headerlen = 0;
static int outputpage = 0;
static char pagelabel[BUFSIZ];
static int pageno;
static long pagelength;

/* make a file seekable; trick stolen from Chris Torek's libdvi */

FILE *seekable(fp)
     FILE *fp;
{
   int fd, tf, n, w;
   char *tmpdir, *p;

   fd = fileno(fp);
   if (lseek(fd, 0L, 1) >= 0 && !isatty(fd))
      return (fp);

   if ((tmpdir = getenv("TMPDIR")) == NULL)
      tmpdir = TMPDIR;
   (void) sprintf(buffer, "%s/#%d", tmpdir, getpid());
   if ((tf = open(buffer, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
      return (NULL);
   (void) unlink(buffer);

   while ((n = read(fd, p = buffer, BUFSIZ)) > 0) {
      do {
	 if ((w = write(tf, p, n)) < 0) {
	    (void) close(tf);
	    (void) fclose(fp);
	    return (NULL);
	 }
	 p += w;
      } while ((n -= w) > 0);
   }
   if (n < 0) {
      (void) close(tf);
      (void) fclose(fp);
      return (NULL);
   }

   /* discard the input file, and rewind and open the temporary */
   (void) fclose(fp);
   (void) lseek(tf, 0L, 0);
   if ((fp = fdopen(tf, "r")) == NULL) {
      (void) close(tf);
   }
   return (fp);
}


int fcopy(len)
     long len;
{
   while (len) {
      int n = (len > BUFSIZ) ? BUFSIZ : len;
      if (!(fread(buffer, sizeof(char), n, infile) &&
	    fwrite(buffer, sizeof(char), n, outfile)))
	 return (0);
      len -= n;
      bytes += n;
   }
   return (1);
}

/* build array of pointers to start/end of pages */

scanpages()
{
   register char *comment = buffer+2;
   register int nesting = 0;

   pages = 0;
   fseek(infile, 0L, 0);
   while (fgets(buffer, BUFSIZ, infile) != NULL)
      if (*buffer == '%') {
	 if (buffer[1] == '%') {
	    if (strncmp(comment, "Page:", 5) == 0)
	       pageptr[pages++] = ftell(infile)-strlen(buffer);
	    else if (headerlen == 0 && strncmp(comment, "Pages:", 6) == 0)
	       pagescmt = ftell(infile)-strlen(buffer);
	    else if (headerlen == 0 &&
		     strncmp(comment, "EndComments", 11) == 0)
	       headerlen = ftell(infile);
	    else if (strncmp(comment, "BeginDocument", 13) == 0)
	       nesting++;
	    else if (strncmp(comment, "EndDocument", 11) == 0)
	       nesting--;
	    else if (strncmp(comment, "BeginBinary", 11) == 0)
	       nesting++;
	    else if (strncmp(comment, "EndBinary", 9) == 0)
	       nesting--;
	    else if (nesting == 0 && strncmp(comment, "Trailer", 7) == 0) {
	       fseek(infile, (long)(-strlen(buffer)), 1);
	       break;
	    }
	 } else if (headerlen == 0 && buffer[1] != '!')
	    headerlen = ftell(infile)-strlen(buffer);
      } else if (headerlen == 0)
	 headerlen = ftell(infile)-strlen(buffer);
   pageptr[pages] = ftell(infile);
}

seekpage(p)
     int p;
{
   fseek(infile, pageptr[p], 0);
   if (fgets(buffer, BUFSIZ, infile) != NULL &&
       sscanf(buffer, "%%%%Page: %s %d\n", pagelabel, &pageno) == 2) {
      pagelength = pageptr[p+1]-pageptr[p]-strlen(buffer);
   } else {
      fprintf(stderr, "%s: I/O error seeking page %d\n", prog, p);
      fflush(stderr);
      exit(1);
   }
}

writestring(s)
     char *s;
{
   fputs(s, outfile);
   bytes += strlen(s);
}

writepageheader(label, page)
     char *label;
     int page;
{
   if (verbose) {
      sprintf(buffer, "[%d] ", page);
      message(buffer);
   }
   sprintf(buffer, "%%%%Page: %s %d\n", label, ++outputpage);
   writestring(buffer);
}

writepagebody()
{
   if (!fcopy(pagelength)) {
      fprintf(stderr, "%s: I/O error writing page %d\n", prog, outputpage);
      fflush(stderr);
      exit(1);
   }
}

writepage(p)
     int p;
{
   seekpage(p);
   writepageheader(pagelabel, p+1);
   writepagebody();
}

/* write header: should alter %%Pages: comment */
writeheader(p)
     int p;
{
   long len = headerlen;
   fseek(infile, 0L, 0);
   if (pagescmt && pagescmt < len) {
      if (!fcopy(pagescmt) || fgets(buffer, BUFSIZ, infile) == NULL) {
	 fprintf(stderr, "%s: I/O error in header\n", prog);
	 fflush(stderr);
	 exit(1);
      }
      len -= pagescmt+strlen(buffer);
      sprintf(buffer, "%%%%Pages: %d 0\n", p);
      writestring(buffer);
   }
   if (!fcopy(len)) {
      fprintf(stderr, "%s: I/O error in header\n", prog);
      fflush(stderr);
      exit(1);
   }
}


writeprolog()
{
   if (!fcopy(pageptr[0]-headerlen)) {
      fprintf(stderr, "%s: I/O error in prologue\n", prog);
      fflush(stderr);
      exit(1);
   }
}

/* write trailer */
writetrailer()
{
   fseek(infile, pageptr[pages], 0);
   while (fgets(buffer, BUFSIZ, infile) != NULL) {
      writestring(buffer);
   }
   if (verbose) {
      sprintf(buffer, "Wrote %d pages, %ld bytes\n", outputpage, bytes);
      message(buffer);
   }
}

/* write message to stderr */
message(s)
     char *s;
{
   static int pos = 0;
   char *nl = strchr(s, '\n');
   int len = nl ? (nl-s) : strlen(s);

   if (pos+len > 79 && (pos > 79 || len < 80)) {
      fputc('\n', stderr);
      pos = 0;
   }
   fputs(s, stderr);
   fflush(stderr);
   pos += len;
}


int writeemptypage()
{
   if (verbose)
      message("[*] ");
   sprintf(buffer, "%%%%Page: * %d\nshowpage\n", ++outputpage);
   writestring(buffer);
}

