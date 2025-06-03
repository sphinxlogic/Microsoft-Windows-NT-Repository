/*
 *	$Source: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/xcalendar.h,v $
 *	$Header: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/xcalendar.h,v 1.2 1993/02/17 21:51:48 bingle Exp bingle $
 *      $Author: bingle $
 *      $Locker: bingle $
 *
 * 	Copyright (C) 1988 Massachusetts Institute of Technology	
 *	
 */

/*

   Permission to use, copy, modify, and distribute this
   software and its documentation for any purpose and without
   fee is hereby granted, provided that the above copyright
   notice appear in all copies and that both that copyright
   notice and this permission notice appear in supporting
   documentation, and that the name of M.I.T. not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.
   M.I.T. makes no representations about the suitability of
   this software for any purpose.  It is provided "as is"
   without express or implied warranty.

*/

#ifdef XI18N
#include <X11/Xaw/Xawi18n.h>
#endif

typedef struct _appRes{
   Boolean reverseVideo;
   Boolean setBackground;
   Pixel   markBackground;
   Boolean setForeground;
   Pixel   markForeground;
   char    **months;
   int     firstDay;
   Boolean markOnStartup;
   char    *helpFile;
   int     textBufferSize;
   char	   *calendarDir;
   Boolean oldStyle;
   Boolean markCurrent;
   int     updateCurrent;
   Pixel   currentForeground;
} AppResourcesRec, *AppResources;

typedef struct _list{
   char     *list;
   Cardinal pos,len,size;
} ListRec, *List;


typedef struct _llist{
   caddr_t element;
   caddr_t data;
   struct _llist *prev;
   struct _llist *next;
} LList;

typedef struct _markcolors{
   Pixel     dayBackground,dayForeground;
} MarkColors;

#define DayForeground(p) ((MarkColors *)(p)->data)->dayForeground
#define DayBackground(p) ((MarkColors *)(p)->data)->dayBackground

extern Widget   help_button;

typedef struct _dayeditor {
  Cardinal day,month,year;
  Widget   shell;
  Widget   title;
  Widget   editor;
  XawTextEditType mode;
  Widget   button;		/* button which popped the editor */
  Widget   saveButton;
  Widget   clearEntry;
  Widget   prevday;
  Widget   succday;
  Boolean  open;
  Boolean     saved;
  Boolean     used;
  char     *filename;
  char     *buffer;
  int      bufSize;
} DayEditorRec, *DayEditor;

extern List       CreateList();
extern Cardinal   PushWidgetOnList();
extern Widget     GetWidgetFromList();
extern DayEditor  GetEditorFromList();
extern Cardinal   PushEditorOnList();
extern Cardinal	  PushOnList();
extern Boolean       ReadFile();
extern Boolean       InitEditors();
extern Boolean       initialized;
extern DayEditor  CreateDayEditor();
extern void       MarkDayEntry();
extern void       GetResources();
extern void       ChangeTextSource();
extern void       StartEditor();
extern void	  CurrentTic();
extern void	  CloseEditors();
extern Boolean	  is_today();
extern void	  MakeDayList();
extern int	  current_month();
extern int	  current_day();
extern int	  current_year();
extern int	  FirstDay();
extern int	  NumberOfDays();
extern int	  jan1();
extern int	  write_to_file();
extern int	  read_file();
extern LList	  *pput();
extern LList      *premove();
extern LList	  *lookup();

extern Display    *dsp;
extern Window     markedDaysKey;
extern Widget     toplevel;
extern char       *smonth[];
extern int        debug;
extern LList      *dayEntryList;
extern List       daylist;
extern Cardinal   day,month,year;
extern AppResourcesRec appResources;	/* application specific resources */
extern LList      *allDaysList;

#define ListLength(listp) ((listp)->pos)
#define GetWidgetList(listp) ((WidgetList)(listp)->list)

#define PushEditorOnList(list,editor) PushOnList(list,(caddr_t)&editor)
#define PushWidgetOnList(list,widget) PushOnList(list,(caddr_t)&widget)

#define GetEditorFromList(listp,index) \
  (index > listp->pos || index <= 0)? NULL : *((DayEditor *)listp->list + index -1)
#define GetWidgetFromList(listp,index) \
  (index > listp->pos || index <= 0)? NULL : *((WidgetList)listp->list + index -1)

#define EmptyBuffer(editor) (strlen((editor)->buffer) == 0)
#define TextSize(editor) strlen((editor)->buffer)
