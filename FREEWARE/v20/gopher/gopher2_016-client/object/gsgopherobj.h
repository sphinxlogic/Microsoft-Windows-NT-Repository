/********************************************************************
 * F.Macrides 13-Jul-1994   Worked in Alan's patches:
	- ifdef GINTERNATIONAL & ifndef NO_STRFTIME, use a function
	  instead of a macro for GSgetModDate(), so we can localize
	  the date string with a call to strftime()
	- move GSfromURL() return code definitions from code so other
	  files can call it
 ********************************************************************/
/********************************************************************
 * lindner
 * 3.14
 * 1994/04/01 04:38:08
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/GSgopherobj.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: GSgopherobj.h
 * Header file, abstraction of a gopher type
 *********************************************************************
 * Revision History:
 * GSgopherobj.h,v
 * Revision 3.14  1994/04/01  04:38:08  lindner
 * Fix for conditional macros
 *
 * Revision 3.13  1994/03/31  21:02:40  lindner
 * Add some error and NULL checking to GS macros
 *
 * Revision 3.12  1993/12/27  16:22:43  lindner
 * Added more prototypes for GS functions
 *
 * Revision 3.11  1993/11/02  06:15:27  lindner
 * HTML additions
 *
 * Revision 3.10  1993/10/22  20:03:32  lindner
 * Add Movie (;) and Info (i) type support
 *
 * Revision 3.9  1993/09/18  04:44:43  lindner
 * Additions to fix caching of Multiple view items
 *
 * Revision 3.8  1993/09/11  06:33:06  lindner
 * URL support
 *
 * Revision 3.7  1993/07/27  20:18:09  lindner
 * Sorry can't take address of conditionals...
 *
 * Revision 3.6  1993/07/27  05:30:25  lindner
 * Mondo Debug overhaul from Mitra
 *
 * Revision 3.5  1993/07/27  00:30:11  lindner
 * plus patch from Mitra
 *
 * Revision 3.4  1993/07/23  04:50:59  lindner
 * Additional stored askdata
 *
 * Revision 3.3  1993/04/15  17:55:18  lindner
 * GSgetAdmin fix, plus more protos
 *
 * Revision 3.2  1993/03/24  17:09:42  lindner
 * Additions for Localfile for each GopherObj
 *
 * Revision 3.1.1.1  1993/02/11  18:03:03  lindner
 * Gopher+1.2beta release
 *
 * Revision 2.1  1993/02/09  22:47:54  lindner
 * Added new def GSSfindBlock
 *
 * Revision 1.2  1993/01/31  00:31:12  lindner
 * Gopher+ structures and macros.
 *
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#ifndef GSGOPHEROBJ_H
#define GSGOPHEROBJ_H

#include "boolean.h"
#include "STRstring.h"
#include "STAarray.h"
#include "VIews.h"
#include "url.h"
#ifdef VMS
#include "Locale.h"
#else
#include "compatible.h"
#endif

#define A_FILE      '0'      /* Types of objects */
#define A_DIRECTORY '1'
#define A_CSO       '2'
#define A_ERROR     '3'
#define A_MACHEX    '4'
#define A_PCBIN     '5'
#define A_INDEX     '7'
#define A_TELNET    '8'
#define A_UNIXBIN   '9'
#define A_SOUND     's'
#define A_EVENT     'e'
#define A_CALENDAR  'c'
#define A_GIF       'g'
#define A_HTML      'h'
#define A_TN3270    'T'
#define A_MIME      'M'
#define A_IMAGE     'I'
#define A_INFO      'i'
#define A_MOVIE     ';'
#define A_EOI	    '.'

typedef struct g_struct GopherObj;
#include "BLblock.h"

/*** Formats for GStoNet() ***/
#define GSFORM_G0    0
#define GSFORM_GPLUS 1
#define GSFORM_HTML  2

/*** Part codes for GSfromLink() & GSfromURL() ***/
#define	G_PATH	1
#define	G_TYPE  2
#define	G_NAME	4
#define	G_PORT	8
#define	G_HOST	16
#define	G_ALL (G_PATH | G_TYPE | G_NAME | G_PORT | G_HOST)

typedef int GSformat;

/*** Our gopher+ attributes structure ***/

struct gplus_struct
{
     /** Abstract Block **/
     /* String *abstract;	Superceeded by BlockArray */

     /** Admin Block **/
     String *Admin;
     String *ModDate;

     VIewArray *Views;
     /** Others Blocks**/
     BlockArray *OtherBlocks;

     /** Filled in askdata **/
     char **Askdata;
};

typedef struct gplus_struct GplusObj;	  

