/*****************************************************************************
 * 21-Mar-1994 F.Macrides - More continuation line handling mods for aFTP
 *			     servers which play fast and loose with them.
 * 12-Mar-1994 F.Macrides - Made continuation line handling in QuitClose()
 *			     fully RFC compliant.
 *			    Added more binary extensions.
 * 21-Feb-1994 F.Macrides - Added code to QuitClose() for dealing with any
 *			     continuation lines, Joel's code for Solaris.
 * 19-Feb-1994 F.Macrides - Fixed QuitClose along lines suggested by Joel M.
 *			     Snyder <Joel.Snyder@SEN.CA.GOV>.
 * 04-Jan-1994 F.Macrides - Yet more binary extensions.
 * 01-Dec-1993 F.Macrides - Yet more binary extensions.
 * 30-Nov-1993 F.Macrides - Yet more binary extensions.
 * 29-Nov-1993 F.Macrides - Added more binary extensions.
 * 19-Nov-1993 F.Macrides - Fixed bug which trimmed any numbers, not just
 *			     version numbers, from the ends of VMS file names.
 * 11-Nov-1993 F.Macrides - Eliminated temporary (scratch) files.  Server does
 *			     direct handshaking between the aFTP host and the
 *			     the client.
 *			    Fixed memory leaks.
 *			    Deleted dead code.
 * 10-Nov-1993 F.Macrides - Use client's host name, not server's, in the
 *			     password sent to the aFTP host.
 * 13-Oct-1993 F.Macrides - Added M.MacArthur's GopherPort->GDCgetPort(config)
 			     changes, and additional binary extensions for
			     VMS.
 * 22-Aug-1993 F.Macrides - Code coordinated with v2.0, pl5.
 *			    Fixed to work reliably with MACOS.
 * 05-Aug-1993 F.Macrides - Improved error handling.
 * 04-Aug-1993 F.Macrides - Code coordinated with v2.0, pl1.
 * 02-Aug-1993 F.Macrides - Modified to accommodate code changes in the
 * 			     gopher+ v2.0 Production release.
 * 27-Jul-1993 F.Macrides - Initial hack for port of gopher+ server to VMS
/********************************************************************
 * lindner
 * 3.10
 * 1993/08/11 14:41:48
 * /home/mudhoney/GopherSrc/CVS/gopher+/gopherd/ftp.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: ftp.c
 * Routines to translate gopher protocol to ftp protocol.
 *********************************************************************
 * Revision History:
 * ftp.c,v
 * Revision 3.10  1993/08/11  14:41:48  lindner
 * Fix for error logging and uninitialized gs
 *
 * Revision 3.9  1993/08/11  14:08:39  lindner
 * Fix for hanging ftp gateway connections
 *
 * Revision 3.8  1993/08/03  20:43:54  lindner
 * Fix for sites that have @ -> for symbolic links
 *
 * Revision 3.7  1993/08/03  06:40:11  lindner
 * none
 *
 * Revision 3.6  1993/08/03  06:14:12  lindner
 * Fix for extra slashes
 *
 * Revision 3.5  1993/07/30  19:21:03  lindner
 * Removed debug stuff, fix for extra slashes
 *
 * Revision 3.4  1993/07/30  18:38:59  lindner
 * Move 3.3.1 to main trunk
 *
 * Revision 3.3.1.7  1993/07/29  21:42:21  lindner
 * Fixes for Symbolic links, plus removed excess variables
 *
 * Revision 3.3.1.6  1993/07/27  05:27:42  lindner
 * Mondo Debug overhaul from Mitra
 *
 * Revision 3.3.1.5  1993/07/26  17:18:55  lindner
 * Removed extraneous abort printf
 *
 * Revision 3.3.1.4  1993/07/26  15:34:21  lindner
 * Use tmpnam() and bzero(), plus ugly fixes..
 *
 * Revision 3.3.1.3  1993/07/23  03:12:29  lindner
 * Small fix for getreply, reformatting..
 *
 * Revision 3.3.1.2  1993/07/07  19:39:48  lindner
 * Much prettification, unproto-ed, and use Sockets.c routines
 *
 * Revision 3.3.1.1  1993/06/22  20:53:21  lindner
 * Bob's ftp stuff
 *
 * Revision 3.3  1993/04/15  04:48:09  lindner
 * Debug code from Mitra
 *
 * Revision 3.2  1993/03/24  20:15:06  lindner
 * FTP gateway now gets filetypes from gopherd.conf
 *
 * Revision 3.1.1.1  1993/02/11  18:02:51  lindner
 * Gopher+1.2beta release
 *
 * Revision 1.2.1 1993/04/02 12:00:01  alberti
 * Robert Alberti, alberti@boombox.micro.umn.edu, 02. Apr 93
 * Extensively rewritten tell different server types apart
 *
 * Andi Karrer, karrer@bernina.ethz.ch, 21. Feb 93
 * Added support for VMS FTP servers that do not understand the FTP PASV
 * command (Wollongong, Multinet). Also present these servers output
 * in a Unix fashion.
 * Also works with Ultrix 4.2 FTP servers whose PASV command is broken.
 *
 * Revision 1.2  1993/01/11  23:18:09  lindner
 * Changed password to be the host of the remote client, not the gateway.
 *
 * Revision 1.1  1992/12/10  23:13:27  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


/* -------------------------------------------------
 *     g2fd.c          Gopher to FTP gateway daemon.
 *     Version 0.3 Hacked up: April 1992.  Farhad Anklesaria.
 *     Based on a Perl story by John Ladwig.
 *     Based on a Perl story by Farhad Anklesaria.
 *
 *      Modified by Greg Smith, Bucknell University, 24 Nov 1992
 *      to handle multiline status replies.
 *
 ---------------------------------------------------- */

