/*****************************************************************************
 * F.Macrides 18-Jun-1994  Removed comment in FIOsystem() about reversed
 *			    return value on VMS (no longer applies).
 ****************************************************************************/

/********************************************************************
 Changes from Alan Coopersmith's patches:
     -initialize temp->filename in FIOnew()
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.20
 * 1994/05/25 20:57:20
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/fileio.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 92, 93, 94 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: fileio.c
 * Socket/file input output routines.
 *********************************************************************
 * Revision History:
 * fileio.c,v
 * Revision 3.20  1994/05/25  20:57:20  lindner
 * Fix for piped play commands
 *
 * Revision 3.19  1994/04/25  03:44:42  lindner
 * Don't block empty FIOsystem() command on VMS.  That's what's used for
 * the '!' or '$' commands, and they've already be checked for permission
 * in Gopher.c.
 *
 * Reversed DCLsystem() return values in CURCurses.c, and eliminated the
 * reversing kludge in FIOsystem().
 *
 * Fixed break of open() for Alpha.  (all From F.Macrides)
 *
 * Revision 3.18  1994/04/25  03:37:00  lindner
 * Modifications for Debug() and mismatched NULL arguments, added Debugmsg
 *
 * Revision 3.17  1994/04/25  02:18:46  lindner
 * Fix for gcc-Wall
 *
 * Revision 3.16  1994/04/14  18:14:49  lindner
 * Fix for closing null files
 *
 * Revision 3.15  1994/04/14  15:44:55  lindner
 * Remove duplicate variable
 *
 * Revision 3.14  1994/04/08  21:07:34  lindner
 * Put back a variable
 *
 * Revision 3.13  1994/04/08  20:05:57  lindner
 * gcc -Wall fixes
 *
 * Revision 3.12  1994/04/08  19:15:47  lindner
 * Fix for old union wait stuff
 *
 * Revision 3.11  1994/04/07  22:51:24  lindner
 * Fix for typecast
 *
 * Revision 3.10  1994/04/01  04:43:31  lindner
 * Fixes for memory leak in argv, and exit status
 *
 * Revision 3.9  1994/03/30  21:37:48  lindner
 * Fix for AIX that doesn't #define unix..
 *
 * Revision 3.8  1994/03/11  00:10:52  lindner
 * Fix for FIOopenUFS()
 *
 * Revision 3.7  1994/03/09  16:57:56  lindner
 * Fix for other vms system() call
 *
 * Revision 3.6  1994/03/09  02:11:49  lindner
 * Use DCLsystem for VMS
 *
 * Revision 3.5  1994/03/08  17:23:17  lindner
 * Fix for return vals
 *
 * Revision 3.4  1994/03/08  06:17:15  lindner
 * Nuke compiler warnings
 *
 * Revision 3.3  1994/03/08  03:22:39  lindner
 * Additions for process management
 *
 * Revision 3.2  1994/03/04  17:43:36  lindner
 * Fix for weird error
 *
 * Revision 3.1  1994/02/20  16:20:48  lindner
 * New object based versions of buffered io routines
 *
 *
 *********************************************************************/

#include "fileio.h"
#include "Malloc.h"

#include <errno.h>
#include "Debug.h"
#include "Wait.h"
#include "String.h"
#include "Stdlib.h"

#ifdef VMS
#  include <stat.h>
#else
#  include <sys/stat.h>
#endif

/*
 * Cache of used fios 
 */

static FileIO* FIOusedfios[FIOMAXOFILES];
static int  FIOnumusedfios = -1;

/*
 * Pop a recently used item.
 */

static FileIO *FIOpopUsedfio()
{
     if (FIOnumusedfios > 0) {
	  FIOnumusedfios--;
	  return(FIOusedfios[FIOnumusedfios]);
     } else
	  return(NULL);
}

/*
 * Push an item on our recently used stack.
 */
 
static boolean FIOpushUsedfio(oldfio)
  FileIO *oldfio;
{
     if (FIOnumusedfios < FIOMAXOFILES) {
	  FIOusedfios[FIOnumusedfios] = oldfio;
	  FIOnumusedfios++;
	  return(0);
     } else
	  return(-1);
}

static FileIO *
FIOnew()
{
     FileIO *temp;

     if (FIOnumusedfios == -1) {
	  int i;
	  /* Initialize the usedfios struct the first time through */
	  for (i=0; i < FIOMAXOFILES; i++) {
	       FIOusedfios[i] = NULL;
	  }
	  FIOnumusedfios=0;
     }
     
     temp = FIOpopUsedfio();
     if (temp == NULL) {
	  temp = (FileIO *) malloc(sizeof(FileIO));
	  temp->filename = STRnew();
     } else {
	  STRinit(temp->filename);
     }

     
     temp->issocket = -1;
     temp->pid = -1;
     temp->fd = (pid_t) -1;
     temp->buf = NULL;

     temp->bufindex = 0;
     temp->bufdatasize = -1;


     return(temp);
}


