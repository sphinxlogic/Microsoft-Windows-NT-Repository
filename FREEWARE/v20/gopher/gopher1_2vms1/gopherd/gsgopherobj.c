/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1.1.3 $
 * $Date: 1993/01/11 19:54:06 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/GSgopherobj.c,v $
 * $Status: $
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: GSgopherobj.c
 * Implement gopher directory functions.
 *********************************************************************
 * Revision History:
 * $Log: GSgopherobj.c,v $
 * Revision 1.1.1.3  1993/01/11  19:54:06  lindner
 * Fixed syntax in UCX #defines. (DuH!)
 *
 * Revision 1.1.1.2  1993/01/08  19:00:00  lindner
 * Mods for UCX
 *
 * Revision 1.1.1.1  1992/12/31  05:01:31  lindner
 * Chnages for VMS.
 *
 * Revision 1.1  1992/12/17  21:03:54  jqj
 * Initial revision
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/
#ifdef SERVER
#include "GopherD.h"
#endif
#include "GSgopherobj.h"

#if defined(mips) && defined(ultrix)   /*** Gross hack, yuck! ***/
#define _SIZE_T
#endif

#include "String.h"
#include <stdio.h>
#include "compatible.h"
#include <stat.h>
#include <time.h>
extern int DEBUG;

/*
 * These are needed for GSconnect().  Ick.
 */
#ifdef VMS
#include <socket.h>
#include <in.h>
#include <file.h>
#include <inet.h>
#include <netdb.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#ifndef hpux
#include <arpa/inet.h>
#endif
#include <netdb.h>
#endif  /* not VMS */

#include "Malloc.h"

/*
 * Make a new gopherobj, set the fields accordingly...
 */

GopherObj *
GSnewSet(objtype, Title, Selstr, Host, Port)
  char objtype;
  char *Title;
  char *Selstr;
  char *Host;
  int Port;
{
     GopherObj *temp;

     temp = (GopherObj *) malloc(sizeof(GopherObj));
     temp->Selstr   = STRnewSet(Selstr);
     temp->Title    = STRnewSet(Title);
     temp->Host     = STRnewSet(Host);
     temp->iPort    = Port;
     GSsetNum(temp, -1);
     GSsetWeight(temp, 0);
#ifdef SERVER
     temp->Access   = NULL;
     temp->Defaccess= ACC_FULL;
     temp->Head   = STRnew();
     temp->Foot   = STRnew();
     temp->RHead  = STRnew();
     temp->RFoot  = STRnew();
#endif
     return(temp);
}


/*
 * Make a new gopherobj...  Should reuse destroyed GopherObjs...
 */

GopherObj *
GSnew()
{
     GopherObj *temp;

     temp = (GopherObj *) malloc(sizeof(GopherObj));
     temp->Selstr = STRnew();
     temp->Title  = STRnew();
     temp->Host   = STRnew();
#ifdef SERVER
     temp->Access = NULL;
     temp->Defaccess=ACC_FULL;
     temp->Head   = STRnew();
     temp->Foot   = STRnew();
     temp->RHead  = STRnew();
     temp->RFoot  = STRnew();
#endif
     GSinit(temp);
     
     return(temp);
}

void
GSdestroy(gs)
  GopherObj *gs;
{
     STRdestroy(gs->Selstr);
     STRdestroy(gs->Title);
     STRdestroy(gs->Host);
#ifdef SERVER
     if (gs->Access != NULL)
	SiteArrDestroy(gs->Access);
     STRdestroy(gs->Head);
     STRdestroy(gs->Foot);
     STRdestroy(gs->RHead);
     STRdestroy(gs->RFoot);
#endif
     free(gs);
}
     


/*
 * Clear out all the crud 
 */

void
GSinit(gs)
  GopherObj *gs;
{
     GSsetType(gs, '\0');
     
     STRinit(gs->Title);
     STRinit(gs->Selstr);
     STRinit(gs->Host);
#ifdef SERVER
     if (gs->Access != NULL)
	SiteArrDestroy(gs->Access);
     gs->Access = NULL;
     gs->Defaccess = ACC_FULL;
     STRinit(gs->Head);
     STRinit(gs->Foot);
     STRinit(gs->RHead);
     STRinit(gs->RFoot);
#endif
     gs->iPort = 0;
     GSsetNum(gs, -1);
     GSsetWeight(gs, 0);
}


void
GStoNet(gs, sockfd)
  GopherObj *gs;
  int sockfd;
{
     static char buf[1024];

     buf[0] = GSgetType(gs);

     sprintf(buf + 1, "%s\t%s\t%s\t%d\r\n",
	     GSgetTitle(gs),
	     GSgetPath(gs),
	     GSgetHost(gs),
	     GSgetPort(gs));
     
     writestring(sockfd, buf);
     
     if (DEBUG)
	  fputs(buf, stderr);

}