#include "gopherd.h"
#include <signal.h>

#include <stdio.h>
#include "ext.h"
#include "Debug.h"

#define GETFILE 0
#define GETBINARY 1
#define GETDIR 2

#define FTPISUNKNOWN 0
#define FTPISPASV   1
#define FTPISVMS    3

#define FTPISNOVELL 61
#define FTPISPORT   7
#define FTPISUNIX   8

#define DUMP 1
#define KEEP 0

#define SLEN 256

static int sockfd;        /* I hate globals, but these are ubiquitous comm */
static int ftp_control;   /* channels and not about to change, so...       */

static short FtpReturnDir; /* TRUE if returning a dir object from AbortF() */

/*** Some forward declarations ***/
int     getreply();
int	FtpConnection();
int	FtpLogin();
void 	QuitClose();
boolean NotText();
int     AbortF();
boolean IsBinaryType();
int	Vaxinate();
int	GopherType();
int	GopherFile();
int	ParseUnixList();
char    *WalkDirs();
int	PortxFerDir();
int	PortxFerFile();
int	PORTQuery();
int	FtpPreAnalyze();
int	AnalyzeType();
char 	*ParseQuery();


/*********** Common Utility for I/O ***************/

int
getreply(int level, char *inputline, int maxlen)
{
     int i, j, errval, origval;
     char errcode[4];
     
     i=readline(ftp_control, inputline, maxlen);
     
     strncpy(errcode, inputline, 3);
     errcode[3] = '\0';

     origval = errval = atoi(errcode);
     
     /* If this is not in an expected range, display and abort */
     if (errval > level) {
	  AbortF("FTP",inputline);
          return(-1);
     }
     
     /* If a continuance line, read the rest */
     if (inputline[3] == '-')
	  while (errval == origval)
	  {
	       i = readline(ftp_control, inputline, maxlen);
	       
	       for (j = 0; (j < 4) && isspace(inputline[j]); j++);
	       
	       if (j == 4) continue;
	       
	       strncpy(errcode, inputline, 3);
	       errval = atoi(errcode);
	       
	       if (inputline[3] == '-') 
		    continue;
	       
	       if ((inputline[3] == ' ') && (errval == origval))
		    errval = 0;
	  } 
     
     return (0);
}

/***************** Connect, Login, and Disconnection routines *********/

/* 
  Establish connection, validate DNS name,
  connect and return control pointer
*/

int
FtpConnect(GopherObj *ftp, char *host)
{
     int newcontrol;
     
     /*** Open an ftp control connection ***/
     GSsetHost(ftp, host);
     GSsetPort(ftp, 21);
     
     newcontrol = GSconnect(ftp);
     if (newcontrol < 0) {
          GSdestroy(ftp);
	  AbortF("FtpConnect","Unable to connect to FTP server!");
     }
     
     return(newcontrol);
}


int
FtpLogin()
{
     char      inputline[512];
     /*** Send username ***/
     
     writestring(ftp_control, "USER anonymous\r\n");
     
     if (getreply(399,inputline,sizeof(inputline)) < 0)
	  return(-1);
     
     writestring(ftp_control, "PASS -gopher@");
     writestring(ftp_control, CurrentPeerName);
     writestring(ftp_control, "\r\n");
     
     if (getreply(399, inputline,sizeof(inputline)) < 0)
	  return(-1);
}


