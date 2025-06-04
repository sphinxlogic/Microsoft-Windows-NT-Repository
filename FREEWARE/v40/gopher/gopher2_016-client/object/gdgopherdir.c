/****************************************************************************
 * F.Macrides 13-Jul-1994   Added Alan Coopersmith's second patches:
	- on Unix use strcoll to sort instead of strcmp
	- add Location item to gopherdir struct to keep track of where
	  the directory came from
 * F.Macrides 01-Jul-1994   Added Lance's internationalization port to VMS:
 *   JL Wilkinson 23-June-1994	VMS doesn't have strcoll() so don't use it yet,
 *				even if the rest of the package is running
 *				internationalized.
 */
/********************************************************************
  Changes from Alan Coopersmith's patches:
    - ifdef GINTERNATIONAL, use strcoll to sort instead of strcmp
 ********************************************************************/

/********************************************************************
 * lindner
 * 3.22
 * 1994/04/27 19:21:37
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/GDgopherdir.c,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: GDgopherdir.c
 * Implement gopher directory routines
 *********************************************************************
 * Revision History:
 * GDgopherdir.c,v
 * Revision 3.22  1994/04/27  19:21:37  lindner
 * Fix for semicolons after Debugmsg
 *
 * Revision 3.21  1994/04/25  03:36:56  lindner
 * Modifications for Debug() and mismatched NULL arguments, added Debugmsg
 *
 * Revision 3.20  1994/04/21  21:24:57  lindner
 * Fix fcn header
 *
 * Revision 3.19  1994/04/19  14:32:27  lindner
 * Change GD and GSfromLink routines to use FIO
 *
 * Revision 3.18  1994/04/13  04:28:58  lindner
 * Fix for Type=X items
 *
 * Revision 3.17  1994/02/20  16:28:19  lindner
 * Remove dead code for GDtoNetHTML
 *
 * Revision 3.16  1994/01/10  03:27:28  lindner
 * Better GDdeleteGS method, allow ignoring of items by doing a Type=X in a .names file
 *
 * Revision 3.15  1993/11/29  01:07:57  lindner
 * In GDtoLink(), add FALSE argument to GStoLink() call in order to prevent
 * the Admin and ModDate information from being saved in the user bookmark
 * file.  (Macrides)
 *
 * Revision 3.14  1993/11/02  06:15:15  lindner
 * HTML additions
 *
 * Revision 3.13  1993/08/23  20:56:34  lindner
 * Fix for empty directory in g+ client
 *
 * Revision 3.12  1993/08/19  20:51:33  lindner
 * Mitra comments
 *
 * Revision 3.11  1993/08/19  20:24:04  lindner
 * Mitra's Debug patch
 *
 * Revision 3.10  1993/07/29  20:02:16  lindner
 * Removed dead variables
 *
 * Revision 3.9  1993/07/27  05:30:22  lindner
 * Mondo Debug overhaul from Mitra
 *
 * Revision 3.8  1993/07/27  00:30:08  lindner
 * plus patch from Mitra
 *
 * Revision 3.7  1993/07/14  20:37:08  lindner
 * Negative numbering patches
 *
 * Revision 3.6  1993/06/22  06:07:17  lindner
 * Added Domain= hacks..
 *
 * Revision 3.5  1993/04/15  21:35:12  lindner
 * Debug code, better .Link processing, better GDfromNet()
 *
 * Revision 3.4  1993/03/26  19:50:44  lindner
 * Mitra fixes for better/clearer fromNet code
 *
 * Revision 3.3  1993/03/24  17:04:49  lindner
 * bad strcmp() can check unmalloced() mem, fixed
 *
 * Revision 3.2  1993/03/18  22:13:29  lindner
 * filtering, compression fixes
 *
 * Revision 3.1  1993/02/11  18:03:01  lindner
 * Initial revision
 *
 * Revision 2.1  1993/02/09  22:46:50  lindner
 * Many additions for gopher+
 *
 * Revision 1.4  1993/01/31  00:22:51  lindner
 * Changed GDaddGS to merge entries with the same path.
 * Added GDplusfromNet() to siphon data from network.
 * GDfromLink now knows about ~/ inside of Path=
 * Changed GDSearch to ignore leading character.
 *
 * Revision 1.3  1992/12/19  04:44:09  lindner
 * Added GDplustoNet()
 *
 * Revision 1.2  1992/12/16  20:37:04  lindner
 * Added function GDsearch(), does a linear search of a gopher directory
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#include "GDgopherdir.h"
#include "Malloc.h"
#include "util.h"

#include "String.h"
#include <stdio.h>
#include "fileio.h"
#include "Debug.h"

/***********************************************************************
** Stuff for GopherDirObjs
**
***********************************************************************/