/** Our Gopher Object Type **/

struct g_struct
{
     char    sFileType;     /* The type of object (A_FILE, A_CSO, etc)*/
     String  *Title;        /* User displayable title */
     String  *Selstr;       /* Selector string on host... */
     String  *Host;         /* Internet name of host                  */
     int     iPort;         /* Port number on host                    */
     int     Itemnum;       /* The number of the item in the directory*/
     int     weight;        /* The weight, expressed as a # from 0-1000 */
     int     ttl;           /* The Time-To-Live, how long the item is good */

     String  *Localfile;    /* The local file of the item */
     String  *Localview;    /* The view of the local file of the item */

     boolean isgplus;       /* Item can be queried gopher+ style */
     boolean isask;         /* Item contains an ASK block */
     GplusObj *gplus;       /* Gopher + attributes */
     Url      *url;         /* The Unix^H^H^Hniversal Resource Locator */
};


typedef struct g_struct GopherStruct;

#define GSgetType(a) ((a)->sFileType)
#define GSsetType(a,b) (a)->sFileType=(b)

#define GSgetTitle(a) ((STRget((a)->Title)))
#define GSsetTitle(a,b) ((STRset((a)->Title, b)))

#define GSgetPath(a) ((STRget((a)->Selstr)))
#define GSsetPath(a,b) ((STRset((a)->Selstr, b)))

#define GSgetHost(a) ((STRget((a)->Host)))
#define GSsetHost(a,b) ((STRset((a)->Host, b)))

#define GSgetPort(a) ((a)->iPort)
#define GSsetPort(a,b) (a)->iPort=(b)

#define GSgetNum(a) ((a)->Itemnum)
#define GSsetNum(a,b) ((a)->Itemnum=(b))

#define GSgetWeight(a)   ((a)->weight)
#define GSsetWeight(a,b) ((a)->weight=(b))

#define GSgetTTL(a)   ((a)->ttl)
#define GSsetTTL(a,b) ((a)->ttl=(b))

#define GSgetLocalFile(a) ((STRget((a)->Localfile)))
#define GSsetLocalFile(a,b) ((STRset((a)->Localfile, (b))))

#define GSgetLocalView(a) ((STRget((a)->Localview)))
#define GSsetLocalView(a,b) ((STRset((a)->Localview, (b))))

/****************** Gopher Plus attributes *******************/
#define GSisGplus(a)    ((a)->isgplus)
#define GSsetGplus(a,b) ((a)->isgplus=(b))

#define GSisAsk(a)      ((a)->isask)
#define GSsetAsk(a,b)   ((a)->isask=(b))


#define GSgplusInited(a) ((a)->gplus != NULL)

#define GSgetAdmin(a)    (((a)->gplus == NULL) ? NULL : (STRget((a)->gplus->Admin)))

#if !defined(VMS) && (!defined(GINTERNATIONAL) || defined(NO_STRFTIME))
#define GSgetModDate(a)  (STRget((a)->gplus->ModDate)) 
#endif

#define GSgetOtherBlocks(a) (Blockobj*)(((a)->gplus == NULL) ? NULL : ((a)->gplus->OtherBlocks))

#define GSfindBlock(a,b)  GSgetBlock(a,BLAsearch(GSgetOtherBlocks(a),(b)))

#define GSgetAskdata(a)   (GSgplusInited(a) ? (a)->gplus->Askdata : NULL)

/*** Real live functions defined in GSgopherobj.c ***/

GopherObj *GSnew();
GopherObj *GSnewSet();
void GSdestroy();
void GSinit();
void GSplusnew();
void GStoNet();
void GStoNetHTML();
int  GSfromNet();
int  GSfromLink();
void GStoLink();
void GScpy();
void      GSsetBlock();
void      GSsetURL();

char     *GSgetURL();
char     *GSgetURLhtml();

#if defined(VMS) || (defined(GINTERNATIONAL) && !defined(NO_STRFTIME))
char	 *GSgetModDate();
#endif
struct tm *GSgetModDateTM();

int       GSgetNumViews();
VIewobj  *GSgetView();

int       GSgetNumBlocks();
Blockobj *GSgetBlock();

/****** Protocol transmission functions *******/
int  GSconnect();
void GStransmit();
void GSsendHeader();
void GSsendErrorHeader();
int  GSrecvHeader();
void GStransmit();

/******Gopher+ functions *******/
void GSplusnew();
void GSplusdestroy();
void GSplusInit();
void GSplustoNet();
void GSpluscpy();

char **GSsetAskdata();

/***** Debugging functions *******/
void GSplusPrint();

#endif /*GSGOPHEROBJ_H*/

