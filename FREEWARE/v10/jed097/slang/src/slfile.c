/* file intrinsics for S-Lang */
/* 
 * Copyright (c) 1992, 1994 John E. Davis 
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.   Permission is not granted to modify this
 * software for any purpose without written agreement from John E. Davis.
 *
 * IN NO EVENT SHALL JOHN E. DAVIS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JOHN E. DAVIS
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JOHN E. DAVIS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
 * BASIS, AND JOHN E. DAVIS HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */


#include <stdio.h>
#include <string.h>

#if 0
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#endif

#include "slang.h"
#include "_slang.h"
#include "slfile.h"

SL_File_Table_Type SL_File_Table[SL_MAX_FILES];


#if 0
static int tcp_open (char *service, char *host)
{
   struct servent *sp;
   struct hostent *hp;
   struct sockaddr_in Read_Socket;
   int s;
   
   if (NULL == (sp = getservbyname (service, "tcp")))
     {
	SLang_doerror ("Unknown service");
	return -1;
     }
   
   if (NULL == (hp = gethostbyname(host)))
     {
	SLang_doerror ("Unknown host");
	return -2;
     }
   
   bzero ((char *) &Read_Socket, sizeof (Read_Socket));
   bcopy (hp->h_addr, (char *) &Read_Socket.sin_addr, hp->h_length);
   Read_Socket.sin_family = hp->h_addrtype;
   Read_Socket.sin_port = sp->s_port;
   
   if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0)
     {
	SLang_doerror ("Unable to create socket");
	return -3;
     }
   
   if (connect (s, (char *) &Read_Socket, sizeof (Read_Socket)) < 0)
     {
	SLang_doerror ("Unable to connect.");
	return -4;
     }
   
   return s;
}

#endif



SL_File_Table_Type *get_file_table_entry(void)
{
   SL_File_Table_Type *t = SL_File_Table, *tmax;
   
   tmax = t + SL_MAX_FILES;
   while (t < tmax)
     {
	if (t->fd == -1) return t;
	t++;
     }
   
   return NULL;
}

static char *remake_string(char *s, int len)
{
   len++;			       /* for null terminator */
   if (s == NULL)
     {
	s = (char *) SLMALLOC(len);
     }
   else s = (char *) SLREALLOC(s, len);
   
   if (s == NULL) SLang_Error = SL_MALLOC_ERROR;
   return (s);
}


unsigned int file_process_flags(char *mode)
{
   char ch;
   unsigned int flags = 0;
   
   while ((ch = *mode++) != 0)
     {
	switch (ch)
	  {
	   case 'r': flags |= SL_READ; 
	     break;
	   case 'w':
	   case 'a':
	   case 'A':
	     flags |= SL_WRITE; 
	     break;
	   case '+': flags |= SL_WRITE | SL_READ; 
	     break;
	   case 'b': flags |= SL_BINARY; 
	     break;
	     
	   default:
	     return(0);
	  }
     }
   return (flags);
}

/* returns -1 upon failure or returns a handle to file */
int SLfopen(char *file, char *mode)
{
   FILE *fp;
   SL_File_Table_Type *t;
   unsigned int flags;
   
   if ((t = get_file_table_entry()) == NULL) return (-1);
   if (0 == (flags = file_process_flags(mode))) return (-1);
   
   if ((fp = fopen(file, mode)) == NULL) return (-1);
   t->fp = fp;
   t->fd = fileno(fp);
   t->flags = flags;
#ifdef HAS_SUBPROCESSES
   t->pid = -1;
#endif
   return ((int) (t - SL_File_Table));
}

#if 0
int SLtcp_open(char *service, char *host)
{
   SL_File_Table_Type *t;
   int fd;
   
   if ((t = get_file_table_entry()) == NULL) return (-1);
   
   if ((fd = tcp_open (service, host)) < 0) return -1;
   t->fp = fdopen (fd, "r+");
   t->fd = fd;
   t->flags = SL_SOCKET | SL_READ | SL_WRITE;
   return ((int) (t - SL_File_Table));
}
#endif