void 
QuitClose(int ftp_data)
{
     char inputline[512];
     char replyval[8];
     int i;
     
     /* no more sends and/or receives on data channel */
     i = shutdown(ftp_data, 2);

     /* close the data channel */
     i = closenet(ftp_data);

     writestring(ftp_control, "QUIT\r\n");

     /* no more sends on control channel */
     i = shutdown(ftp_control, 1);

     /* get reply and ignore, dealing with continuation lines if present */
     i = readline(ftp_control, inputline, sizeof(inputline));
     if (i > 3 && inputline[3] == '-') {
         strncpy(replyval, inputline, 3);
	 replyval[3] = '\0';
         do {
             i = readline(ftp_control, inputline, sizeof(inputline));
         } while((i > 3) && 
     	         (strncmp(replyval, inputline, 3) ||
     	          (!strncmp(replyval, inputline, 3) && inputline[3] != ' ')));
     }

     /* close the control channel */
     i = closenet(ftp_control);
}


boolean
NotText(char *buf)
{
     int max;   char *c;
     
     if ((max = strlen(buf)) >= (BUFSIZ - 50))
          max = BUFSIZ - 50;
     for (c = buf; c < (buf + max); c++) {
	  if (*c > '~') return(TRUE);
     }
     return(FALSE);
}


int
AbortF(char *where, char *complaint)
{
     int i;
     char inputline[BUFSIZ], errmsg[BUFSIZ+20], replyval[8];
     extern char RangeErr[256];
     
     strcpy(inputline, complaint);

     ZapCRLF(inputline);
     if (FtpReturnDir) {
          sprintf(errmsg, "%s:%s", where, inputline);
          Abortoutput(sockfd, errmsg, "range");
     }
     else {
          sprintf(errmsg, "Server Error: %s:%s\n\n", where, inputline);
	  writestring(sockfd, errmsg);
          if (!printfile(sockfd,&RangeErr[1],0,-1))
               writestring(sockfd, ".\r\n");
	  errmsg[strlen(errmsg)-2] = '\0';
          LOGGopher(sockfd, "Client Abort: %s", errmsg);
     }
     
     /** Finish reading any continuation lines so aFTP host won't hang. **/
     if (inputline[3] == '-') {
         strncpy(replyval, inputline, 3);
	 replyval[3] = '\0';
         do {
             i = readline(ftp_control, inputline, sizeof(inputline));
         } while((i > 3) && 
     	         (strncmp(replyval, inputline, 3) ||
     	          (!strncmp(replyval, inputline, 3) && inputline[3] != ' ')));
     }

     return(0);
}


boolean
IsBinaryType(char *path)
{
     static char *binExt[] = {
	  ".arj", ".arc", ".lzh", ".hyp", ".pak", ".pct",   /* A_PCBIN   */
	  /*".com", ".ps",*/
	  ".bck", ".exe", ".tar", ".Z", ".zip", ".zoo",     /* A_UNIXBIN */
	  ".bck_Z", ".tar_Z", ".bkp", "bkp-z", ".olb",
	  ".alpha_exe", "_Z", "-Z", ".wpg", ".wk1", ".app",
	  ".tos", ".ttp", ".prg", ".sea", ".cpt", ".sit",
	  ".msa", ".xbm", ".bmp", ".obj", ".hlb", ".mlb",
	  ".tlb", ".gz",
	  ".gif", ".pict", ".tiff", ".tif", ".pcx", ".xbm", /* A_IMAGE   */
     	  ".jpg", ".jpeg",
	  ".au", ".snd"};				    /* A_SOUND   */
     
     int extType, i;
     
     for (extType = 0; extType < 47; extType++) { 
	  i = strcasecmp(path + strlen(path) - strlen(binExt[extType]),
			 binExt[extType]);
	  if (i == 0) return(TRUE);
     }
     return(FALSE);
     
}


int
Vaxinate(char *bufptr)
{
     int last;
     char *cp;
     
     last = strlen(bufptr) - 1;

     /* strip off VMS version numbers */
     if (isdigit(bufptr[last]) && (cp=strrchr(bufptr, ';')) != NULL) {
          *cp = '\0';
	  last = strlen(bufptr) - 1;
     }
     
     /* if bufptr ends in ".dir", it's a directory, replace ".dir" with "/" */
     if((last > 3) && (strncmp(bufptr + last - 3, ".dir", 4) == 0))
     {
	  last -= 3;
	  bufptr[last] = '/';
	  bufptr[last+1]  = '\0';
	  return(last);
     }

     /* for files, uppercase terminal .z's or _z's */
     if (strlen(bufptr) > 1) {
          if (bufptr[last] == 'z' &&
	     (bufptr[last-1] == '.' || bufptr[last-1] == '_'))
               bufptr[last] = 'Z';
     }

     return(last);
}