GopherDirObj*
GDnew(size)
  int size;
{
     GopherDirObj *temp;
     
     temp = (GopherDirObj*) malloc(sizeof(GopherDirObj));

     temp->Gophers = DAnew(size, GSnew, GSinit, GSdestroy, GScpy);

     temp->Title = STRnew();
     temp->Location = NULL;
     temp->currentitem = 1;

     GDinit(temp);
     return(temp);
}


void
GDdestroy(gd)
  GopherDirObj *gd;
{
     DAdestroy(gd->Gophers);
     if (gd->Location != NULL)
	  GSdestroy(gd->Location);
     STRdestroy(gd->Title);
     free(gd);
}


void
GDinit(gd)
  GopherDirObj *gd;
{
     DAinit(gd->Gophers);
     STRinit(gd->Title);
     gd->Location = NULL;
}


void
GDsetLocation(gd, gs)
  GopherDirObj *gd;
  GopherObj *gs;           
{
     if (gd->Location == NULL)
	  gd->Location = GSnew();
     else
	  GSinit(gd->Location);

     GScpy(gd->Location, gs);
}


/** This proc adds a GopherObj to a gopherdir.
    It will attempt to merge two items if need be..
 **/
void
GDaddGSmerge(gd, gs)
  GopherDirObj *gd;
  GopherObj *gs;
{
     int num;
     
     num = GDSearch(gd, GSgetPath(gs));
     
     if (num == -1)
	  GDaddGS(gd, gs);
     else {
	  if (GSgetType(gs) == 'X') {
	       gd = GDdeleteGS(gd, num);
	  } else
	       GSmerge(GDgetEntry(gd, num),gs);
     }
}

/*
 * This one never tries to merge 
 */

void
GDaddGS(gd, gs)
  GopherDirObj *gd;
  GopherObj *gs;
{
 
	if (GSgetType(gs) != 'X')
	     DApush(gd->Gophers, gs);
}


/*
 * Really weird!!!  We need this for qsort,  don't know why we can't use
 * GScmp...
 */

#define sgn(a)	((a) == 0 ? 0 : (a) < 0 ? -1 : 1)

int
GSqsortcmp(gs1, gs2)
  GopherObj **gs1, **gs2;
{
     if (GSgetTitle(*gs1) == NULL)
	  return(1);
     if (GSgetTitle(*gs2) == NULL)
	  return(-1);
     
     /** No numbering set on either entry, or both numbered
         entries have the same number   **/

     if (GSgetNum(*gs1) == GSgetNum(*gs2))
#ifndef VMS
	  /* compatible.h should #define strcoll strcmp if strcoll
	      is not available (it should be on ANSI C compliant systems) */
	  return(strcoll(GSgetTitle(*gs1), GSgetTitle(*gs2)));
#else
	  return(strcmp(GSgetTitle(*gs1), GSgetTitle(*gs2)));
#endif

     /** If the signs are equal, compare the numbers conventionally **/
     
     /** N.B. If the signs ARE equal, they cannot be 0 (otherwise we would **/
     /** have had the above case, because only the sign of 0 is 0)  */
     if (sgn(GSgetNum(*gs1)) == sgn(GSgetNum(*gs2)))
          return(GSgetNum(*gs1) < GSgetNum(*gs2) ? -1 : 1);

     /** The signs must be different, so we can use a conventional test, **/
     /** remembering only to say positive numbers go before negative ones **/
     return(GSgetNum(*gs1) > GSgetNum(*gs2) ? -1 : 1);
}