/*
 * Should only be called by FIOclose* functions really..
 */

void
FIOdestroy(fio)
  FileIO *fio;
{
     if (FIOpushUsedfio(fio)) {
	  /** No space in cache. **/
	  free(fio);
     }
}

/*
 * Open a file, initialize data structures.
 */

FileIO *
FIOopenUFS(fname, flags, mode)
  char   *fname;
  int    flags;
  int    mode;
{
     int    fd;
     FileIO *fio;

     /* Okay, try and open up the file */
     fd = open(fname, flags, mode );

     if (fd < 0)
	  return(NULL); /* Couldn't open file */

     fio = FIOnew();
     
     FIOsetSocket(fio, FALSE);
     FIOsetFilename(fio, fname);
     FIOsetfd(fio,fd);
     
     return(fio);
}


/*
 * Start FIO routines on an already open file descriptor
 */

FileIO*
FIOopenfd(fd, issocket)
  int     fd;
  boolean issocket;
{
     FileIO *fio;

     fio = FIOnew();

     FIOsetfd(fio, fd);
     FIOsetSocket(fio, issocket);
     return(fio);
}

FileIO*
FIOopenProcess(prog, args, rw)
  char   *prog;
  char   **args;
  char   *rw;
{
     int    pfd[2];
     int    pid;
     FileIO *fio;

     if (prog == NULL) 
	  return(NULL);

     fio = FIOnew();

     if (pipe(pfd) < 0)
	  return(NULL);


     switch (pid = vfork()) {
     case -1:			/* Error */
	  (void) close(pfd[0]);
	  (void) close(pfd[1]);
	  break;
     case 0:			/* Child */
	  if (rw == NULL || *rw == '\0') {
	       /** mimic system(), don't do anything **/
	       (void) close(pfd[0]);
	       (void) close(pfd[1]);
	  }
	  else if (*rw == 'r') {
	       if (pfd[1] != 1) {
		    dup2(pfd[1], 1);
		    (void) close(pfd[1]);
	       }
	       (void) close(pfd[0]);
	  } else {
	       if (pfd[0] != 0) {
		    dup2(pfd[0], 0);
		    (void) close(pfd[0]);
	       }
	       (void) close(pfd[1]);
	  }
#ifdef VMS
	  execv(prog, args);
#else
	  /** Unix **/

	  if (*prog == '/')
	       execv(prog, args);
	  else
	       execvp(prog, args); /* search the path for the command */
#endif

	  _exit(1);
     }

     /* parent.. */
     if (rw == NULL || *rw == '\0') {
	  /** Don't do anything, mimic system() **/
	  FIOsetfd(fio, -1);
	  (void) close(pfd[0]);
	  (void) close(pfd[1]);
     } else if (*rw == 'r') {
	  FIOsetfd(fio, pfd[0]);
	  (void) close(pfd[1]);
     } else {
	  FIOsetfd(fio, pfd[1]);
	  close(pfd[0]);
     }
     FIOsetPid(fio, pid);
     return(fio);
}


/*
 * Close a file/socket/process
 */

int
FIOclose(fio)
  FileIO *fio;
{
     int result;

     if (fio == NULL)
	  return(0);

     if (FIOgetPid(fio) >= 0) {
	  
	  close(FIOgetfd(fio));
	  result = FIOwaitpid(fio);
	  FIOdestroy(fio);
	  
	  return(result);
     }

     result = FIOisSocket(fio) ? socket_close(FIOgetfd(fio)) :
	  close(FIOgetfd(fio));

     FIOdestroy(fio);

     return(result);
}

/*
 * A portable waitpid fcn that returns the exit value of the child process
 *
 * Should be better about stopped and signaled processes....
 */

int
FIOwaitpid(fio)
  FileIO *fio;
{
     Portawait   status;
     pid_t       result;

     result = waitpid(FIOgetPid(fio), &status, 0);
     
     return(Gwaitstatus(status) & 0xf);
}

/*
 * write n bytes to an fd.. 
 *
 * returns -1 on error.
 */

int
FIOwriten(fio, ptr, nbytes)
  FileIO *fio;
  char   *ptr;
  int    nbytes;
{
     int nleft, nwritten;
     int fd = FIOgetfd(fio);

     nleft = nbytes;
     while(nleft > 0) {
	  nwritten = FIOisSocket(fio) ? socket_write(fd, ptr, nleft) :
	       write(fd, ptr, nleft);

	  if (nwritten <= 0)
	       return(nwritten);	/* error */
	  
	  nleft	-= nwritten;
	  ptr	+= nwritten;
     }
     return(nbytes - nleft);
}