int
GopherType(char *bufptr, char *theName, int ftptype)
{
     int last;
     
     
     if (ftptype == FTPISVMS) 
	  last = Vaxinate(bufptr);
     else
	  last = strlen(bufptr)-1;
     
     
     if (bufptr[last] == '/')
     {
	  sprintf(theName, "%c%s", A_DIRECTORY, bufptr);
	  theName[strlen(theName)-1] = '\0';
	  return(A_DIRECTORY);
     }
     
     if ((bufptr[last] == '*') || (bufptr[last] == '@'))  /* Hack out * and @ */
	  bufptr[last] = '\0';
     
     
     return(GopherFile(bufptr, theName, ftptype));
}



/* At this point we're looking at a file */

int
GopherFile(char *buf, char *theName, int ftptype)
{
     static char ext4[] = ".hqx";
     static char *ext5[] = {".arj", ".arc", ".lzh", ".hyp", ".pak", ".pct"
			    /*, ".com", ".ps"*/};
     static char *ext6[] = {".uu", ".uue"}; 
     static char *ext9[] = {".bck", ".exe", ".tar", ".Z", ".zip", ".zoo",
                            ".bck_Z", ".tar_Z", ".bkp", "bkp-z", ".olb",
			    ".alpha_exe", "_Z", "-Z", ".wpg", ".wk1", ".app",
			    ".tos", ".ttp", ".prg", ".sea", ".cpt", ".sit",
			    ".msa", ".xbm", ".bmp", ".obj", ".hlb", ".mlb",
			    ".tlb", ".gz"}; 
     static char *extI[] = {".gif", ".pict", ".tiff", ".tif", ".pcx", ".xbm",
     			    ".jpg", ".jpeg"};
     static char *exts[] = {".au", ".snd"};

     char	Gtype, *prefix;
     int		last;
     char	tmpName[SLEN];	
     int 	extType, i;
     
     last = strlen(buf) -1;
     
     strcpy(tmpName, buf);
     if (buf[last] == '/') {
	  tmpName[last] = '\0';
	  sprintf(theName, "%c%s", A_DIRECTORY, tmpName);
	  return;
     }
     if ((buf[last] == '*') || (buf[last] == '@')) {	/* Hack out * and @ */
	  buf[last] = '\0';
	  tmpName[last] = '\0';
     }
     
     /* At this point we're looking at a file */

     /* BinHex? */
     if (strcasecmp(tmpName + strlen(tmpName) - strlen(ext4), ext4) == 0) {
	  Gtype = A_MACHEX;
	  
	  sprintf(theName, "%c%s", Gtype, tmpName);
	  return;
     }
     
     /* PC garbage? */ 
     for (extType = 0; extType < 6; extType++) {
	  i = strcasecmp(tmpName + strlen(tmpName) - strlen(ext5[extType]), 
			 ext5[extType]);
	  if (i == 0) {
	       Gtype = A_PCBIN;
	  
	       sprintf(theName, "%c%s", Gtype, tmpName);
	       return;
	  }
     }
     
     /* UUENCODE'd? */
     for (extType = 0; extType < 2; extType++) {
	  i = strcasecmp(tmpName + strlen(tmpName) - strlen(ext6[extType]), 
			 ext6[extType]);
	  if (i == 0) {
	       Gtype = A_UUENCODE;
	  
	       sprintf(theName, "%c%s", Gtype, tmpName);
	       return;
	  }
     }
     
     /* Unix Binary? */
     for (extType = 0; extType < 31; extType++) {
	  i = strcasecmp(tmpName + strlen(tmpName) - strlen(ext9[extType]), 
			 ext9[extType]);
	  if (i == 0) {
	       Gtype = A_UNIXBIN;
	  
	       sprintf(theName, "%c%s", Gtype, tmpName);
	       return;
	  }
     }

     /* Image? */
     for (extType = 0; extType < 8; extType++) {
	  i = strcasecmp(tmpName + strlen(tmpName) - strlen(extI[extType]), 
			 extI[extType]);
	  if (i == 0) {
	       Gtype = A_IMAGE;
	  
	       sprintf(theName, "%c%s", Gtype, tmpName);
	       return;
	  }
     }	  

     /* Sound? */
     for (extType = 0; extType < 2; extType++) {
	  i = strcasecmp(tmpName + strlen(tmpName) - strlen(exts[extType]), 
			 exts[extType]);
	  if (i == 0) {
	       Gtype = A_SOUND;
	  
	       sprintf(theName, "%c%s", Gtype, tmpName);
	       return;
	  }
     }	  
     
     /* Some other and hopefully text file */
     sprintf(theName, "%c%s", A_FILE, tmpName);
     return;
}