void
GStoNetHTML(gs, sockfd)
  GopherObj *gs;
  int sockfd;
{
     static char buf[1024];
     static char pathbuf[1024];

     buf[0] = '\0';
     pathbuf[0] = '\0';

     /** Convert Path so that spaces are %20 **/
     Tohexstr(GSgetPath(gs), pathbuf);

     sprintf(buf, "<A HREF=http://%s:%d/%s>%s</A>",
	     GSgetHost(gs),
	     GSgetPort(gs),
	     pathbuf,
	     GSgetTitle(gs));

     writestring(sockfd, buf);

     if (DEBUG)
	  printf("HTML: %s", buf);
     
     if (GSgetWeight(gs) != 0) {
	  sprintf(buf, "Score: %d\r\n", GSgetWeight(gs));
	  writestring(sockfd, buf);
     }
     else
	  writestring(sockfd, "\r\n");

}


/*
 * Fill in a GopherObj, given an HREF= link from a WWW anchor..
 * So far only works with http
 */

void
GSfromHREF(gs, href)
  GopherObj *gs;
  char *href;
{
     char *cp;
     char *host;
     char path[1024];
     
     if (strncasecmp(href, "http://", 7)==0) {
	  host = href +7;
	  cp  = strchr(href+7, '/');
	  if (cp == NULL)
	       return;
	  
	  *cp = '\0';
	  strcpy(path, "GET ");
	  strcat(path, cp+1);
	  GSsetPath(gs, path);

	  GSsetType(gs, A_HTML);

	  GSsetPath(gs, path);
	  
	  cp = strchr(host, ':');
	  if (cp==NULL) 
	       GSsetPort(gs, 80);  /** default WWW port **/
	  else {
	       GSsetPort(gs, atoi(cp+1));
	       *cp = '\0';
	  }

	  GSsetHost(gs, host);
     }
}
	  



extern int readfield();
extern int readline();

int
GSfromNet(gs, sockfd)
  GopherObj *gs;
  int sockfd;
{
     char foo[1024];
     
     if (readfield(sockfd, foo, 1024)<= 0) {
	  /* EOF or error */
	  return(-1);
     }

     GSsetType(gs, foo[0]);

     /** Get the kind of file from the first character **/
     /** Filter out files that we can't deal with **/

     switch (GSgetType(gs)) {
       case A_FILE:
       case A_DIRECTORY:
       case A_MACHEX:
       case A_PCBIN:
       case A_UUENCODE:
       case A_CSO:
       case A_INDEX:
       case A_TELNET:
       case A_SOUND:
       case A_UNIXBIN:
       case A_GIF:
       case A_HTML:
       case A_TN3270:
       case A_MIME:
       case A_IMAGE:
	  break;
       case A_EOI:
	  if (foo[1] == '\r' && foo[2] == '\n')
	       return(1);
     default:
	return(-2);  
     }

     /** Suck off the User Displayable name **/
     GSsetTitle(gs, foo+1);
     
     /** Suck off the Pathname **/
     if (readfield(sockfd, foo, 1024) == 0)
	  return(-1);
     GSsetPath(gs, foo);

     /** Suck off the hostname **/
     if (readfield(sockfd, foo, 1024) == 0)
	  return(-1);
     GSsetHost(gs, foo);

     if (readline(sockfd, foo, 1024)==0)
	  return(-1); 

     GSsetPort(gs, 0);

     /** Get the port number **/
     GSsetPort(gs, atoi(foo));

     return(0);
}

/** Copy a GopherObj ***/

void
GScpy(dest, orig)
  GopherObj *dest, *orig;
{
     dest->sFileType = orig->sFileType;
     dest->iPort     = orig->iPort;
     dest->Itemnum   = orig->Itemnum;
#ifdef SERVER
     if (dest->Access != NULL)
	SiteArrDestroy(dest->Access);
     dest->Access = NULL;
     if (orig->Access != NULL) {
	Site *temp;
	int i;
	dest->Access = SiteArrayNew();
	for (i=0; i< DAgetTop(orig->Access); i++) {
	  temp = SiteArrgetEntry(orig->Access, i);
	  SiteArrayAdd(dest->Access,temp->domain,temp->Level);
	}
     }
     dest->Defaccess = orig->Defaccess;
     GSsetHeader(dest, GSgetHeader(orig));
     GSsetFooter(dest, GSgetFooter(orig));
     GSsetRHeader(dest, GSgetRHeader(orig));
     GSsetRFooter(dest, GSgetRFooter(orig));
#endif
     GSsetTitle(dest, GSgetTitle(orig));
     GSsetPath(dest, GSgetPath(orig));
     GSsetHost(dest, GSgetHost(orig));
}