/* returns pointer to file entry if it is open and consistent with 
   flags.  Returns NULL otherwise */
static SL_File_Table_Type *check_handle(int *h, unsigned int flags)
{
   int n = *h;
   SL_File_Table_Type *t;
   
   if ((n < 0) || (n >= SL_MAX_FILES)) return(NULL);
   t = SL_File_Table + n;
   if (t->fd == -1) return (NULL);
   if (flags & t->flags) return (t);
   return NULL;
}

   

/* returns 0 upon failure or 1 if successful */
int SLfclose(int *handle)
{
   int ret = 0;
   
   SL_File_Table_Type *t = check_handle(handle, 0xFFFF);
   
   if (t == NULL) return 0;
   if (t->fp != NULL)
     {
	if (EOF != fclose (t->fp)) ret = 1;
     }
#ifdef USE_SUBPROCESSES
   else if (t->fd != -1)
     {
	if (close (t->fd) == 0) ret = 1; 
     }
#endif
   t->fp = NULL;  t->fd = -1;
   return (ret);
}

/* returns number of characters read and pushes the string to the stack.  
   If it fails, it returns -1 */
int SLfgets(int *handle)
{
   char buf[256];
   char *s = NULL, *s1;
   register char *b = buf, *bmax = b + 256;
   register int ch;
   int len = 0, dlen;
   SL_File_Table_Type *t;
   register FILE *fp;
   

   if (NULL == (t = check_handle(handle, SL_READ))) return (-1);
   fp = t->fp;
   while (EOF != (ch = getc(fp)))
     {
	if (b == bmax)
	  {
	     if (NULL == (s1 = remake_string(s, len + 256)))
	       {
		  if (s != NULL) SLFREE(s);
		  return(-1);
	       }
	     s = s1;
	     b = buf;
	     strncpy(s + len, b, 256);
	     len += 256;
	  }
	*b++ = (char) ch;
	if (ch == '\n') break;
     }
   
   dlen = (int) (b - buf);
   if ((dlen == 0) && (s == NULL)) return(0);
   
   
   if (NULL == (s1 = remake_string(s, len + dlen)))
     {
	if (s != NULL) SLFREE(s);
	return(-1);
     }
   
   strncpy(s1 + len, buf, dlen);
   len += dlen;
   *(s1 + len) = 0;   /* null terminate since strncpy may not have */
   SLang_push_malloced_string(s1);
   return(len);
}

int SLfputs(char *s, int *handle)
{
   SL_File_Table_Type *t;
   

   if (NULL == (t = check_handle(handle, SL_WRITE))) return (-1);
   if (EOF == fputs(s, t->fp)) return (0);
   return (1);
}
#if 0
int SLinput_pending(int *handle, int *tsecs)
{
   SL_File_Table_Type *t;
   struct timeval wait;
   fd_set readfd, writefd, exceptfd;
   long usecs, secs;
   int fd, ret;

   secs = *tsecs / 10;
   usecs = (*tsecs % 10) * 100000;
   wait.tv_sec = secs;
   wait.tv_usec = usecs;
   
   if (NULL == (t = check_handle(handle, SL_READ))) return (0);
   fd = t->fd;
   if (t->fp->_cnt) return 1;
   
   
   FD_SET(fd, &readfd);   /* readfd = 1 << fd; */
   FD_ZERO(&writefd);
   FD_ZERO(&exceptfd);

   ret = select(fd + 1, &readfd, &writefd, &exceptfd, &wait);
   return(ret);

}
#endif

int SLfflush(int *handle)
{
   SL_File_Table_Type *t;
   

   if (NULL == (t = check_handle(handle, SL_WRITE))) return (-1);
   if (EOF == fflush(t->fp)) return (0);
   return (1);
}




static int slfile_stdin = 0;
static int slfile_stdout = 1;
static int slfile_stderr = 2;