int
ParseUnixList(char *bufptr, char *IntName, char *theName, int ftptype, int kind)
{
     int i, end, j;
     int gap;
     char *dirname, *alias;
     
     end = strlen(bufptr);
     
    /*
     *  Count white space runs to path.  If characters fill their fields,
     *  so that the expected number of white space runs isn't present,
     *  'i' will reach 'end'.  In that case, treat the characters after
     *  the preceding white space run (marked by 'j') as the path (this
     *  still can fail if the path itself has spaces, but it's much more
     *  reliable than what was in the v2.0 code). -- F.Macrides
     */
     for (i= 0, gap=0; (gap < kind) && (i < end); gap++)
     {
          /* Skip chars to white */
	  for (;(!isspace(bufptr[i])) && (i < end); i++);
	  if (i >= end) {
	       i = j;
	       break;
	  }
	  
          /* Skip white to chars */
	  for (;isspace(bufptr[i]) && (i < end); i++);
	  if (i >= end) {
	       i = j;
	       break;
	  }

	  /* Save current position */
	  j = i;
     }
     
     /* Point at supposed start-of-fileIntName */
     dirname = alias = &bufptr[i];
     
     if (dirname[strlen(dirname)-1] == '/')
	  dirname[strlen(dirname)-1] = '\0';

     switch(bufptr[0])
     {
     case 'l': /* Link? Skip to REAL IntName! */
         /*
	  *  This code still assumes all links point to a directory, and will
	  *  mishandle ones which point to a file, so we're still not using
	  *  it in the VMSGopherServer.  --  F.Macrides
	  */
	  for (dirname = alias ; (*dirname != '>') && (dirname != NULL);
	       dirname++)
	       ;
	  if (dirname == NULL)
	       return(-1); /* No real DirName?  Hm.  Oh well */
	  
	  dirname +=2; /* Skip space following arrow */
	  /* Grab fake name (sans arrow) */	  
	  strncpy(IntName,alias, dirname-alias-4);

	  /* Display name in theName */
	  sprintf(theName, "%c%s", A_DIRECTORY, IntName );

	  /*Internal name in 'IntName' */
	  strcpy(IntName,dirname);

	  if (IntName[strlen(IntName)-1] == '@')
	       IntName[strlen(IntName)-1] = '\0';

	  /* Tag slash on end */
	  sprintf(bufptr, "%s/", IntName);
	  
	  return(A_DIRECTORY);
	  break;
	  
     case 'd': /* Now treat as regular directory */
	  /* Display name in theName */
	  sprintf(theName, "%c%s", A_DIRECTORY, dirname);

	  /*Internal name in 'IntName' */
	  strcpy(IntName,dirname); 

	  /* Tag slash on end */
	  sprintf(bufptr, "%s/", IntName);
	  return(A_DIRECTORY);
	  break;
	  
     default: 
	  /* Determine type of file */
	  strcpy(IntName, dirname); 
	  GopherType(IntName, theName, ftptype);
	  strcpy(bufptr, theName+1);
	  break;
     }
     return(i);
}


/*--------------------------------*/
/* CWD into proper directory */

