/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:27:52 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/GDgopherdir.c,v $
 * $Status: $
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
 * $Log: GDgopherdir.c,v $
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#include "GDgopherdir.h"
#include "Malloc.h"


#include <string.h>
#include <stdio.h>
extern int DEBUG;


/***********************************************************************
** Stuff for GopherDirObjs
**
***********************************************************************/


GopherDirObj*
GDnew(size)
  int size;
{
     int i;
     GopherDirObj *temp;
     
     temp = (GopherDirObj*) malloc(sizeof(GopherDirObj));

     temp->Gophers = DAnew(size, GSnew, GSinit, GSdestroy, GScpy);

     temp->Title = STRnew();
     temp->currentitem = 1;

     GDinit(temp);
     return(temp);
}


void
GDdestroy(gd)
  GopherDirObj *gd;
{
     int i;

     DAdestroy(gd->Gophers);
     
     STRdestroy(gd->Title);
     free(gd);
}


void
GDinit(gd)
  GopherDirObj *gd;
{
     int i;

     DAinit(gd->Gophers);
     STRinit(gd->Title);
}


extern int DEBUG;

/** This proc adds a GopherObj to a gopherdir. **/
void
GDaddGS(gd, gs)
  GopherDirObj *gd;
  GopherObj *gs;
{
	int top;


	if (GSgetNum(gs) == -99) {
	    if (DEBUG)
		fprintf(stderr, "Ignoring : %s\n", GSgetTitle(gs));
	    return;	/** somebody wants this item hidden **/
	}

	if (DEBUG)
	  fprintf(stderr, "Adding #%d: %s\n", GSgetNum(gs), GSgetTitle(gs));
	DApush(gd->Gophers, gs);
}


/*
 * Really weird!!!  We need this for qsort,  don't know why we can't use
 * GScmp...
 */

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
	  return(strcmp(GSgetTitle(*gs1), GSgetTitle(*gs2)));

     /** first one numbered, second not **/
     if (GSgetNum(*gs1) != -1 && GSgetNum(*gs2) == -1)
	  return(-1);

     /** second one numbered, first not **/
     if (GSgetNum(*gs1) == -1 && GSgetNum(*gs2) != -1)
	  return(1);

     /** Both numbered, integer compare them **/

     return(GSgetNum(*gs1) - GSgetNum(*gs2));
}

/*
 * Adds the "Date+Size" patch
 */

void
GDaddDateNsize(gd)
GopherDirObj *gd;
{
    int		i;
    GopherObj	*ge;

    for (i=0; 
	i< GDgetNumitems(gd); 
	    GSaddDateNsize(ge = GDgetEntry(gd, i++)));
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
GDtoNet(gd, sockfd)
  GopherDirObj *gd;
  int sockfd;
{
     int i;

     for (i=0; i< GDgetNumitems(gd); i++) {
	  GStoNet(GDgetEntry(gd, i), sockfd);
     }	  

}

void
GDtoNetHTML(gd, sockfd)
  GopherDirObj *gd;
  int sockfd;
{
     int i;
     
     writestring(sockfd, "<MENU>\r\n");
     
     for (i=0; i< GDgetNumitems(gd); i++) {
	  writestring(sockfd, "<LI>");
	  GStoNetHTML(GDgetEntry(gd, i), sockfd);
     }	  
     writestring(sockfd, "</MENU>");
}


/*
 * Fill up a GopherDirObj with GopherObjs, given a gopher directory coming
 * from sockfd.
 *
 * For each GopherObj retrieved, eachitem() is executed.
 *
 *  Returns 

 */

int
GDfromNet(gd, sockfd, eachitem)
  GopherDirObj *gd;
  int sockfd;
  int (*eachitem)();
{
     static GopherObj *TempGopher;
     static char ZesTmp[1024];
     int j, i;

     if (TempGopher == NULL)
	  TempGopher = GSnew();

     for (j=0; ; j++) {

	  ZesTmp[0] = '\0';
	  
	  GSinit(TempGopher);
	  i = GSfromNet(TempGopher, sockfd);
	  
	  if (i==0) {
	       GDaddGS(gd, TempGopher);
	       if (eachitem != NULL) 
		    eachitem();
	  }

	  else if (i==1)
	       return(j);

	  /*** Unknown object type ***/
	  if (i== -2) {
	       j = j-1;
	       if (j<0) j=0;
	       readline(sockfd, ZesTmp, 1024); /** Get the rest of the line **/
	       ;
	  }
	  if (i==-1) {
	       j = j-1;
	       if (j<0) j=0;
	       readline(sockfd, ZesTmp, 1024); /** Get the rest of the line **/
	       return(j);
	  }
     }
} 


/*
 * Given an open file descriptor and an inited GopherDirobj,
 *   read in gopher links, and add them to a gopherdir
 */
#ifdef SERVER
void
GDfromLink(gd, fd, host, port, filename, sockfd, access)
  int	       sockfd;
  int	       access;
#else
void
GDfromLink(gd, fd, host, port, filename)
#endif
  GopherDirObj *gd;
  int          fd;
  char         *host;
  int          port;
  char	       *filename;
{
     GopherObj *gs;

     gs = GSnew();

     while (GSfromLink(gs, fd, host, port, filename) != -1) {
#ifdef SERVER
	if (GScanAccess(sockfd, gs, access))
#endif
	  GDaddGS(gd, gs);
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
	  GStoLink(GDgetEntry(gd, i), fd);
     }	  

}