/*
 * Sorts a gopher directory
 */

void
GDsort(gd)
  GopherDirObj *gd;
{

     DAsort(gd->Gophers, GSqsortcmp);
}


void
GDtoNet(gd, sockfd, fmt)
  GopherDirObj *gd;
  int sockfd;
  GSformat fmt;
{
     int i;
     Debugmsg("GDplustoNet\n");

     if (fmt == GSFORM_HTML) {
	  writestring(sockfd, "<DL COMPACT>\r\n");
     }

     for (i=0; i< GDgetNumitems(gd); i++) {
	  GStoNet(GDgetEntry(gd, i), sockfd,fmt);
     }	  

     if (fmt == GSFORM_HTML) {
	  writestring(sockfd, "</DL>\r\n");
     }
}


void
GDplustoNet(gd, sockfd, filter)
  GopherDirObj *gd;
  int          sockfd;
  char         **filter;
{
     int i;

     for (i=0; i< GDgetNumitems(gd); i++) {
	  GSplustoNet(GDgetEntry(gd, i), sockfd,filter);
     }	  
}

/*
 * Gopher+ counterpart to GDfromNet()
 * returns number of items found
 */


int
GDplusfromNet(gd, fd, eachitem)
  GopherDirObj *gd;
  int fd;
  int (*eachitem)();
{
     static GopherObj *TempGopher;
     static char ZesTmp[1024];
     int j, result;
     char inputline[256];

     Debugmsg("GDplusfromNet:: start\r\n");
     if (TempGopher == NULL)
	  TempGopher = GSnew();

     /** State: _begin_ **/

     result = readrecvbuf(fd, inputline, 1);
     if (result <=0)
	  return(0);
     else if (*inputline == '.') {
	  /*** Read off the rest of the junk... ***/
	  readline(fd,inputline,sizeof(inputline));
	  return(0);
     }
     else if (*inputline != '+')
	  return(0);

     Debugmsg("after readrecvbuf");
     /** State _FirstPlus_ **/

     result = readtoken(fd, inputline, sizeof(inputline), ':');
     if (result <=0)
	  return(result);

     Debugmsg("after readtoken");
     if (strcmp(inputline, "INFO")!=0) {
	  return(0);
     }
     Debugmsg("after INFO");
     /** Read the space **/
     readrecvbuf(fd, inputline, 1);


     /*** State _FirstINFO_ ***/

     for (j=0; ; j++) {

     	  Debugmsg("for start");
	  ZesTmp[0] = '\0';
	  
	  GSinit(TempGopher);
	  result = GSplusfromNet(TempGopher, fd);
	  
	  switch (result) {
	  case MORECOMING:
	       GDaddGS(gd, TempGopher);
	       if (eachitem != NULL) 
		    eachitem();
	       break;

	  case FOUNDEOF:
	       GDaddGS(gd, TempGopher);
	       return(j+1);

	  case HARDERROR:  /** Give up reading - bad read or protocol error **/
	       return(j);

	  case SOFTERROR:  /** This one was bad, but we can try for next **/
	       j= j-1;
	       if (j<0) j=0;
	       break;
	  }
	       
     } /* for */

     /** Never get here **/
} 

/*
 * Fill up a GopherDirObj with GopherObjs, given a gopher directory coming
 * from sockfd.
 *
 * For each GopherObj retrieved, eachitem() is executed.
 *
 */