char *
WalkDirs(int ftptype, char *path)
{
     char *beg, *end;
     char vmspath[256], inputline[512];
     
     /* path looks like '/dir/dir.../dir/' now. Because Wollongong
      * wants "CWD dir/dir/dir" and Multinet wants "CWD dir.dir.dir"
      * so we do the CWD in a stepwise fashion. Oh well...
      */
     
     /* Is a VMS 'device:' specified? */
     if(ftptype == FTPISVMS &&(beg = strrchr(path, ':')) != 0) {
          /* Must be followed by a /directory/ spec */
	  if (*(++beg) != '/') return(NULL);
	  if (strlen(beg) < 3) return(NULL);
	  if ((end=strchr(beg+1, '/')) == 0) return(NULL);
	  
	  bzero(vmspath, 256);
	  strncpy(vmspath, path, end-path);
	  *(strchr(vmspath, '/')) =  '[';
	  strcat(vmspath, "]");
	       
	  writestring(ftp_control, "CWD ");
	  writestring(ftp_control, vmspath);
	  writestring(ftp_control, "\r\n");
	  
	  if (getreply(299,inputline,sizeof(inputline)) < 0)
	       return(NULL);
	  
	  beg = end+1;
     }
     else
          beg = path+1;

     for (end = beg; (*end != '\0') && (*end != '/'); ++end); 
     
     while (*end != '\0')
     {
	  bzero(vmspath, 256);
	  strncpy(vmspath, beg, end-beg);
	  
	  writestring(ftp_control, "CWD ");
	  writestring(ftp_control, vmspath);
	  writestring(ftp_control, "\r\n");
	  
	  if (getreply(299,inputline,sizeof(inputline)) < 0)
	       return(NULL);
	  
	  beg=end+1; /* Skip slash */
	  for (end = beg; (*end != '\0') && (*end != '/'); ++end); 
     }
     
     return(beg);
}

int
PortxFerDir(int ftptype, char *path)
{
     char inputline[512];
     char theName[BUFSIZ];
     
     /* Advance down directories to bottom */
     if (WalkDirs(ftptype, path) == NULL)
          return(-1);
     
     switch(ftptype) /* Send appropriate directory command */
     {
     case FTPISUNKNOWN:
	  Debug("PxFD: NLST -LF\n",0);
	  writestring(ftp_control, "NLST -LF\r\n");
	  break;
     case FTPISVMS:
	  Debug("PxFD: NLST\n",0);
	  writestring(ftp_control, "NLST\r\n");
	  break;
     case FTPISUNIX:
     case FTPISPORT:
	 /*
	  *  Don't use LIST -LF until a ParseUnixList() can handle it
	  *  properly - F.Macrides
	  *
	  Debug("PxFD: LIST -LF\n",0);
	  writestring(ftp_control, "LIST -LF\r\n");
	  break;
	  *
	  */
	  Debug("PxFD: NLST -LF\n",0);
	  writestring(ftp_control, "NLST -LF\r\n");
	  break;

     default:
	  Debug("PxFD: LIST\n",0);
	  writestring(ftp_control, "LIST\r\n");
	  break;
     }
     
     if (getreply(299,inputline,sizeof inputline) < 0)
          return(-1);
     
     return(GETDIR);
}

int
PortxFerFile(int ftptype, char *path)
{
     char *fname;
     char inputline[512];
     int getting;
     
     if ((getting = IsBinaryType(path)) == GETBINARY)
     {
	  writestring(ftp_control, "TYPE I\r\n");
	  
	  Debug("Send: TYPE I\n",0);
	  
	  if (getreply(299,inputline,sizeof(inputline)) < 0)
	       return(-1);
     }
     
     fname = WalkDirs(ftptype, path);
     
     writestring(ftp_control, "RETR ");
     writestring(ftp_control, fname);
     writestring(ftp_control, "\r\n");
     
     Debug("Send: RETR %s CRLF\n", fname);
     
     if (getreply(199,inputline,sizeof(inputline)) < 0)
          return(-1);

     return(getting);
}


