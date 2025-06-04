/********************************************************************
 * $Author: lindner $
 * $Revision: 1.1 $
 * $Date: 1992/12/10 23:27:52 $
 * $Source: /home/mudhoney/GopherSrc/release1.11/object/RCS/GSgopherobj.h,v $
 * $Status: $
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
 * $Log: GSgopherobj.h,v $
 * Revision 1.1  1992/12/10  23:27:52  lindner
 * gopher 1.1 release
 *
 *
 *********************************************************************/


#ifndef GSGOPHEROBJ_H
#define GSGOPHEROBJ_H

#if defined(SERVER) && !defined(SiteArray)
#include "site.h"
#endif

#include "STRstring.h"

#define A_FILE      '0'      /* Types of objects */
#define A_RANGE	    'R'
#define A_DIRECTORY '1'
#define A_CSO       '2'
#define A_ERROR     '3'
#define A_MACHEX    '4'
#define A_PCBIN     '5'
#define A_UUENCODE  '6'
#define A_INDEX     '7'
#define A_TELNET    '8'
#define A_UNIXBIN   '9'
#define A_SOUND     's'
#define A_EVENT     'e'
#define A_FTP	    'f'
#define A_CALENDAR  'c'
#define A_GIF       'g'
#define A_HTML      'h'
#define A_TN3270    'T'
#define A_MIME      'M'
#define A_MAILSPOOL 'm'
#define A_IMAGE     'I'
#define A_WAIS	    'w'
#define A_EOI	    '.'

#define GS_NAME		"NAME="
#define GS_TYPE		"TYPE="
#define GS_PORT		"PORT="
#define GS_PATH		"PATH="
#define	GS_HOST		"HOST="
#define	GS_NUMB		"NUMB="
#define	GS_HEAD		"HEAD="
#define	GS_FOOT		"FOOT="
#define	GS_RHEAD	"RHEAD="
#define	GS_RFOOT	"RFOOT="
#define	GS_HDDN		"HIDDEN"
#define	GS_ACCS		"ACCESS="
#define GS_TUPLE_CNT	12
#ifdef VMS
#define GOPHER_ACE	"GOPHER.L"
#define	GOPHER_ACESZ	(4+strlen(GOPHER_ACE))
#endif

/** Our Gopher Object Type **/

struct g_struct
{
     char    sFileType;     /* The type of object (A_FILE, A_CSO, etc)*/
     String  *Title;        /* User displayable title */
     String  *Selstr;       /* Selector string on host... */
     String  *Host;         /* Internet name of host                  */
     int     iPort;         /* Port number on host                    */
     int     Itemnum;      /* The number of the item in the directory*/
     int     weight;        /* The weight, expressed as a # from 0-1000 */
#ifdef SERVER
     SiteArray *Access;	    /* What sites can access this item? */
     Accesslevel Defaccess; /* And the item's default access level */
     String  *Head;	    /* lookaside-only printfile prefix header */
     String  *Foot;	    /* lookaside-only printfile suffix footer */
     String  *RHead;	    /* lookaside-only printfile range prefix header */
     String  *RFoot;	    /* lookaside-only printfile range suffix footer */
#endif
};



typedef struct g_struct GopherStruct;
typedef struct g_struct GopherObj;

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
#define GSincNum(a) ((a)->Itemnum++)

#define GSgetWeight(a)   ((a)->weight)
#define GSsetWeight(a,b) ((a)->weight=(b))
#ifdef SERVER
#define GSgetAccess(a)  ((a)->Access)
#define GSsetAccess(a,b) ((a)->Access=(b))
#define GSsetAccessSite(a,b) SiteProcessLine((a)->Access, (b), (a)->Defaccess)
#define GSgetDefAcc(a)  ((a)->Defaccess)
#define GSsetDefAcc(a,b) ((a)->Defaccess=(b))
#define GSgetSiteAccess(a,b) SiteAccess((a)->Access, (b))
#define GSgetHeader(a) ((STRget((a)->Head)))
#define GSsetHeader(a,b) ((STRset((a)->Head, b)))
#define GSgetFooter(a) ((STRget((a)->Foot)))
#define GSsetFooter(a,b) ((STRset((a)->Foot, b)))
#define GSgetRHeader(a) ((STRget((a)->RHead)))
#define GSsetRHeader(a,b) ((STRset((a)->RHead, b)))
#define GSgetRFooter(a) ((STRget((a)->RFoot)))
#define GSsetRFooter(a,b) ((STRset((a)->RFoot, b)))
#endif

/*** Real live functions defined in GSgopherobj.c ***/

GopherObj *GSnew();
GopherObj *GSnewSet();
void GSdestroy();
void GSinit();
void GStoNet();
void GStoNetHTML();
int  GSfromNet();
int  GSconnect();
int  GSfromLink();
void GStoLink();
void GScpy();
#ifdef SERVER
boolean GScanAccess();
void GSaddDateNsize();
#endif
#endif /*GSGOPHEROBJ_H*/