void
GDfromNet(gd, sockfd, eachitem)
  GopherDirObj *gd;
  int sockfd;
  int (*eachitem)();
{
     static GopherObj *TempGopher;
     static char ZesTmp[1024];
     int i;

     Debugmsg("GDfromNet...");
     if (TempGopher == NULL)
	  TempGopher = GSnew();

     for (; ;) {

	  ZesTmp[0] = '\0';
	  
	  GSinit(TempGopher);
	  i = GSfromNet(TempGopher, sockfd);
	  
         /* In gopher+1.2b2 this routine clears up if GSfromNet returns 
            a failure, better to clear up in GSfromNet so that the 
            system returns in a known state - note that other callers of 
            GSfromNet didn't clean up and crashed! */
	  
	  switch (i) {

	  case 0:
	       GDaddGS(gd, TempGopher);
	       if (eachitem != NULL) eachitem();
	       break;

	  case 1:  /* . on a line by itself, nothing more */
	       return;

	  case SOFTERROR:  /** Unknown object type **/
	       break;

	  case HARDERROR:
	       return;
	  }
     }
} 


/*
 * Given an open file descriptor and an inited GopherDirobj,
 *   read in gopher links, and add them to a gopherdir
 */

void
GDfromLink(gd, fio, host, port, directory, peer)
  GopherDirObj *gd;
  FileIO       *fio;
  char         *host;       /** Current Host **/
  int          port;        /** Current port **/
  char         *directory;  /** Current directory **/
  char         *peer;       /** connected client **/
{
     GopherObj *gs;
     int       result;
     char      *cp;

     gs = GSnew();


     while (1) {
	  result = GSfromLink(gs, fio, host, port,directory, peer);

	  if (result == HARDERROR)
	       break;

	  if (result == SOFTERROR)
	       continue;

	  cp = GSgetPath(gs);

	  if (*cp == '.')
	       GDaddGSmerge(gd, gs);
	  else
	       GDaddGS(gd, gs);

	  if (result == FOUNDEOF)
	       break;

	  GSinit(gs);
     }
	  
     GSdestroy(gs);
}


void
GDtoLink(gd, fd)
  GopherDirObj *gd;
  int fd;
{
     int i;

     for (i=0; i< GDgetNumitems(gd); i++) {
	  GStoLink(GDgetEntry(gd, i), fd, FALSE);
     }	  

}

/***  Search for a specific gopher item ***/
int
GDSearch(gd, text)
  GopherDirObj *gd;
  char         *text; 	/* Note first char is G0 type and is ignored*/
{
     int       i;
     GopherObj *gs;
     int       len;
     char      *cp;

     Debug("GDSearch: %s;\n",text);

     if (gd == NULL)
	  return(-1);

     if (text == NULL)
	  return(-1);

     len = strlen(text);
	
     for (i=0; i< GDgetNumitems(gd); i++) {
	  gs = GDgetEntry(gd, i);

	  cp = GSgetPath(gs);
	  if (len >1 && strcmp(text+1, cp+1) == 0) {
	       Debugmsg("Matched\n");
	       return(i);
	  }
     }
     Debugmsg("GDsearch: No Match\n");
     return(-1);
}

/* 
 * Delete an item in a gopher GD..  Do it in place...
 */

GopherDirObj *
GDdeleteGS(gd,j) 
  GopherDirObj	*gd;
  int		j;	/* Number of GS to delete */
{
     int i;

     if (GDgetNumitems(gd) == j+1) {
	  /* Last item in the directory */
	  GSinit(GDgetEntry(gd,j));
	  GDsetNumitems(gd, j);
	  return(gd);
     }
     
     /** Okay, now let's copy the items down, one by one..  **/

     for (i= j+1; i<GDgetNumitems(gd); i++) {
	  GScpy(GDgetEntry(gd,i-1), GDgetEntry(gd, i));
     }
     
     GDsetNumitems(gd, GDgetNumitems(gd)-1);

     if (GDgetCurrentItem(gd) > GDgetNumitems(gd)-1)
	  GDsetCurrentItem(gd, GDgetCurrentItem(gd)-1);

     return(gd);
}