int
PORTQuery(int ftptype, char *host, char *path)
{
     int       termCh, i;
     char      inputline[512], *cp;
     char      buf[BUFSIZ];     /*** Nice MTU size ***/
     int       ftp_data;
     int       ftp_dataport, nRead;
     int       getting;
     GopherObj *gs;
     struct sockaddr_in we;
     
     if ((ftp_dataport = SOCKlisten(&we)) < 0) {
	  AbortF("PQ1","No clue what FTP socket error!");
	  return(-1);
     }
     
     sprintf(inputline, "PORT %d,%d,%d,%d,%d,%d\r\n",
	     (htonl(we.sin_addr.s_addr) >> 24) & 0xFF,
	     (htonl(we.sin_addr.s_addr) >> 16) & 0xFF,
	     (htonl(we.sin_addr.s_addr) >>  8) & 0xFF,
	     (htonl(we.sin_addr.s_addr)      ) & 0xFF,
	     (htons(we.sin_port)        >>  8) & 0xFF,
	     (htons(we.sin_port)             ) & 0xFF);
     
     writestring(ftp_control, inputline); /* Send PORT command */
     
     if (getreply(299,inputline,sizeof(inputline)) < 0)
	  return(-1);
     
     termCh = path[strlen(path)-1]; /* Grab possible end char: / etc  */
     
     if (termCh == '/') /* Directory case */
	  getting = PortxFerDir(ftptype, path);
     else
	  getting = PortxFerFile(ftptype, path);
     if (getting < 0)
          return(-1);
     
     if ((ftp_data = SOCKaccept(ftp_dataport, we)) < 0) {
	  AbortF("PQ3","Unable to establish FTP data connection!");
	  return(-1);
     }
     
     /*** Transfer the data... ***/
     if (getting == GETDIR) {
          char theName[256];
          char outputline[512];
 
          while ((nRead = readline(ftp_data, buf, sizeof(buf))) > 0)
          {
	       ZapCRLF(buf);
	       if (ftptype==FTPISVMS)
	            for (i=0; i<nRead; i++) buf[i] = tolower(buf[i]);
	  
	       GopherType(buf, theName, ftptype);
	  
	       switch(ftptype) {

	       case FTPISNOVELL:
	            sprintf(outputline, "%s\tftp:%s@%s%s\t%s\t%d\r\n",
		            theName, host, path, &buf[FTPISNOVELL],
			    Zehostname, GDCgetPort(Config));
	            break;

	       default:
	            sprintf(outputline, "%s\tftp:%s@%s%s\t%s\t%d\r\n", 
		            theName, host, path, buf,
			    Zehostname, GDCgetPort(Config));
	            break;
	       }
	  
	       if (writestring(sockfd, outputline) < 0) {
	            LOGGopher(-2, "aFTP: Client Went Away!");
                    QuitClose(ftp_data);
	            return(-1);
	       }
          }
     
          if (writestring(sockfd, ".\r\n") < 0) {
               LOGGopher(-2, "aFTP: Client Went Away!");
               QuitClose(ftp_data);
	       return(-1);
	  }
     
     }
     else {
          while ((nRead = readn(ftp_data, buf, sizeof buf)) > 0) {
	       if (writen(sockfd, buf, nRead) <= 0) {
	            LOGGopher(-2, "aFTP: Client Went Away!");
                    QuitClose(ftp_data);
	            return(-1);
	       }
          }
          if (getting == GETFILE) {
               if (writestring(sockfd, ".\r\n") < 0) {
                    LOGGopher(-2, "aFTP: Client Went Away!");
                    QuitClose(ftp_data);
	            return(-1);
	       }
	  }
     }

     QuitClose(ftp_data);
     return(getting);
}

int
FtpPreAnalyze()
{
     char inputline[512];
     int i;
     
     /*** Strip off the connection message ***/
    if (getreply(299,inputline,sizeof(inputline)) < 0)
          return(-1);
     
     for (i = 4; i < strlen(inputline); i++) /* Scan first line for OS */
     {
	  if (strncmp(inputline+i, "NetWare", 7) == 0)
	       return(FTPISNOVELL);
	  if (strncmp(inputline+i, "SunOS 4.1", 9) == 0)
	       return(FTPISUNIX);
	  if (strncmp(inputline+i, "ULTRIX", 6) == 0)
	       return(FTPISUNIX);
	  if (strncmp(inputline+i, "Windows NT FTP", 14) == 0)
	       return(FTPISUNIX);
	  if (strncmp(inputline+i, "Peter's Macintosh FTP daemon", 28) == 0)
	       return(FTPISPORT);
          if (strncmp(inputline+i, "UNIX(r) System V",16) == 0)
               return(FTPISUNIX);
     }
     return (FTPISUNKNOWN);
}

