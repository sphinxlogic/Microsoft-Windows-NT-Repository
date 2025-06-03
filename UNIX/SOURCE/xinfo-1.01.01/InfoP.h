#ifndef _InfoP_h
#define _InfoP_h

/* $Header: /usr1/ben/jkh/tmp/xinfo/RCS/InfoP.h,v 1.4 91/01/07 14:02:11 jkh Exp Locker: jkh $ */

/*
 *
 *                   Copyright 1989, 1990
 *                    Jordan K. Hubbard
 *
 *                PCS Computer Systeme, GmbH.
 *                   Munich, West Germany
 *
 *
 * This file is part of GNU Info widget.
 *
 * The GNU Info widget is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 */

/*
 * $Log:	InfoP.h,v $
 * Revision 1.4  91/01/07  14:02:11  jkh
 * Some naming changes.
 * 
 * Revision 1.3  90/11/12  18:07:21  jkh
 * Added SET_BLOCK method for initializing text blocks.
 * 
 * Revision 1.2  90/11/11  22:24:27  jkh
 * Added retain_arg
 * 
 * Revision 1.1  90/11/11  21:20:02  jkh
 * Initial revision
 * 
 */

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#include "Info.h"

/* max length of argument string */
#define ARGLEN		128

/* max generic string size */
#define MAXSTR		128

/* The largest we expect a note or menu name to get */
#define MAXTOKEN	128

/* if no max pathlen, define arbitrarily */
#ifndef MAXPATHLEN
#define MAXPATHLEN	512
#endif

/* More explanatory macro names */
#define Export
#define Import	extern
#define Local	static
#ifdef __GNUC__
#define Inline __inline__
#else
#define Inline
#endif

/* If we couldn't find it anywhere, make it up */
#ifndef R_OK
#define R_OK	04
#endif

/* Convenience macro for setting text blocks */
#define SET_BLOCK(blk, start, end, string) \
	(blk).firstPos = (start), (blk).length = (end), \
	(blk).ptr = (string), (blk).format = FMT8BIT

/* number of inits to bump tables up by during allocation */
#define TABLE_INC	50

/* special INFO separator character */
#define INFO_CHAR(ch)	((ch) == '\000' || (ch) == '\037')

/* Delete char */
#define DEL_CHAR	'\177'

/* Indirect start token */
#define INDIRECT_TOKEN	"\037\nIndirect:\n"

/* Tag table start token */
#define TAGTABLE_TOKEN	"\nTag Table:\n"

/* Indirect tag table token */
#define ITAGTABLE_TOKEN	"(Indirect)\n"

/* Tag table end token */
#define TAGEND_TOKEN	"\037\nEnd tag table\n"

/* Node header tokens */
#define NODE_TOKEN	"Node: "
#define PREV_TOKEN	"Prev: "
#define NEXT_TOKEN	"Next: "
#define UP_TOKEN	"Up: "

/* Menu start token */
#define MENU_TOKEN	"\n* Menu:"

/* Menu seperator token */
#define MENU_SEP_TOKEN	"\n* "

/* Footnote start token */
#define NOTE_TOKEN	"*note"

/* End of a name */
#define NAME_END_TOKEN	"\t\n,."

/* white space */
#define WHITE		"\t\n\f "

/* delimiting characters that designate a node name */
#define NAME		"\t\n,."

/* Table manipulation macros */

#define ZERO_TABLE(tab)							\
     (tab).size = (tab).idx = 0;					\
     (tab).table = (ID_P)NULL

#define FREE_TABLE(tab)							\
     if ((tab).table)							\
	  XtFree((tab).table);						\
     ZERO_TABLE(tab)

#define ALLOC_TABLE(tab)						\
     FREE_TABLE(tab);							\
     (tab).size = TABLE_INC;						\
     (tab).idx = 0;							\
     (tab).table = (ID_P)XtMalloc(sizeof(ID) * TABLE_INC)

#define MAYBE_BUMP_TABLE(tab)						\
     (((tab).idx == (tab).size) ? (tab).size += TABLE_INC,		\
      (tab).table = (ID_P)XtRealloc((tab).table, (tab).size *		\
				    sizeof(ID)), 1 : 0)

#define ROUND_TABLE(tab)						\
     ((tab).table[(tab).idx].tag.name = NULL,				\
      (tab).table[(tab).idx].offset.length = 0,				\
      ((tab).idx + 1 != (tab).size) ? (tab).size = (tab).idx + 1,	\
      (tab).table = (ID_P)XtRealloc((tab).table, (tab).size *		\
				    sizeof(ID)), 1 : 0)

#define ZERO_LIST(lst)							\
     ZERO_TABLE((lst).t);						\
     (lst).l = (Strings)NULL

#define FREE_LIST(lst)							\
     FREE_TABLE((lst).t);						\
     if ((lst).l) {							\
	  Strings lp = (lst).l;						\
	  while (*lp)							\
	       XtFree(*(lp++));						\
	  XtFree((lst).l);						\
	  (lst).l = (Strings)NULL;					\
     }

#define ALLOC_LIST(lst)							\
     ALLOC_TABLE((lst).t);						\
     (lst).l = (Strings)XtMalloc(sizeof(String) * TABLE_INC)