/*
 * write a string to a FileDescriptor, eventually buffer outgoing input
 *
 * If write fails a -1 is returned. Otherwise zero is returned.
 */
   
int
FIOwritestring(fio, str)
  FileIO *fio;
  char   *str;
{
     int length;

     Debug("writing: %s\n",str);

     if (str == NULL)
	  return(0);

     length = strlen(str);
     if (FIOwriten(fio, str, length) != length) {
	  Debugmsg("writestring: writen failed\n");
	  return(-1);
     }
     else
	  return(0);
}

/*
 * Read through a buffer, more efficient for character at a time
 * processing.  Not so good for block binary transfers
 */

int
FIOreadbuf(fio, newbuf, newbuflen)
  FileIO *fio;
  char   *newbuf;
  int     newbuflen;
{
     int len;
     int fd = FIOgetfd(fio);
     char *recvbuf;
     int  bytesread = 0;

     if (FIOgetBufIndex(fio) == 0) {
	  if (fio->buf == NULL)
	       fio->buf = (char *) malloc(sizeof(char) * FIOBUFSIZE);
	  
	  len = FIOisSocket(fio) ? socket_read(fd, fio->buf, FIOBUFSIZE) :
	       read(fd, fio->buf, FIOBUFSIZE);
	  FIOsetBufDsize(fio, len);
	  FIOsetBufIndex(fio, 0);
	  
#if defined(FIO_NOMULTIEOF)
	  if (len < 0 && errno == EPIPE)
	       return(0);
#endif
	  if (len == 0)
	       return(0);

     }
     
     recvbuf = fio->buf;

     while (newbuflen--) {
	  *newbuf++ = recvbuf[FIOgetBufIndex(fio)++];
	  bytesread++;
	  
	  if (FIOgetBufIndex(fio) == FIOgetBufDsize(fio) && newbuflen != 0) {
	       /** Read another buffer **/
	       len = FIOisSocket(fio) ? socket_read(fd, fio->buf, FIOBUFSIZE) :
		    read(fd, fio->buf, FIOBUFSIZE);

	       if (len == 0) {
		    FIOsetBufIndex(fio,0);
		    return(bytesread); /** EOF **/
	       }
	       if (len < 0)
		    return(len);       /** Error **/

	       FIOsetBufDsize(fio, len);
	       FIOsetBufIndex(fio, 0);
	  } else if (FIOgetBufIndex(fio) >= FIOgetBufDsize(fio))
	       /* Read a new buffer next time through */
	       FIOsetBufIndex(fio, 0);
     }
     return(bytesread);
     
}


/* Read 'n' bytes from a descriptor, non buffered direct into the storage. */
int
FIOreadn(fio, ptr, nbytes)
  FileIO *fio;
  char   *ptr;
  int    nbytes;
{
     int nleft, nread;
     int fd = FIOgetfd(fio);
     
     nleft = nbytes;
     while (nleft > 0) {
	  nread = FIOisSocket(fio) ? socket_read(fd, ptr, nleft) :
	       read(fd, ptr, nleft);
#if defined(FIO_NOMULTIEOF)
          if (nread < 0 && errno == EPIPE)
	       break;
#endif

	  if (nread < 0)
	       return(nread);  /* error, return < 0 */
	  else if (nread == 0) /* EOF */
	       break;

	  nleft -= nread;
	  ptr   += nread;
     }
     return(nbytes - nleft);

}


/*
 * Read a line from the file/socket, Read the line one byte at a time,
 * looking for the newline.  We store the newline in the buffer,
 * then follow it with a null (the same as fgets(3)).
 * We return the number of characters up to, but not including,
 * the null (the same as strlen(3))
 */

int
FIOreadline(fio, ptr, maxlen)
  FileIO *fio;
  char   *ptr;
  int    maxlen;
{
     int bytesread;
     int rc;
     char c;

     for (bytesread=1; bytesread < maxlen; bytesread++) {
	  if ( (rc = FIOreadbuf(fio, &c, 1)) == 1) {
	       *ptr++ = c;
	       if (c == '\n')
		    break;
	  }
	  else if (rc == 0) {
	       if (bytesread == 1)
		    return(0);	/* EOF, no data read */
	       else
		    break;		/* EOF, some data was read */
	  }
	  else
	       return(-1);		/* error */
     }
     
     *ptr = 0; 				/* Tack a NULL on the end */
     Debug("readline: %s\n", (ptr-bytesread));

     return(bytesread);
}


/*
 * This does the same as readline, except that all newlines and 
 * carriage returns are automatically zapped.
 *
 * More efficient than doing a readline and a ZapCRLF
 */

int 
FIOreadlinezap(fio, ptr, maxlen)
  FileIO *fio;
  char   *ptr;
  int    maxlen;
{
     int len;

     len = FIOreadtoken(fio, ptr, maxlen, '\n');
     ptr += len;
     ptr --;
     
     if (*ptr == '\r') {
	  ptr[len] = '\0';
	  len--;
     }
     return(len);
}  