int
AnalyzeType()
{
     int ftpflag;
     int resplen, i;
     int has_pasv, has_port;
     char  *typeptr;
     char inputline[2048];
     
     writestring(ftp_control, "SYST\r\n");
     
     getreply(999,inputline,sizeof(inputline));
     
     typeptr = inputline+4;
     inputline[3] = 0;
     ftpflag = atoi(inputline);
     
     switch (ftpflag)
     {
     case 215:
	  if (strncmp(typeptr, "MTS", 3) == 0)
	       return(FTPISVMS);
	  if (strncmp(typeptr, "VMS", 3) == 0)
	       return(FTPISVMS);
	  if (strncmp(typeptr, "UNIX", 4) == 0)
	       return(FTPISUNIX);
	  if (strncmp(typeptr, "MACOS", 5) == 0)
	       return(FTPISPORT);
     case 500:
     case 502:
     default:
	  break;
     }
     
     Debug("Didn't have SYST capabaility, checking HELP commands\n",0);
     
     writestring(ftp_control, "HELP\r\n");
     
     resplen = readline(ftp_control,inputline,sizeof(inputline));
     
     Debug("%s", inputline);
     
     has_pasv = FALSE;
     has_port = FALSE;
     
     if ((resplen) && (strncmp(inputline, "214", 3) != 0))
     {
	  Debug("FTP said %s\nA server that doesnt do HELP?\n", inputline);
	  resplen = 0;
     }
     
     bzero(inputline, sizeof(inputline));
     
     while((resplen > 0) && (strncmp(inputline, "214", 3) != 0))
     {
	  for (i = 0; i < resplen-4; i++)  /* search till next-to-end */
	  {
	       if (   (inputline[i] == 'P')  /* If we don't see a 'P' test no further */
		   && (inputline[i+4] != '*') ) /*And not neutralized by an asterisk */
	       {
		    if (strncmp(inputline+i, "PASV", 4) == 0) /*If it's PASV */
			 has_pasv = TRUE; /* It has PASV */
		    
		    if (strncmp(inputline+i, "PORT", 4) == 0) /*If it's PORT */
			 has_port = TRUE;
	       }
	  }
	  resplen = readline(ftp_control, inputline,sizeof(inputline)); 

	  /* No need to check further if it has both, clear buffer */
	  if (has_pasv && has_port)
	       while (strncmp(inputline, "214", 3) != 0)
		    resplen = getreply(399, inputline, sizeof(inputline));
     }
     
     Debug("PASV = %d, ", has_pasv);
     Debug("%s\n", (has_pasv ? "PASV detected" : "PASV not implemented\n"));
     Debug("PORT = %d, ", has_port);
     Debug("%s\n", (has_port ? "PORT detected" : "PORT not implemented\n"));
     
     if (has_port) return (FTPISPORT);
     if (has_pasv) return (FTPISPASV);
     
     return(FTPISUNKNOWN);
}

/*
 * This function parses the ftp: string, i.e. ftp:moo.com@/pub/ or
 * URL syntax: ftp://moo.com/pub/
 */

char *
ParseQuery(char *query)
{
     int i;
     int sLen;
     char termCh;
     
     /* Find @ or ? and parse out, abort if not present */
     for (i = 0; (query[i] != '\0') && (query[i] != '@') && (query[i] != '?'); i++);
     
     if ((i < 2) || (query[i] == '\0')) {
          FtpReturnDir = TRUE; /* Hopefully, a directory was requested */
	  AbortF("ParseQ","Invalid FTP query.");
	  return(NULL);
     }
     
     sLen = strlen(query);/* Point at end of string */
     termCh = query[sLen - 1];    /* Grab possible end char: / etc */
     
     if ((termCh == '*') || (termCh == '@')) {
	  query[sLen - 1] = '\0'; /* Clip invalid end chars */
	  termCh = query[sLen - 2];
     }
     
     query[i] = '\0';

     if (termCh == '/')
          FtpReturnDir = TRUE;
     else
          FtpReturnDir = FALSE;
     
     i++;
     
     return(&query[i]);
}

void
SendFtpQuery(int ctrl_sockfd, char *query)
{
     int ftptype;
     int getting;
     char *path;
     GopherObj *ftp;
     
     sockfd = ctrl_sockfd; /* Make it global, makes life easier */

     if ((path = ParseQuery(query)) == NULL) {
          Debug("ParseQ: Invalid FTP query.\n",0);
          return;
     }
     
     Debug(":Query=%s\n", query);
     Debug(":Path =%s\n", path);
     
     /*** hook onto the gs code to do our ftp socket connects ***/
     ftp = GSnew();
     
     /* Use query str to make control connection */
     if ((ftp_control = FtpConnect(ftp, query)) < 0)
          return;
     Debug("Connected\n",0);

     if ((ftptype = FtpPreAnalyze()) < 0) {
          GSdestroy(ftp);
          return;
     }
     
     if (FtpLogin() < 0) {
          GSdestroy(ftp);
          return;
     }
     
     if (ftptype == FTPISUNKNOWN)
	  ftptype = AnalyzeType();
     
     if (ftptype == FTPISUNKNOWN) {
	  AbortF ("SFQ1", "The FTP host is an UNKNOWN platform?!");
	  GSdestroy(ftp);
	  return;
     }
     
     switch(ftptype)
     {
     case FTPISPASV:
	  AbortF ("SFQ1", "The FTP host requires PASV - Not implemented!");
	  GSdestroy(ftp);
	  return;
     default:
	  Debug("About to PORTQuery %d\n", ftptype);
	  getting = PORTQuery(ftptype, query, path);
	  break;
     }

     GSdestroy(ftp);
     return;
}