static SLang_Name_Type SLFiles_Name_Table[] = 
{
   MAKE_INTRINSIC(".fopen", SLfopen, INT_TYPE, 2),
   /* Usage: int fopen(String file, String mode);
      Open 'file' in 'mode' returning handle to file.
      Here mode is one of:  "r", "w", "a", "r+", "w+", "a+", "rb", ...
          where 'r' means read, 'w' means write, '+' means read and write,
	  'b' means binary, etc...
      Returns -1 if file could not be opened.  Any other value is a handle
      to the file.  This handle must be used in other file operations. */
   MAKE_INTRINSIC(".fclose", SLfclose, INT_TYPE, 1),
   /* Usage: int fclose(int handle);
      Closes file associated with 'handle'.  
      returns: 0 if file could not be closed or buffers could not be flushed.
               1 upon success.
	       -1 if handle is not associated with an open stream. */
   MAKE_INTRINSIC(".fgets", SLfgets, INT_TYPE, 1),
   /*  Usage:  [String] int fgets(int handle);
       Reads a line from the open file specified by handle.  
       Returns: -1 if handle not associated with an open file.
                 0 if at end of file.
		 Otherwise, returns the number of characters read.  The 
		   second value on the stack is the string of characters.
		   
	  Example:
	    variable buf, fp, n;
	    fp = fopen("myfile", "r");
	    if (fp < 0) error("File not open!");
	    n = fgets(fp);
	    if (n == 0) print("End of File!"); 
	    else if (n == -1) print("fgets failed!");
	    else buf = ();  % characters left on the stack.
	    */
	     
   MAKE_INTRINSIC(".fflush", SLfflush, INT_TYPE, 1),
   /* Usage: int fflush(int handle);
      flushes an output stream.
      returns: -1 if handle is not associated with an open stream
                0 if fflush fails (lack of disk space, etc...)
		1 success */
    
   MAKE_INTRINSIC(".fputs", SLfputs, INT_TYPE, 2),
   /* Usage: int fputs(string buf, int handle);
      Writes a null terminated string (buf) to the open output stream (handle).
      returns: -1 if handle is not associated with an open output stream.
                1 indicates success. */
   
#ifdef HAS_SUBPROCESSES
   MAKE_INTRINSIC(".create_process", SLcreate_child_process, INT_TYPE, 1),
   /*Prototype: (handle, pid) = create_process (String name);
    * This function returns the pid of the created process as well as 
    * an open file handle for both read and write.  It returns -1 for both
    * parameters upon failure. */
#endif
   
#if 0
   MAKE_INTRINSIC(".tcp_open", SLtcp_open, INT_TYPE, 2),
   /*Prototype: Integer tcp_open (String service, String host);
    */
   MAKE_INTRINSIC(".unix_select", SLinput_pending, INT_TYPE, 2),
   /*Prototype : Integer unix_select(Integer handle, integer secs);
    */
#endif
   MAKE_VARIABLE(".stdin", &slfile_stdin, INT_TYPE, 1),
   /* Handle to stdin */
   MAKE_VARIABLE(".stdout", &slfile_stdout, INT_TYPE, 1),
   /* Handle to stdout */
   MAKE_VARIABLE(".stderr", &slfile_stderr, INT_TYPE, 1),
   /* Handle to stderr */
   SLANG_END_TABLE
};


int init_SLfiles()
{
   int i;
   SL_File_Table_Type *s = SL_File_Table;
   
   for (i = 3; i < SL_MAX_FILES; i++) s[i].fd = -1;
   
   s->fd = fileno(stdin);  s->flags = SL_READ; s->fp = stdin; s++;
   s->fd = fileno(stdout); s->flags = SL_WRITE; s->fp = stdout; s++;
   s->fd = fileno(stderr); s->flags = SL_READ | SL_WRITE; s->fp = stderr;
   
   return SLang_add_table(SLFiles_Name_Table, "_Files");
}

   
   