/** Compare two GopherObjs ***/

int
GScmp(gs1, gs2)
  GopherObj *gs1, *gs2;
{
     if (GSgetTitle(gs1) == NULL)
	  return(1);
     if (GSgetTitle(gs2) == NULL)
	  return(-1);

     return(strcasecmp(GSgetTitle(gs1), GSgetTitle(gs2)));
}




/* GSconnect performs a connection to socket 'service' on host
 * 'host'.  Host can be a hostname or ip-address.  If 'host' is null, the
 * local host is assumed.   The parameter full_hostname will, on return,
 * contain the expanded hostname (if possible).  Note that full_hostname is a
 * pointer to a char *, and is allocated by connect_to_gopher()
 *
 * Errors:
 *
 * -1 get service failed
 *
 * -2 get host failed
 *
 * -3 socket call failed
 *
 * -4 connect call failed
 */

int
GSconnect(gs)
  GopherObj *gs;
{
     struct sockaddr_in Server;
     struct hostent *HostPtr;
     int iSock = 0;
     int ERRinet = -1;

#ifdef _CRAY
     ERRinet = 0xFFFFFFFF;  /* -1 doesn't sign extend on 64 bit machines */
#endif

     /*** Find the hostname address ***/
     
     if ((Server.sin_addr.s_addr = inet_addr(GSgetHost(gs))) == ERRinet) {
	  if (HostPtr = gethostbyname(GSgetHost(gs))) {
	       bzero((char *) &Server, sizeof(Server));
	       bcopy(HostPtr->h_addr, (char *) &Server.sin_addr, HostPtr->h_length);
	       Server.sin_family = HostPtr->h_addrtype;
	  } else
	       return (-2);
     } else
	  Server.sin_family = AF_INET;

     Server.sin_port = (unsigned short) htons(GSgetPort(gs));

     /*** Open the socket ***/

     if ((iSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	  return (-3);

#ifndef UCX
     setsockopt(iSock, SOL_SOCKET, ~SO_LINGER, 0, 0);
#endif

     setsockopt(iSock, SOL_SOCKET, SO_REUSEADDR, 0, 0);
     setsockopt(iSock, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

     /*** Connect ***/

     if (connect(iSock, (struct sockaddr *) &Server, sizeof(Server)) < 0) {
	  closenet(iSock);
	  return (-4);
     }
     return(iSock);
}

/*
 * GSfromLink takes an open file descriptor and starts reading from it.
 * It keeps going until it findsL
 *    enough fields for a gopherobj
 *    no data left
 * 
 * returns 0 with success, -1 on an error.
 */

#define	G_PATH	(1<<0)
#define	G_TYPE  (1<<1)
#define	G_NAME	(1<<2)
#define	G_PORT	(1<<3)
#define	G_HOST	(1<<4)
#define	G_ALL (G_PATH | G_TYPE | G_NAME | G_PORT | G_HOST)

int
GSfromLink(gs, fd, host, port, filename)
  GopherObj *gs;
  FILE	    *fd;
  char      *host;
  int       port;
  char	    *filename;
{
     int doneflags = 0;
     char buf[1024];

     while ((doneflags != G_ALL) && fgets(buf, 1024, fd)) {
	  if (buf[0] == '#')
	       continue;   /* comment */

	  ZapCRLF(buf);
#ifdef VMS
	  Continuation(buf, fd, 1024, '-');
#endif
	  if (DEBUG)
	    printf("%s: %s\n", filename, buf);

	  if (strncasecmp(buf, GS_TYPE, strlen(GS_TYPE))==0) {
	       GSsetType(gs, buf[strlen(GS_TYPE)]);
	       doneflags |= G_TYPE;
	  }
	  else
	  if (strncasecmp(buf, GS_NAME, strlen(GS_NAME))==0) {
	       GSsetTitle(gs, buf+strlen(GS_NAME));
	       doneflags |= G_NAME;
	  }
 	  else
	  if (strncasecmp(buf, GS_PATH, strlen(GS_PATH))==0) {
	       GSsetPath(gs, buf+strlen(GS_PATH));
	       doneflags |= G_PATH;
	  }
 	  else
	  if (strncasecmp(buf, GS_HOST, strlen(GS_HOST))==0) {
	       if ((buf[strlen(GS_HOST)] == '+'
			|| buf[strlen(GS_HOST)] == '*') 
			    && buf[strlen(GS_HOST)+1] == '\0')
		    GSsetHost(gs, host);
	       else
		    GSsetHost(gs, buf+strlen(GS_HOST));

	       doneflags |= G_HOST;
	  }
	  else
	  if (strncasecmp(buf, GS_PORT, strlen(GS_PORT))==0) {
	       if ((buf[strlen(GS_PORT)] == '+'
			|| buf[strlen(GS_PORT)] == '*') 
			    && buf[strlen(GS_PORT)+1] == '\0')
		    GSsetPort(gs, 0);
	       else
		    GSsetPort(gs, atoi(buf+strlen(GS_PORT)));

	       doneflags |= G_PORT;
	  }
	  else
	  if (strncasecmp(buf, GS_NUMB, strlen(GS_NUMB))==0)
	       GSsetNum(gs, atoi(buf+strlen(GS_NUMB)));
	  else
	  if (strncasecmp(buf, GS_HDDN, strlen(GS_HDDN))==0)
	       GSsetNum(gs, -99);
#ifdef SERVER
	  else
	  if (strncasecmp(buf, GS_ACCS, strlen(GS_ACCS))==0) {
	    if (GSgetAccess(gs) == NULL)
		GSsetAccess(gs, SiteArrayNew());
	    GSsetAccessSite(gs, buf+strlen(GS_ACCS));
	    GSsetDefAcc(gs,GSgetSiteAccess(gs, "default"));
	    if (GSgetDefAcc(gs) == ACC_UNKNOWN)
		GSsetDefAcc(gs,ACC_FULL);
	  }
	  else
	  if (strncasecmp(buf, GS_HEAD, strlen(GS_HEAD))==0)
	       GSsetHeader(gs, buf+strlen(GS_HEAD));
	  else
	  if (strncasecmp(buf, GS_FOOT, strlen(GS_FOOT))==0)
	       GSsetFooter(gs, buf+strlen(GS_FOOT));
	  else
	  if (strncasecmp(buf, GS_RHEAD, strlen(GS_RHEAD))==0)
	       GSsetRHeader(gs, buf+strlen(GS_RHEAD));
	  else
	  if (strncasecmp(buf, GS_RFOOT, strlen(GS_RFOOT))==0)
	       GSsetRFooter(gs, buf+strlen(GS_RFOOT));
#endif
     }
#ifdef SERVER
     if (GSgetPort(gs)==0)
	if (GSgetPath(gs))
	    switch(*GSgetPath(gs))
	    {
	    case A_INDEX:	GSsetPort(gs, GDCgetSRCHPort(Config)); break;
	    case A_FTP:	GSsetPort(gs, GDCgetFTPPort(Config));  break;
	    case A_EVENT:	GSsetPort(gs, GDCgetEXECPort(Config)); break;
	    default:	GSsetPort(gs,port);
	    }
	else
	    GSsetPort(gs,port);
#endif
     return ((doneflags == G_ALL) ? 0 : -1); /* 0 == success */
}


void
GStoLink(gs, fd)
  GopherObj *gs;
  int fd;
{
     char gtype[2];
     char portnum[16];
     
     gtype[0] = GSgetType(gs);
     gtype[1] = '\0';

     writestring(fd, "#");
     writestring(fd, "\nType=");
     writestring(fd, gtype);
     writestring(fd, "\nName=");
     writestring(fd, GSgetTitle(gs));
     writestring(fd, "\nPath=");
     writestring(fd, GSgetPath(gs));
     writestring(fd, "\nHost=");
     writestring(fd, GSgetHost(gs));
     writestring(fd, "\nPort=");
     sprintf(portnum, "%d", GSgetPort(gs));
     writestring(fd, portnum);
     writestring(fd, "\n");
}


#ifdef SERVER
/*
 * This tests to see if the current Gopher Object has an access specification 
 * matches the current client's address, which allows the specified access.
 */
boolean
GScanAccess(int sockfd, GopherObj *gs, int access)
{
     boolean		test;

     if (GSgetAccess(gs) == NULL)
	return(TRUE);

     if (DEBUG)
          printf("Testing %s/%s for access\n", CurrentPeerName,CurrentPeerIP);

     switch(access)
     {
     case ACC_SEARCH:
	test = SiteArrCanSearch(GSgetAccess(gs), CurrentPeerName,CurrentPeerIP);
	break;
     case ACC_READ:
        test = SiteArrCanRead(GSgetAccess(gs), CurrentPeerName,CurrentPeerIP);
	break;
     case ACC_BROWSE:
        test = SiteArrCanBrowse(GSgetAccess(gs), CurrentPeerName,CurrentPeerIP);
	break;
     default:
        if ((GSgetDefAcc(gs) & access) == access)
	    return(TRUE);
     }
     if (test != ACC_UNKNOWN)
	  return(test);
     
     if ((GSgetDefAcc(gs) & access) == access)
	  return(TRUE);
     else
	  return(FALSE);
}

/*
 * Adds the "Date+Size" patch
 *	(actually, edits symbolic tokens in the Title to reflect information
 *	about the file specified in the Path).  Nops if there are no symbolic
 *	tokens in the Title, if the Title or Path are NULL, or if the type
 *	code for the path is inappropriate for the operation specified.  Nops
 *	also if statistical information about the file specified can't be
 *	secured.
 */

void
GSaddDateNsize(GopherObj *ge)
{
    int 	fd;
    int		l;
    char  	longname[256];
    char	*ThisYear;
    time_t	elapsed;
    struct tm	*local;
    char	code[4];
    static char	*codes = "%sz%dt%fn%ts%ti%pt%ho";
    struct stat	buf;
    char	cdate[26];
    char	Teststr[256];
    char	*fp;
    char	*ft;
    char	*ftx;
#define	MMM	cdate+4
#define DD	cdate+8
#define HHMMSS	cdate+11
#define	YYYY	cdate+20

    if (*(fp = GSgetTitle(ge)) == '\0')		return;
    if (strchr(ft=GSgetTitle(ge),'%')==NULL)    return;
    if (*(fp = GSgetPath(ge)) == '\0')		return;
    switch(*fp) {
	case A_DIRECTORY:	/*** It's a directory ***/
	case A_INDEX:		/*** It's an index ***/
	case A_FTP:		/*** ftp link ***/
	case A_EVENT:		/*** exec link ***/
	case A_HTML:		/*** www link ***/
	case A_WAIS:		/*** wais or whois link ***/
		    return;
    }
    if (stat(fp+1, &buf) == 0) {		/* [date, size] date:     */
	/* One of the next two lines must be commented out.  If the first
	 * is commented out, the date displayed will be the last modification
	 * date.  If the first is uncommented and the second commented out,
	 * the date displayed will be the creation date.
	 */
/*	strcpy(cdate,ctime((unsigned long *) &buf.st_ctime)); /* Creation */
	strcpy(cdate,ctime((unsigned long *) &buf.st_mtime)); /* Revision */
	cdate[3] = cdate[7]= cdate[10]= cdate[19] = cdate[24]= '\0';
	if (cdate[8]==' ')
	    cdate[8] = '0';
    }
    else
	cdate[0] = '\0';
    for (l=0,code[3]=longname[0]='\0'; *ft!='\0';) {
	fd=strcspn(ft,"%");
	if (fd) {
	    strncat(longname,ft,fd);
	    ft += fd;
	    l += fd;
	}
	if (*ft=='%') {
	    strncpy(code,ft,3);
	    if (NULL == (ftx=strstr(codes,code))) {
		strncat(longname,ft++,1);
		l++;
		continue;
	    }
	    ft += 3;
	    switch((ftx-codes)/3) {
/* %sz */   case 0: if (!cdate[0])  break;
		    longname[l+=sprintf(longname+l,
				(buf.st_size >= 1000)?"%uKB":"%u Bytes",
				(buf.st_size >= 1000)?((buf.st_size+1023)/1024)
						     :buf.st_size)] = '\0';
		    break;
/* %fn */   case 2: strcat(longname, strchr(fp,']')+1);
		    l=strlen(longname);
		    break;
/* %ts */   case 3: if (!cdate[0])  break;
		    elapsed = time(&elapsed);
		    local = localtime(&elapsed);
		    ThisYear = asctime(local) + 20;
		    if (strncmp(YYYY,ThisYear,4)==0) {
			longname[l+=sprintf(longname+l,"%s-%s %s",
					    DD, MMM, HHMMSS)]='\0';
			break;
		    }
/* %dt */   case 1: if (!cdate[0])  break;
		    longname[l+=sprintf(longname+l,"%s-%s-%s",
					    DD, MMM, YYYY)] = '\0';
		    break;
/* %ti */   case 4: if (!cdate[0])  break;
		    strcat(longname, HHMMSS);   l+=strlen(HHMMSS);
		    break;
/* %pt */   case 5: longname[l+=sprintf(longname+l,"%d",GSgetPort(ge))] = '\0';
		    break;
/* %ho */   case 6: strcat(longname, GSgetHost(ge));
		    l=strlen(longname);
		    break;
	    }
	}
    }
    GSsetTitle(ge,longname);
}
#endif