#define FREE_TAG_TABLE(tab)						\
     {									\
     int i;								\
     for (i = 0; i < IDX((tab)); i++) 					\
          XtFree(I_NAME((tab).table[i])); 				\
     XtFree((tab).table);						\
     (tab).table = NULL;						\
     }

#define MAYBE_BUMP_LIST(lst)						\
     if (MAYBE_BUMP_TABLE((lst).t))					\
	 (lst).l = (Strings)XtRealloc((lst).l, (lst).t.size *		\
				      sizeof(String));			\

#define ROUND_LIST(lst)							\
     if (ROUND_TABLE((lst).t))						\
	 (lst).l = (Strings)XtRealloc((lst).l, (lst).t.size *		\
				      sizeof(String));			\
     (lst).l[(lst).t.idx] = NULL

#define IDX(tab)	((tab).idx)
#define TPOS(tab)	((tab).table[IDX(tab)])
#define LPOS(lst)	((lst).l[IDX((lst).t)])
#define INCP(tab)	(IDX(tab)++)

/* Turn address s into ptr relative index */
#define INTOFF(ptr, s)		((int)((s) - (ptr)))

typedef String *Strings;
     
typedef struct {
     int nichts;
} InfoClassPart;

typedef struct _InfoClassRec {
     CoreClassPart		core_class;
     CompositeClassPart		composite_class;
     InfoClassPart		info_class;
} InfoClassRec;

extern InfoClassRec infoClassRec;

/* A generic ID (tag/offset). */
typedef union _id {
     struct {				/* if it's a tag		*/
	  String name;
	  int offset;
     } tag;
     struct {				/* if it's an offset		*/
	  int start;
	  int length;
     } offset;
} ID, *ID_P;

/* An array of ID's */
typedef struct _table {
     int idx;				/* where we are in the table	*/
     int size;
     ID_P table;
} Table;

/* a special string/ID associative table */
typedef struct _idlist {
     Table t;				/* ID array representation	*/
     Strings l;				/* string array representation	*/
} IDList;

/* everything we'd like to know about a node */
typedef struct _nodeinfo {
     String file;			/* node's file name		*/
     String node;			/* node's nodename		*/
     int start;				/* starting position		*/
     int length;			/* length of node		*/
     ID name;				/* location of nodename		*/
     ID prev, up, next;			/* locations of prev, up, next	*/
     ID text;				/* location of text		*/
     IDList menu;			/* menu information		*/
     IDList xref;			/* cross references		*/
     struct _nodeinfo *nextNode;	/* for history list		*/
} NodeInfo;

typedef struct {
     /* resources */
     String path;			/* search path			*/
     String file;			/* current info file		*/
     String node;			/* current node name		*/
     String printCmd;			/* lpr command			*/
     int bell_volume;			/* bell volume for error feeps	*/
     Boolean retain_arg;		/* whether or not to save arg	*/
     XtCallbackList callback;		/* quit callback		*/

     /* private state */
     String subFile;			/* current split file (if any)	*/
     NodeInfo *history;			/* the history list		*/
     Table indirect;			/* indirect files		*/
     Table tags;			/* indirect tags		*/
     String data;			/* pointer to file contents	*/
     int size;				/* size of file contents	*/
     int hdrSize;			/* size of file header		*/
     char arg[ARGLEN];			/* command argument string	*/
     Widget fileLabel, nodeLabel;	/* file and node labels		*/
     Widget prevCmd, upCmd, nextCmd;	/* prev, up and next commands	*/
     Widget menuList;			/* menu list			*/
     Widget xrefList;			/* xref list			*/
     Widget nodeText;			/* node text			*/
     Widget statusLabel;		/* status area			*/
     Widget messageLabel;		/* message area			*/
     Widget xrefCmd, gotoCmd, searchCmd;/* xref, goto and search cmds	*/
     Widget argText;			/* xref/goto/search arg text	*/
     Widget helpPopup;			/* help popup			*/
     Widget argPopup;			/* argument popup		*/
     void (*requester)();		/* routine asking for argument	*/
} InfoPart;

typedef struct _InfoRec {
     CorePart		core;
     CompositePart	composite;
     InfoPart		info;
} InfoRec;

/* special accessors for info widget */
#define DATA(iw)		((iw)->info.data)
#define DATASIZE(iw)		((iw)->info.size)
#define HDRSIZE(iw)		((iw)->info.hdrSize)
#define INDIRECT(iw)		((iw)->info.indirect)
#define TAGTABLE(iw)		((iw)->info.tags)
#define HISTORY(iw)		((iw)->info.history)

/* misc */
#define CURNODE(iw)		HISTORY(iw)

/* for search */
#define START(iw)		(DATA(iw))
#define END(iw)			(START(iw) + DATASIZE(iw))
#define NSTART(iw, n)		(START(iw) + (n)->start)
#define NEND(iw, n)		(NSTART(iw, n) + (n)->length)
#define NSEARCH(iw, n, str)	(search(iw,NSTART(iw,n),NEND(iw,n),str,False))

/* for id's */
#define I_NAME(i)		((i).tag.name)
#define I_OFFSET(i)		((i).tag.offset)
#define I_START(i)		((i).offset.start)
#define I_LEN(i)		((i).offset.length)

#endif /* _InfoP_h */