/*
 * Read a line from the file/socket, Read the line one byte at a time,
 * looking for the token.  We nuke the token from the returned string.
 * We return the number of characters up to, but not including,
 * the null (the same as strlen(3))
 */

int 
FIOreadtoken(fio, ptr, maxlen, zechar)
  FileIO *fio;
  char	 *ptr;
  int 	 maxlen;
  char   zechar;
{
     int bytesread;
     int rc;
     char c;

     for (bytesread=1; bytesread < maxlen; bytesread++) {
	  if (FIOgetBufIndex(fio) == 0)
	       rc = FIOreadbuf(fio, &c, 1);
	  else {
	       rc = 1;
	       c = fio->buf[FIOgetBufIndex(fio)++];
	       if (FIOgetBufIndex(fio) >= FIOgetBufDsize(fio))
		    FIOsetBufIndex(fio, 0);
	  }

	  if (rc == 1) {
	       *ptr++ = c;
	       if (c == zechar) {
		    *(ptr - 1) = '\0';
		    break;
	       }
	  }
	  else if (rc == 0) {
	       if (bytesread == 1)
		    return(0);	/* EOF, no data read */
	       else
		    break;		/* EOF, some data was read */
	  }
	  else
	       return(-1);		/* error */
     }
     
     *ptr = 0; 				/* Tack a NULL on the end */
     Debug("readtoken: %s\n", (ptr-bytesread));
     return(bytesread);
}

int
FIOexecv(prog, args)
  char *prog;
  char **args;
{
     FileIO *fio;
     int    result;

#ifdef VMS
     int     i = 0;
     char    buffer[1024];

     /* DCL hog heaven */
     strcpy(buffer, prog);
     strcat(buffer, " ");

     while (i++) {
	  if (args[i] == NULL)
	       break;

	  strcat(buffer, args[i]);
	  strcat(buffer, " ");
     }
     result = DCLsystem(buffer);

#else
     fio = FIOopenProcess(prog, args, NULL);
     result = FIOclose(fio);
#endif /* VMS */

     return(result);
}


/*
 * Do the minimal shell/dcl processing
 */

char **
FIOgetargv(cmd)
  char *cmd;
{
     int           inquote = 0;
     int           insquote = 0;
     int           i;
     static char  *argv[128];		/* Sufficient for now.. */
     int           argc = 0;
     char          buf[256];
     static char   savecmd[256];        /* Maximum cmd length is 256 */
     char         *cp = buf;

     if (cmd == NULL)
	  return(NULL);

     for (i=0; cmd[i] != '\0'; i++) {

	  switch (cmd[i]) {

	  case ' ': case '\t':
	       /* Separators */
	       if (insquote || inquote) {
		    *cp = cmd[i]; cp++;
		    break;
	       } else {
		    *cp = '\0';
		    argv[argc++] = strdup(buf);
		    cp = buf;

		    /** Get rid of any other whitespace **/
		    while (cmd[i+1] == ' ' || cmd[i+1] == '\t')
			 i++;
	       }
	       break;

	  case '"':
	       if (!insquote)
		    inquote = 1-inquote;
	       break;
	       
	  case '\'':
	       if (!inquote)
		    insquote = 1-insquote;
	       break;

	  case '\\':
	       /* Quote next character if not in quotes */
	       if (insquote || inquote) {
		    *cp = cmd[i]; cp++;
	       } else {
		    *cp = cmd[i+1]; cp++; i++;
	       }

	       break;

	  default:
	       *cp = cmd[i]; cp++;
	       break;
	  }
     }
     if (buf != cp) {
	  *cp = '\0';
	  argv[argc++] = strdup(buf);
     }
     argv[argc] = NULL;
     
     return(argv);
}


/*  
 *  An emulation of the system() call without the shell
 *  returns the exit status of the child
 */

int
FIOsystem(cmd)
  char *cmd;
{
     char **argv, i;
     int    result;

#ifdef VMS
     return(DCLsystem(cmd));
#else

     if (cmd == NULL || *cmd == '\0')
	  return(-1);

     argv = FIOgetargv(cmd);

     result = FIOexecv(argv[0], argv);
     
     for (i=0; argv[i] != NULL; i++)
	  free(argv[i]);

     return(result);
#endif
}



/*
 * Similar to popen...
 */

FileIO
*FIOopenCmdline(cmd, rw)
  char *cmd;
  char *rw;
{
     char   **argv;
     FileIO *fio;

     if (cmd == NULL)
	  return(NULL);

     if (*cmd == '|')
	  cmd++;

     argv = FIOgetargv(cmd);
     
     fio = FIOopenProcess(argv[0], argv, rw);
     
     return(fio);
}

