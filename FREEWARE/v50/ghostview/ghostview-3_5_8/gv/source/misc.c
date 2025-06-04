/*
**
** misc.c
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/

/*
 * This code is derived from:
*/

/*
 * misc.c -- Everything that isn't a callback or action.
 * Copyright (C) 1992  Timothy O. Theisen
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
*/

/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef SEEK_SET
#   define SEEK_SET 0
#endif

#include <signal.h>
#ifdef SIGNALRETURNSINT
#   define SIGVAL int
#else
#   define SIGVAL void
#endif

#define GV_MAXLENGTH 512

#ifdef VMS
#   include<descrip.h>
#   include<lnmdef.h>
#   include<lib$routines.h>
#   include<starlet.h>
#   include<rmsdef.h>
#else  
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <unistd.h>
#endif

#include <math.h>

#include "paths.h"
#include INC_X11(Xos.h)
#include INC_X11(Xatom.h)
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include INC_XAW(Cardinals.h)
#include INC_XAW(SimpleMenu.h)
#include INC_XAW(SmeBSB.h)
#include INC_XAW(SmeLine.h)
#include INC_X11(IntrinsicP.h)
#include INC_XAW(TextP.h)
#include INC_XMU(StdCmap.h)
#include "Aaa.h"
#include "Button.h"
#include "Clip.h"
#include "Frame.h"
#include "Ghostview.h"
#include "Vlist.h"

#ifdef VMS
#   include <unixio.h>
#   define unlink remove
#endif

#include "types.h"
#include "actions.h"
#include "callbacks.h"
#include "d_memdebug.h"
#include "file.h"
#include "ps.h"
#include "doc_misc.h"
#include "info.h"
#include "main_resources.h"
#include "main_globals.h"
#include "misc.h"
#include "note.h"
#include "error.h"
#include "save.h"
#include "widgets_misc.h"

#ifndef max
#   define max(a, b)	((a) > (b) ? (a) : (b))
#endif
#ifndef min
#   define min(a, b)	((a) < (b) ? (a) : (b))
#endif

#define UNMAP_CONTROL	(1<<0)
#define UNMAP_PAGEVIEW	(1<<1)
#define UNMAP_PAGE	(1<<2)
#define MAP_CONTROL	(1<<3)
#define MAP_PAGEVIEW	(1<<4)
#define MAP_PAGE	(1<<5)

#if NeedFunctionPrototypes
#   define PT(aaa) aaa
#else 
#   define PT(aaa) ()
#endif
static Boolean set_new_scale        PT(());
static Boolean set_new_orientation  PT((int));
static Boolean set_new_pagemedia    PT((int));
static Widget  build_label_menu     PT((Widget,String,String,Pixmap));
static void    layout_ghostview     PT(());
#undef PT

/*############################################################*/
/* misc_drawEyeGuide */
/*############################################################*/

typedef struct
{
  Widget w;
  int drawn;
  int x,y,width,height;
  XtIntervalId timer;
} EyeGuideDataStruct,*EyeGuideData;

static void 
misc_catchEyeGuideTimer(client_data, idp)
  XtPointer client_data;
  XtIntervalId *idp;
{
  EyeGuideData egd = (EyeGuideData) client_data;

  BEGINMESSAGE(misc_catchEyeGuideTimer)
  if (egd->timer) misc_drawEyeGuide(egd->w,EYEGUIDE_REMOVE,0,0);
  ENDMESSAGE(misc_catchEyeGuideTimer)
}

void 
misc_drawEyeGuide(w,d,x,y)
  Widget w;
  int d;
  int x,y;
{
  unsigned long t = (unsigned long) 1000;
  static EyeGuideData egd = NULL;

  BEGINMESSAGE(misc_drawEyeGuide)
  if (!app_res.scrolling_eye_guide) {
    ENDMESSAGE(misc_drawEyeGuide)
    return;
  }
  if (!egd) {
    egd = (EyeGuideData) GV_XtMalloc(sizeof(EyeGuideDataStruct));
    egd->w = w;
    egd->drawn = 0;
    egd->timer = (XtIntervalId) 0;
  }
  if (d & EYEGUIDE_DRAW) {
    if (egd->timer) XtRemoveTimeOut(egd->timer);
    egd->timer = XtAppAddTimeOut(app_con,t,misc_catchEyeGuideTimer,(XtPointer)egd);
    if (!egd->drawn) {
      int x1,y1,x2,y2,nx,ny;
      Widget clip = XtParent(XtParent(w));
      INFMESSAGE(drawing)
      nx = (int)w->core.x + (int) w->core.border_width;
      ny = (int)w->core.y + (int) w->core.border_width;
      if (-x>nx) x1 = -x - nx; else x1=0;
      if (-y>ny) y1 = -y - ny; else y1=0;
      x2 = x1 + (int)clip->core.width -1; 
      if (-x<nx) x2 -= nx;
      if (x2 > w->core.width) x2 = w->core.width;
      y2 = y1 + (int)clip->core.height - 1;
      if (-y<ny) y2 -= ny;
      if (y2 > w->core.height) y2 = w->core.height;
      if (x1==0) x1 = -1;
      if (y1==0) y1 = -1;
      egd->x = x1;
      egd->y = y1;
      egd->width = x2-x1;
      egd->height = y2-y1;
      GhostviewDrawRectangle(w,egd->x,egd->y,egd->width,egd->height);
      egd->drawn=1;
    }
  }
  if (d & EYEGUIDE_REMOVE) {
    if (egd->drawn) {
      INFMESSAGE(removing)
      GhostviewDrawRectangle(w,egd->x,egd->y,egd->width,egd->height);
    }
  }
  if (d & (EYEGUIDE_REMOVE|EYEGUIDE_RESET)) {
    if (egd->timer) XtRemoveTimeOut(egd->timer);
    GV_XtFree((char*)egd);
    egd = NULL;
  }
  ENDMESSAGE(misc_drawEyeGuide)
}

/*############################################################*/
/* misc_savePagePosition */
/*############################################################*/

static int pagepos_x,pagepos_y,pagepos_saved=0;

void
misc_savePagePosition()
{
  int x,y,psx,psy;
  BEGINMESSAGE(misc_savePagePosition)
  x = ((int)viewClip->core.width)/2  - viewControl->core.x - page->core.x;
  y = ((int)viewClip->core.height)/2 - viewControl->core.y - page->core.y;
  GhostviewCoordsXtoPS(page,x,y,&psx,&psy);
  pagepos_x = psx;
  pagepos_y = psy;
  pagepos_saved = 1;
  ENDMESSAGE(misc_savePagePosition)
}

/*############################################################*/
/* misc_restorePagePosition */
/*############################################################*/

int
misc_restorePagePosition(xP,yP)
  int *xP;
  int *yP;
{
  BEGINMESSAGE(misc_restorePagePosition)
  if (pagepos_saved) {
    *xP = pagepos_x;
    *yP = pagepos_y;
    ENDMESSAGE(misc_restorePagePosition)
    return(1);
  }
  ENDMESSAGE(misc_restorePagePosition)
  return(0);
}

/*############################################################*/
/* misc_restorePagePosition */
/*############################################################*/

void
misc_resetPagePosition()
{
  BEGINMESSAGE(misc_resetPagePosition)
  pagepos_saved = 0;
  ENDMESSAGE(misc_resetPagePosition)
}

/*############################################################*/
/* misc_setPageMarker */
/*############################################################*/

void
misc_setPageMarker(entry,kind)
  int entry;
  int kind; /* 0 = selected, 1 = highlighted , 2 = bring selected in sight*/
{
  int x,y,yl,yu,ny=99999;
  Boolean b = False;
  INFMESSAGE(misc_setPageMarker)
  if (toc_text && (entry >= 0)) {
    if (kind == 0 && VlistSelected(newtoc) != entry)  
      VlistChangeSelected(newtoc,entry,XawVlistSet);
    else if (kind == 1 && VlistHighlighted(newtoc) != entry)  
      VlistChangeHighlighted(newtoc,entry,XawVlistSet);
    else if (kind == 2) {
      entry=VlistSelected(newtoc);
      if (entry<0) return;
    }
    VlistPositionOfEntry(newtoc,entry,&yu,&yl);
    x = newtocControl->core.x;
    y = newtocControl->core.y;
    IIMESSAGE(x,y)
    IIMESSAGE(yu,yl)
    IIMESSAGE(y,newtoc->core.y)
    if (yu != yl) {
      ny = -((int)newtoc->core.y + yu) + 14;
      if (y<0 && y < ny) b = True;
      if (!b) {
	ny = (int)newtocClip->core.height - ((int)newtoc->core.y + yl + 14);
        if (y>ny) b = True;
      }
      if (b) {
	INFIMESSAGE(jumping to,ny)
	ClipWidgetSetCoordinates(newtocClip,x,ny);
      }
    }
  }
}

/*------------------------------------------------------------*/
/* misc_openFile */
/*------------------------------------------------------------*/

static String
misc_openFile(name, fpP)
   String name;
   FILE **fpP;
{
   char *str,*error=NULL;
   FILE *fp=NULL;

   BEGINMESSAGE(misc_openFile)

   if (!name) name = "";
   if (strcmp(name, "-")) {
      INFSMESSAGE(trying to open,name)
      if (file_fileIsNotUseful(name)) {
         size_t l;
         INFMESSAGE(file is not useful)
         str="Invalid file: %s";
         l = strlen(str) + strlen(name) + 1;
         error = GV_XtMalloc(l*sizeof(char));
         sprintf(error,str,name);
      }
      else if ((fp = fopen(name, "r")) == NULL) {
         INFMESSAGE(failed to open)
         INFIMESSAGE(error number,errno)
         error = open_fail_error(errno,GV_ERROR_OPEN_FAIL,name,0);
      }
   }
   if (fpP) *fpP=fp;
   else if (fp) fclose(fp);

   ENDMESSAGE(misc_openFile)
   return(error);
}

/*############################################################*/
/* misc_testFile */
/*############################################################*/

String
misc_testFile(name)
   String name;
{
   char *error;
   BEGINMESSAGE(misc_testFile)
   error = misc_openFile(name,NULL);
   ENDMESSAGE(misc_testFile)
   return(error);
}

/*############################################################*/
/* misc_changeFile */
/*############################################################*/

String misc_changeFile(name)
  String name;
{
  FILE *fp=NULL;
  String error=NULL;
  char *p;
  Boolean b = False;

  BEGINMESSAGE(misc_changeFile)

  if (!name) name="";
  p = GV_XtMalloc((strlen(name)+5)*sizeof(char));
  strcpy(p,name);
  if (strcmp(name,"-")) {
    if (!b && file_fileIsNotUseful(p)) sprintf(p,"%s.ps",name);  else b = True;
    if (!b && file_fileIsNotUseful(p)) sprintf(p,"%s.pdf",name); else b = True;
    if (!b)                            strcpy(p,name);
  }
  name = p;
  INFSMESSAGE(trying to open,name)
  error = misc_openFile(name,&fp);
  if (error) {
    ENDMESSAGE(misc_changeFile)
    GV_XtFree(name);
    return(error);
  }

  if (gv_filename_old) GV_XtFree(gv_filename_old);
  if (gv_filename_raw) GV_XtFree(gv_filename_raw);
  gv_filename_old = gv_filename;
  if (gv_filename_dsc) {
    unlink(gv_filename_dsc);
    GV_XtFree(gv_filename_dsc);
    gv_filename_dsc=NULL;
  }
  if (gv_filename_unc) {
    unlink(gv_filename_unc);
    GV_XtFree(gv_filename_unc);
    gv_filename_unc=NULL;
  }
  if (gv_psfile) fclose(gv_psfile);
  
  gv_filename = GV_XtNewString(name);
  gv_filename_raw = GV_XtNewString(name);
  gv_filename_raw = file_getUsefulName(gv_filename_raw);
  gv_psfile = fp;     
  if (strcmp(name,"-")) {
    struct stat sbuf;
    stat(gv_filename, &sbuf);
    mtime = sbuf.st_mtime;
    INFSMESSAGE(new,gv_filename)
  }
  GV_XtFree(name);
  ENDMESSAGE(misc_changeFile)
  return(error);
}

/*############################################################*/
/* close_file */
/*############################################################*/

String close_file(file,name)
  FILE *file;
  String name;
{
  char *error=NULL;

  BEGINMESSAGE(close_file)
  if (file && fclose(file)!=0) {
    char *error_close_fail     = "Cannot close file %s\n";
    size_t l;
    l = strlen(error_close_fail) + strlen(name) + 1;
    error = GV_XtMalloc(l*sizeof(char));
    sprintf(error,error_close_fail,name);
  }
  ENDMESSAGE(close_file)
  return(error);
}

/*############################################################*/
/* check_file */
/* check if there is a new version of the file */
/* returns -1 if no filename or error in checking file */
/*          0 if no new version exists */
/*          1 if new version exists */
/*############################################################*/

int
check_file(mode)
int mode;
{
   int status=0;
   struct stat sbuf;
   char *tmpname;
   int  r = -1;
#  ifdef VMS
      char *pos;
#  endif

   BEGINMESSAGE(check_file)

   if (!gv_filename) {
      INFMESSAGE(file not useful)
      ENDMESSAGE(check_file)
      return(r);
   }
   if (!strcmp(gv_filename,"-")) {
      INFMESSAGE(reading from stdin; nothing to update)
      ENDMESSAGE(check_file)
      return(0);
   }

#ifdef VMS
   if (mode&CHECK_FILE_DATE) {
#else
   if (1) {
#endif
      INFMESSAGE(checking file date)
      status = stat(gv_filename, &sbuf);
      if (!status && mtime != sbuf.st_mtime) {
         INFMESSAGE(file has changed)
         ENDMESSAGE(check_file)
         return(1);
      }
   }

   tmpname=gv_filename;
   r=status;

#ifdef VMS

   if ( (mode&CHECK_FILE_VERSION) ||
        (status && (mode&CHECK_FILE_DATE) && strrchr(gv_filename,';'))
      ) {
      tmpname = GV_XtNewString(gv_filename);
      pos = strrchr(tmpname,';'); /* strip the version */
      if (pos) *pos='\0';
      status = stat(tmpname, &sbuf);
      if (!status) {
         if (mtime != sbuf.st_mtime) {
            if (pos) { /* get the full specification of the new file*/
               unsigned long s;
               char newfile[256];
               struct dsc$descriptor_s sd;
               struct dsc$descriptor_s fd;
               unsigned long context = 0;       

               sd.dsc$w_length  = (unsigned short)strlen(tmpname);
               sd.dsc$b_dtype   = DSC$K_DTYPE_T;
               sd.dsc$b_class   = DSC$K_CLASS_S;
               sd.dsc$a_pointer = tmpname;
               fd.dsc$w_length  = sizeof(newfile)-1;
               fd.dsc$b_dtype   = DSC$K_DTYPE_T;
               fd.dsc$b_class   = DSC$K_CLASS_S;
               fd.dsc$a_pointer = newfile;
               s=lib$find_file(&sd,&fd,&context,0,0,0,0);
               if (s != RMS$_SUC) {
                  INFMESSAGE(not found) r = -1;
               } else {
                  newfile[fd.dsc$w_length]='\0';
                  pos = strchr(newfile,' ');
                  if (pos) *pos = '\0';
                  INFSMESSAGE(found new file:,newfile)
                  if (gv_filename_old) GV_XtFree(gv_filename_old);
                  gv_filename_old = gv_filename;
                  gv_filename=GV_XtNewString(newfile);
                  INFSMESSAGE(new file:,gv_filename)
                  r = 1;
               }
               lib$find_file_end(&context);
             } else { /* file changed, but filename shows no version number */
                INFSMESSAGE(new file:,gv_filename)
                r = 1;
             }
         } else {
           INFMESSAGE(no new version)
           r = 0;
         }
      } else r=status;
   }
#endif /* VMS */

   if (r<0) {
     char message[GV_MAXLENGTH]; 
     if (r != -2) {
        INFSMESSAGE(cannot access file:,tmpname)
        sprintf(message,"Unable to access file '%s'\n",tmpname);
     } else { /* privilege violation */
        INFSMESSAGE(user not authorized to access file:,tmpname)
        sprintf(message,"User is not authorized to access file '%s'\n",tmpname);
     }
     NotePopupShowMessage(message);
   }
   if (gv_filename!=tmpname) GV_XtFree(tmpname);
   ENDMESSAGE(check_file)
   return(r);
}

/*------------------------------------------------------------*/
/* render_page */
/* Start rendering a new page */
/*------------------------------------------------------------*/

static void
render_page(gvw)
    Widget gvw;
{
    int i;

    BEGINMESSAGE(render_page)

    if (!gv_filename) { INFMESSAGE(no file) ENDMESSAGE(render_page) return; }

    INFIMESSAGE(displaying page,current_page)

    if (toc_text) {
       Boolean processflag;
       Boolean idleflag;
       Boolean noinputflag;
       INFMESSAGE(toc available)
       GhostviewState(gvw,&processflag,&idleflag,&noinputflag);
#      ifdef MESSAGES
          if (processflag) {INFMESSAGE(interpreter running)}
          else             {INFMESSAGE(no interpreter running)}
          if (idleflag)    {INFMESSAGE(widget is idle)}
          else             {INFMESSAGE(widget is busy)}
          if (noinputflag) {INFMESSAGE(interpreter has no input)}
          else             {INFMESSAGE(interpreter has input)}
#      endif
       /* Check first what the state of the ghostview widget is.
          Some documents show additional lines between
          the 'showpage' and the next '%%Page' comment.
          In this case the 'noinputflag' is 'False' but the additional
          lines are not really of significance (at least in no document I have
          encountered).
          So we ignore this flag and start from scratch only if the widget is
          busy or if no interpreter is running.
          Only if 'GV_RESTART_IF_CBUSY' is defined the noinputflag will be
          considered.
       */
#ifdef GV_RESTART_IF_BUSY /* ###jp### added 1.2.95 */
       if (processflag && idleflag && noinputflag) {
#else
       if (processflag && idleflag) {
#endif
          INFMESSAGE(displaying next page)
 	  GhostviewNextPage(gvw);
       } else {
          INFMESSAGE(starting new interpreter)
 	  GhostviewEnableInterpreter(gvw);
	  GhostviewSendPS(gvw, gv_psfile, doc->beginprolog,
			  doc->lenprolog, False);
	  GhostviewSendPS(gvw, gv_psfile, doc->beginsetup,
			  doc->lensetup, False);
       }
       if (doc->pageorder == DESCEND) i = (doc->numpages - 1) - current_page;
       else                           i = current_page;
       GhostviewSendPS(gvw, gv_psfile, doc->pages[i].begin,doc->pages[i].len, False);
    } else {
       INFMESSAGE(no toc available)
       if (!GhostviewIsInterpreterRunning(gvw)) {
          INFMESSAGE(enabling interpreter for unstructured document)
          GhostviewEnableInterpreter(gvw);
       }
       else if (GhostviewIsInterpreterReady(gvw)) {
          INFMESSAGE(displaying page of unstructured document)
          GhostviewNextPage(gvw);
       }
       else {
          INFMESSAGE(interpreter running but not ready)
          XBell(XtDisplay(gvw), 0);
       }
    }

    if (gvw == page) {
       if (toc_text) {
	 if (show_prevPage) {
	   if (current_page == 0) ButtonReset(w_prevPage,NULL,NULL,NULL);
	   XtSetSensitive(w_prevPage, current_page != 0);
	 }
	 if (show_nextPage) {
	   if (current_page == (int)doc->numpages-1) ButtonReset(w_nextPage,NULL,NULL,NULL);
	   XtSetSensitive(w_nextPage, current_page != (int)doc->numpages-1);
	 }
	 XtSetSensitive(prevEntry, current_page != 0);
	 XtSetSensitive(nextEntry, current_page != (int)doc->numpages-1);
       }
       {
         int n = doc ? doc->nummedia : 0;
	 Boolean b = (doc_mediaIsOk(doc,current_page,n) ? True : False);
	 XtSetSensitive(pagemediaEntry[n],b);
       }
       if (toc_text) {
	 INFMESSAGE(marking current_page as current)
         misc_setPageMarker(current_page,0);
       }
    }

    ENDMESSAGE(render_page)
}

/*############################################################*/
/* show_page */
/* This routine is probably the heart of GV */
/* It receives requests from the various callbacks and actions, */
/* maps them onto three flags (need_layout, need_setup, need_render) */
/* and calls the necessary subroutines */
/*############################################################*/

void
show_page(number,data1)
   int number;
   XtPointer data1;
{
   Bool need_layout = False;
   Bool need_setup  = False;
   Bool need_render = False;
   int request=number;
   int ori_request=number;

   BEGINMESSAGE(show_page)
   INFIMESSAGE(received,request)

   if ( /* check if file has changed */
        gv_filename &&
        (request != REQUEST_NEW_FILE) &&
        (request != REQUEST_REOPEN) &&
        (request != REQUEST_TOGGLE_RESIZE) &&
        (request != REQUEST_SETUP) 
      ) {
      int changed = check_file(CHECK_FILE_DATE);
      if (changed==1) {
         INFMESSAGE(file has changed; requesting new file)
         request = REQUEST_NEW_FILE;
	 if (number < NO_CURRENT_PAGE) number = current_page;
      } else if (changed == -1) {
         INFMESSAGE(file is not accessible)
         ENDMESSAGE(show_page)
         return;
      }
   }

   if (!toc_text && (request==REQUEST_REDISPLAY)) {
      INFMESSAGE(request to redisplay non DSC file; changing to request for new file)
      request=REQUEST_NEW_FILE;
   }

   if (request >= NO_CURRENT_PAGE) {
      INFMESSAGE(request for new page)
      if (GhostviewIsBusy(page)) {
         INFMESSAGE(busy state)
         if (toc_text) {
            number = doc_putPageInRange(doc,number);
            gv_pending_page_request=number;
            INFIMESSAGE(will remember,gv_pending_page_request)
         }
         ENDMESSAGE(show_page)
         return;
      }
      need_layout = need_setup = 
          set_new_orientation(number)|set_new_pagemedia(number);
      need_render = True;
   } else if (request<NO_CURRENT_PAGE) {
      INFIMESSAGE(analyzing,request)
      switch (request) {
      case REQUEST_TOGGLE_RESIZE:
		INFMESSAGE(### request for change of resize behaviour)
		number=current_page;
		need_layout = True;
		need_setup  = False;
		need_render = False;
		break;
      case REQUEST_REDISPLAY:
		INFMESSAGE(### request for redisplay)
		number=current_page;
		need_layout = False;
		need_setup  = False;
		need_render = True;
		break;
      case REQUEST_SETUP:
		INFMESSAGE(### request for setup)
		number=current_page;
		need_layout =	set_new_scale()
				|set_new_orientation(number)
				|set_new_pagemedia(number);
                need_setup  = True;
		need_render = True;
		break;
      case REQUEST_NEW_SCALE:
		INFMESSAGE(### request for new scale)
		number=current_page;
		need_layout = need_setup = need_render = 
				set_new_scale();
                if (!need_layout) {ENDMESSAGE(show_page) return;}
                break;
      case REQUEST_NEW_PAGEMEDIA:
		INFMESSAGE(### request for new pagemedia)
		number=current_page;
		need_layout = need_setup = need_render =
				set_new_pagemedia(number);
                if (!need_layout) {ENDMESSAGE(show_page) return;}
                break;
      case REQUEST_NEW_ORIENTATION:
		INFMESSAGE(### request for new orientation)
		number=current_page;
		need_layout = need_setup = need_render =
				set_new_orientation(number);
                if (!need_layout) {ENDMESSAGE(show_page) return;}
		break;
      case REQUEST_OPTION_CHANGE:
        {
		INFMESSAGE(### request by options menu)
		cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
		number=current_page;
		number = doc_putPageInRange(doc,number);
		need_setup = need_layout = need_render =
		                set_new_orientation(number) |
		                set_new_pagemedia(number)   |
		                set_new_scale();
		        }
		break;
      case REQUEST_REOPEN:
      case REQUEST_NEW_FILE:
	{
                String filename;
                String error = NULL;
                if (data1) filename = (String) data1;
                else       filename = gv_filename;
		INFMESSAGE(### request to open or reopen file)
                error = misc_changeFile(filename);
                if (error) {
		   NotePopupShowMessage(error);
		   GV_XtFree(error);
 		   ENDMESSAGE(show_page)
                   return;
                }
		if (request==REQUEST_REOPEN) {
		   INFMESSAGE(request to reopen file)
		   number=current_page;
		}
		need_layout = setup_ghostview();
		number = doc_putPageInRange(doc,number);
		need_layout = need_layout
				|set_new_orientation(number)
				|set_new_pagemedia(number)
				|set_new_scale();
                need_setup  = True;
                need_render = True;
		break;
	}
      default:
		INFMESSAGE(### unknown request)
		fprintf(stderr,"  %s: Unknown request in show_page\n",gv_name);
		ENDMESSAGE(show_page)
		return;
      }
   }

   if (!gv_psfile && need_render) {
      INFMESSAGE(no gv_psfile; forcing setup and layout)
      need_setup=True;
      need_layout=True;
   }

#  ifdef MESSAGES
      if (need_layout) {INFMESSAGE(### need layout)} else {INFMESSAGE(### do not layout)}
      if (need_setup)  {INFMESSAGE(### need setup)}  else {INFMESSAGE(### do not setup)}
      if (need_render) {INFMESSAGE(### need render)} else {INFMESSAGE(### do not render)}
#  endif

   if (need_layout) layout_ghostview();

   if (need_setup)  GhostviewSetup(page);
   if (!gv_filename) {
      need_render=False;
      INFMESSAGE(no filename; forcing no render)
   }
   if (toc_text) {
      number = doc_putPageInRange(doc,number);
      current_page = number;
   }
   if (need_render) {
     render_page(page);
     misc_drawEyeGuide(page,EYEGUIDE_RESET,0,0);
     if (ori_request == REQUEST_NEW_FILE && !need_layout)
       cb_positionPage(page,(XtPointer)NULL,(XtPointer)NULL);
   }

   gv_pending_page_request=NO_CURRENT_PAGE; /* eliminate any pending requests now */

   ENDMESSAGE(show_page)
}

/*############################################################*/
/* setup_ghostview */
/* This includes:
 *  scanning the PostScript file,
 *  setting the title and date labels,
 *  building the pagemedia menu,
 *  building the toc (table of contents)
 *  sensitizing the appropriate menu buttons,
 *  popping down and erasing the infotext popup.
 */
/*############################################################*/

static void misc_setSensitive(w,s,b)
  Widget w;
  Boolean s;
  Boolean b;
{
  if (s) {
    if (!b) ButtonReset(w,NULL,NULL,NULL);
    XtSetSensitive(w,b);
  }
}

static void misc_setBitmap(w,s,b)
  Widget w;
  Boolean s;
  Pixmap b;
{
  Arg args[1];
  if (s) {
    if (b != None) {
      XtSetArg(args[0],XtNbitmap,(toc_text ? b :app_res.mark_empty_bitmap));
      XtSetValues(w,args,(Cardinal)1);
    }
  }
}

Boolean
setup_ghostview()
{
    Arg args[10];
    Cardinal n;
    int oldtoc_entry_length;
    int toc_length;
    char *tocp;
    Pixmap bitmap;
    String label,buttonlabel;

    BEGINMESSAGE(setup_ghostview)
    /* Reset to a known state. */
    psfree(olddoc);
    olddoc = doc;
    doc = NULL;
    current_page = NO_CURRENT_PAGE;
    if (toc_text) GV_XtFree(toc_text);
    oldtoc_entry_length = toc_entry_length;
    toc_text = NULL;

    INFMESSAGE(scanning file for structure information)
    gv_filename_dsc = gv_filename_unc = NULL;
    doc_scanFile( &gv_psfile,&doc,
		  gv_filename,
		  gv_filename_raw,
		  &gv_filename_dsc,gv_gs_cmd_scan_pdf,
		  &gv_filename_unc,gv_uncompress_command,
		  gv_scanstyle);
    {
      int m;
      m = gv_pagemedia;
      if (olddoc && olddoc->nummedia && m >= 0) {
	if (m >= olddoc->nummedia) m = m - olddoc->nummedia;
	else                       m = MEDIA_ID_INVALID;
      }
      if (doc && doc->nummedia && m >= 0) {
         m = m + doc->nummedia;
      }
      if (m != gv_pagemedia) gv_pagemedia = gv_pagemedia_old = m;
    }
    if (gv_pagemedia == MEDIA_ID_INVALID) {
       int m;
       gv_pagemedia_old         = MEDIA_ID_INVALID;
       m = doc_convStringToPageMedia(doc,app_res.default_pagemedia);
       if (m== MEDIA_ID_AUTO) {
          gv_pagemedia_auto     = 1;
          gv_pagemedia_auto_old = 0;
          gv_pagemedia          = MEDIA_ID_INVALID;
       } else {
          gv_pagemedia_auto     = 0;
          gv_pagemedia_auto_old = 1;
          gv_pagemedia          = m;
       }
    }
    cb_showTitle(NULL,NULL,NULL);
    if (show_title) {
       if (doc && doc->title) {
          buttonlabel = doc->title;
          label = doc->title;
          bitmap = app_res.document_bitmap;
       } 
       else if (gv_filename) {
#        ifdef VMS
           buttonlabel = strrchr(gv_filename,']');
	   if (!buttonlabel) buttonlabel = strrchr(gv_filename,':');
	   if (buttonlabel) buttonlabel++;
	   else buttonlabel = gv_filename;
#        else
	   buttonlabel = gv_filename;
#        endif
	   label = gv_filename;
	   bitmap = None;
       } else {
	 buttonlabel = ""; label = "";
	 bitmap = None;
       }
                                                  n=0;
       XtSetArg(args[n], XtNlabel, buttonlabel);  n++;
       XtSetValues(titlebutton, args, n);  
       if (titlemenu) XtDestroyWidget(titlemenu); 
       titlemenu = build_label_menu(titlebutton, "title", label, bitmap);
    }

    if (show_date) {
       if (doc && doc->date) {
          label = doc->date;
          bitmap = app_res.document_bitmap;
       } 
       else {
          if (gv_psfile) { label = ctime(&mtime); } 
          else { label = ""; }
          bitmap = None;
       }

                                                  n=0;
       XtSetArg(args[n], XtNlabel, label);        n++;
       XtSetValues(datebutton, args, n);
       if (datemenu) XtDestroyWidget(datemenu);
       datemenu = build_label_menu(datebutton, "date", label, bitmap);   
    }

    misc_buildPagemediaMenu();
    {
      int media_bbox = doc ? doc->nummedia : 0;
      Boolean b = (doc_mediaIsOk(doc,current_page,media_bbox) ? True : False);
      XtSetSensitive(pagemediaEntry[media_bbox], b);
    }

    /* Reset ghostscript and output messages popup */
#   ifdef VMS
    {
       Bool disable=False;
       if ( !doc || !olddoc ||
	    olddoc->beginprolog != doc->beginprolog ||
	    olddoc->endprolog != doc->endprolog ||
	    olddoc->beginsetup != doc->beginsetup ||
	    olddoc->endsetup != doc->endsetup
       ) disable=True;
       if (!disable) {
          char *fn = GV_XtNewString(gv_filename);
          char *ofn= GV_XtNewString(gv_filename_old);
          char *pos;
          pos = strrchr(fn,';');  if (pos) *pos='\0';
          pos = strrchr(ofn,';'); if (pos) *pos='\0';
          if (strcmp(fn, ofn)) disable=True;
          GV_XtFree(fn);
          GV_XtFree(ofn);
       }
       if (disable==True) {   
	  GhostviewDisableInterpreter(page);
          cb_popdownInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
          cb_resetInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
       }
    }
#   else
    if (!doc || !olddoc ||
	strcmp(gv_filename_old, gv_filename) ||
	olddoc->beginprolog != doc->beginprolog ||
	olddoc->endprolog != doc->endprolog ||
	olddoc->beginsetup != doc->beginsetup ||
	olddoc->endsetup != doc->endsetup) {
        INFMESSAGE(disabling interpreter)
	GhostviewDisableInterpreter(page);
        cb_popdownInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
        cb_resetInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)NULL);
    }
#   endif

    /* Build table of contents */
    if (doc && doc->structured) {
	int maxlen = 0;
	int i, j;

        INFMESSAGE(toc available)
	if (doc->labels_useful) {
	    for (i = 0; i < doc->numpages; i++) 
		maxlen = max(maxlen, strlen(doc->pages[i].label));
	} else {
	    double x;
	    x = doc->numpages;
	    maxlen = log10(x) + 1;
	}
	toc_entry_length = maxlen + 1;
	toc_length = doc->numpages * toc_entry_length - 1;
	toc_text = GV_XtMalloc(toc_length + 2); /* include final NULL */

	for (i = 0, tocp = toc_text; i < doc->numpages;
	     i++, tocp += toc_entry_length) {
	    if (doc->labels_useful) {
		if (doc->pageorder == DESCEND) {
		    j = (doc->numpages - 1) - i;
		} else {
		    j = i;
		}
		sprintf(tocp, "%*s\n", maxlen, doc->pages[j].label);
	    } else {
		sprintf(tocp, "%*d\n", maxlen, i+1);
	    }
	}
	toc_text[toc_length] = '\0';
							n=0;
	XtSetArg(args[n], XtNfilename, NULL);      	n++;
	XtSetValues(page, args, n);
    } else {
        String fn;
        fn = gv_filename_unc ? gv_filename_unc : gv_filename;
        INFMESSAGE(toc not available)
	toc_length = 0;
	toc_entry_length = 1;
				       	        n=0;
	XtSetArg(args[n], XtNfilename, fn);     n++;
	XtSetValues(page, args, n);
    }

    {
      String s;
      int i=0;
      if (toc_text) {
	s = (char*)GV_XtMalloc((doc->numpages+1)*sizeof(char));
	while (i < (int)doc->numpages) {
	  s[i] = 'p';
	  i++;
	}
	s[i] = '\0';
							n=0;
	XtSetArg(args[n], XtNvlist, s);			n++;
	XtSetArg(args[n], XtNlabel, toc_text);		n++;
      } else {
	s = NULL;
	XtSetArg(args[n], XtNvlist, "");		n++;
	XtSetArg(args[n], XtNlabel, "");		n++;
      }
      XtSetValues(newtoc, args, n);
      ClipWidgetSetCoordinates(newtocClip,0,0);
      GV_XtFree(s);
    }

    misc_setBitmap(w_toggleCurrentPage , show_toggleCurrentPage , app_res.mark_current_bitmap);
    misc_setBitmap(w_toggleEvenPages   , show_toggleEvenPages   , app_res.mark_even_bitmap);
    misc_setBitmap(w_toggleOddPages    , show_toggleOddPages    , app_res.mark_odd_bitmap);
    misc_setBitmap(w_unmarkAllPages    , show_unmarkAllPages    , app_res.mark_unmark_bitmap);

    misc_setSensitive(w_saveMarkedPages   , show_saveMarkedPages   , (toc_text    != NULL));
    misc_setSensitive(w_saveAllPages      , show_saveAllPages      , (gv_psfile   != NULL));
    misc_setSensitive(w_printMarkedPages  , show_printMarkedPages  , (toc_text    != NULL));
    misc_setSensitive(w_printAllPages     , show_printAllPages     , (gv_psfile   != NULL));
    misc_setSensitive(w_checkFile         , show_checkFile         , (gv_filename != NULL));
    misc_setSensitive(w_updateFile        , show_updateFile        , (gv_filename != NULL));
    misc_setSensitive(w_showThisPage      , show_showThisPage      , (gv_filename != NULL));
    misc_setSensitive(w_prevPage          , show_prevPage          , (toc_text    != NULL));
    misc_setSensitive(w_nextPage          , show_nextPage          , (gv_filename != NULL));
    misc_setSensitive(w_toggleCurrentPage , show_toggleCurrentPage , (toc_text    != NULL));
    misc_setSensitive(w_toggleEvenPages   , show_toggleEvenPages   , (toc_text    != NULL));
    misc_setSensitive(w_toggleOddPages    , show_toggleOddPages    , (toc_text    != NULL));
    misc_setSensitive(w_unmarkAllPages    , show_unmarkAllPages    , (toc_text    != NULL));

    XtSetSensitive(reopenEntry,      (gv_psfile   != NULL));
    XtSetSensitive(printAllEntry,    (gv_psfile   != NULL));
    XtSetSensitive(printMarkedEntry, (toc_text    != NULL));
    XtSetSensitive(saveAllEntry,     (gv_psfile   != NULL));
    XtSetSensitive(saveMarkedEntry,  (toc_text    != NULL));
    XtSetSensitive(nextEntry,        (gv_filename != NULL));
    XtSetSensitive(redisplayEntry,   (gv_filename != NULL));
    XtSetSensitive(prevEntry,        (toc_text    != NULL));
    XtSetSensitive(currentEntry,     (toc_text    != NULL));
    XtSetSensitive(oddEntry,         (toc_text    != NULL));
    XtSetSensitive(evenEntry,        (toc_text    != NULL));
    XtSetSensitive(unmarkEntry,      (toc_text    != NULL));

    ENDMESSAGE(setup_ghostview)
    return oldtoc_entry_length != toc_entry_length;
}

Dimension view_width, view_height, view_border;
Dimension control_width, control_height;
Dimension page_width, page_height;

/*------------------------------------------------------------*/
/* layout_ghostview */
/*------------------------------------------------------------*/

static void
layout_ghostview()
{
   Arg       args[10];
   Cardinal  n;
   Dimension page_prefWidth, page_prefHeight;
   Dimension page_width, page_height;
   static Boolean firsttime=True;
   Boolean auto_resize;

   BEGINMESSAGE(layout_ghostview)

   if (!firsttime) {
      XtSetArg(args[0], XtNallowShellResize,&auto_resize);
      XtGetValues(toplevel, args,ONE);
      if (auto_resize != app_res.auto_resize) {
         INFMESSAGE(######## changing resize behaviour)
#        ifdef MESSAGES
            if (app_res.auto_resize) {INFMESSAGE(shell is allowed to resize)}
            else                     {INFMESSAGE(shell must not resize)}
#        endif
         XtSetArg(args[0], XtNallowShellResize,app_res.auto_resize);
         XtSetValues(toplevel, args,ONE);
	 if (app_res.auto_resize==False) {
	   ENDMESSAGE(layout_ghostview)
	   return;
         }
	 INFIMESSAGE(setting tocFrame height:,TOC3D_INITIAL_HEIGHT)
         XtSetArg(args[0], XtNheight,TOC3D_INITIAL_HEIGHT);
	 XtSetValues(newtocFrame, args, ONE);
      }
   }

   INFMESSAGE(#### retrieving dimensions)
   XtSetArg(args[0], XtNpreferredWidth, &page_prefWidth);
   XtSetArg(args[1], XtNpreferredHeight, &page_prefHeight);
   XtSetArg(args[2], XtNwidth, &page_width);
   XtSetArg(args[3], XtNheight, &page_height);
   XtGetValues(page, args, FOUR);
   INFIIMESSAGE(## preferred,page_prefWidth,page_prefHeight)
   INFIIMESSAGE(## actual,page_width,page_height)

   if (page_prefWidth != page_width || page_prefHeight != page_height) {
      INFMESSAGE(#### setting ghostview widget size to its preferred size)
      XtSetArg(args[0], XtNwidth,           page_prefWidth);
      XtSetArg(args[1], XtNheight,          page_prefHeight);
      XtSetValues(page, args, TWO);
   }
   cb_positionPage(page,(XtPointer)NULL,(XtPointer)NULL);

   if (firsttime) {
						            n=0;
     XtSetArg(args[n], XtNminWidth, (Dimension)app_res.minimum_width);  n++;
     XtSetArg(args[n], XtNminHeight,(Dimension)app_res.minimum_height);n++;
     if (app_res.auto_resize==False) {
        INFMESSAGE(switching to No-Resize mode)
        XtSetArg(args[n], XtNallowShellResize,app_res.auto_resize); n++;
     }
     XtSetValues(toplevel, args,n);
     firsttime=False;
   }
 
  ENDMESSAGE(layout_ghostview)
}

/*############################################################*/
/* setup_layout_ghostview */
/*############################################################*/

void
setup_layout_ghostview()
{
  BEGINMESSAGE(setup_layout_ghostview )
  ENDMESSAGE(setup_layout_ghostview)
}

/*------------------------------------------------------------*/
/* set_new_scale */
/*------------------------------------------------------------*/

static Boolean
set_new_scale()
{
  int new_scale,new_scale_base;
  Boolean changed = False;
  Arg args[2];
  Cardinal n;
  Scale scale;

  BEGINMESSAGE(set_new_scale)

  new_scale_base = gv_scale_base;
  if (!default_xdpi || !default_ydpi || new_scale_base != gv_scale_base_current) {
    scale = gv_scales[new_scale_base];
    if ((scale->is_base)&SCALE_IS_REAL_BASED) {
      default_xdpi = gv_real_xdpi; 
      default_ydpi = gv_real_ydpi;
    } else {
      default_xdpi = gv_pixel_xdpi; 
      default_ydpi = gv_pixel_ydpi;
    }
    default_xdpi *= scale->scale;
    default_ydpi *= scale->scale;
    XtSetArg(args[0], XtNleftBitmap, None);
    if (gv_scale_base_current >=0) XtSetValues(scaleEntry[gv_scale_base_current],args, ONE);
    XtSetArg(args[0], XtNleftBitmap, app_res.selected_bitmap);
    XtSetValues(scaleEntry[new_scale_base],args, ONE);
    gv_scale_base_current = new_scale_base;
    changed=True;
  }
  
  new_scale = gv_scale;
  if (changed || new_scale != gv_scale_current) {
    float xdpi, ydpi;
    GhostviewDisableInterpreter(page);
    scale = gv_scales[new_scale];
    xdpi = default_xdpi / scale->scale;
    ydpi = default_ydpi / scale->scale;
                                              n=0;
    XtSetArg(args[n], XtNlabel, scale->name); n++;
    XtSetValues(scaleButton, args, n);
			            	      n=0;
    XtSetArg(args[n], XtNlxdpi, (1000*xdpi)); n++;
    XtSetArg(args[n], XtNlydpi, (1000*ydpi)); n++;
    XtSetValues(page, args, n);
			            	      n=0;
    XtSetArg(args[n], XtNleftBitmap, None);   n++;
    if (gv_scale_current >=0) XtSetValues(scaleEntry[gv_scale_current],args, n);
			            	      n=0;
    XtSetArg(args[n], XtNleftBitmap, app_res.selected_bitmap); n++;
    XtSetValues(scaleEntry[new_scale],args, n);
    gv_scale_current = new_scale;
    changed=True;
  }
  ENDMESSAGE(set_new_scale)
  return changed;
}

/*------------------------------------------------------------*/
/* set_orientationButton_label */
/*------------------------------------------------------------*/

static void
set_orientationButton_label(orientation)
   int orientation;
{
   Arg args[1];
   Widget w = portraitEntry;
   String label;
    
   BEGINMESSAGE(set_orientationButton_label)
   if (orientation == O_LANDSCAPE)       w = landscapeEntry;
   else if (orientation == O_UPSIDEDOWN) w = upsidedownEntry;
   else if (orientation == O_SEASCAPE)   w = seascapeEntry;
   XtSetArg(args[0], XtNlabel,&label);
   XtGetValues(w, args, ONE);
   XtSetArg(args[0], XtNlabel,label);
   XtSetValues(orientationButton, args, ONE);
   ENDMESSAGE(set_orientationButton_label)
}

/*------------------------------------------------------------*/
/* set_newBitmapIfChanged */
/*------------------------------------------------------------*/

static void 
set_newBitmapIfChanged(w,new_bitmap)
   Widget w;
   Pixmap new_bitmap;
{
   Arg args[1];
   Pixmap old_bitmap;

   BEGINMESSAGE(set_newBitmapIfChanged)
   XtSetArg(args[0], XtNleftBitmap, &old_bitmap);
   XtGetValues(w, args, ONE);
   if (new_bitmap != old_bitmap) {
      XtSetArg(args[0], XtNleftBitmap, new_bitmap);
      XtSetValues(w, args, ONE);
   }
   ENDMESSAGE(set_newBitmapIfChanged)
}

/*------------------------------------------------------------*/
/* set_new_orientation */
/*------------------------------------------------------------*/

static Boolean
set_new_orientation(pagenumber)
   int pagenumber;
{
   Boolean changed  = False;
   int from_doc = 0;
   int no;
   Widget w;
   Pixmap bitmap;
   XtPageOrientation xto,xto_old;

   BEGINMESSAGE(set_new_orientation)

   no = O_UNSPECIFIED;
   if (no == O_UNSPECIFIED && gv_orientation != gv_orientation_old) {
      INFIMESSAGE(forcing new orientation to be,no)
      no = gv_orientation;
      INFMESSAGE(disabling automatic orientation)
      gv_orientation_auto = 0;
   }
   if (no == O_UNSPECIFIED && gv_orientation_auto) {
      int po;
      po = doc_preferredOrientationOfPage(doc,pagenumber);
      INFIMESSAGE(using orientation from doc, po)
      if (po != O_UNSPECIFIED) {
         INFIMESSAGE(using orientation from doc, po)
         no = po;
         from_doc = 1;
      }
   }
   if (no==O_UNSPECIFIED) no = gv_orientation_old;
   if (no!=O_PORTRAIT && no!=O_LANDSCAPE && no!=O_SEASCAPE && no!=O_UPSIDEDOWN)
      no = gv_fallback_orientation;
   gv_orientation = no;

   xto     = doc_convDocOrientToXtOrient(gv_orientation,    gv_swap_landscape    );
   xto_old = doc_convDocOrientToXtOrient(gv_orientation_old,gv_swap_landscape_old);
   IIMESSAGE(xto,xto_old)

   if (xto != xto_old) {
      Arg args[1];
      if      (gv_orientation_old == O_PORTRAIT)   w = portraitEntry;
      else if (gv_orientation_old == O_LANDSCAPE)  w = landscapeEntry;
      else if (gv_orientation_old == O_UPSIDEDOWN) w = upsidedownEntry;
      else                                         w = seascapeEntry;
      widgets_setSelectedBitmap(w,0);

      INFIMESSAGE(changing orientation for page to be,xto)
      GhostviewDisableInterpreter(page);
      XtSetArg(args[0], XtNorientation, xto);
      XtSetValues(page, args, ONE);
      changed = True;
      set_orientationButton_label(gv_orientation);
   }

   if (from_doc) bitmap = app_res.document_bitmap;
   else          bitmap = app_res.selected_bitmap;
   if      ( no == O_PORTRAIT)   w = portraitEntry;
   else if ( no == O_LANDSCAPE)  w = landscapeEntry;
   else if ( no == O_UPSIDEDOWN) w = upsidedownEntry;
   else                          w = seascapeEntry;
   set_newBitmapIfChanged(w,bitmap);

   if (gv_swap_landscape != gv_swap_landscape_old)
      widgets_setSelectedBitmap(swapEntry,gv_swap_landscape);
   if (gv_orientation_auto != gv_orientation_auto_old)
      widgets_setSelectedBitmap(autoOrientEntry,gv_orientation_auto);

   gv_orientation_old       = gv_orientation;
   gv_orientation_auto_old  = gv_orientation_auto;
   gv_swap_landscape_old    = gv_swap_landscape;

   ENDMESSAGE(set_new_orientation)
   return(changed);
   
}

/*------------------------------------------------------------*/
/* set_pagemediaButton */
/*------------------------------------------------------------*/

static void
set_pagemediaButton_label(media_id)
   int media_id;
{ 
   String s = NULL;
   Arg args[1];

   BEGINMESSAGE(set_pagemediaButton_label)
   if (media_id>=0) {
      Widget w;
      if (pagemediaEntry[media_id]) w = pagemediaEntry[media_id];
      else                          w = pagemediaEntry[media_id-1];
      XtSetArg(args[0], XtNlabel, &s);
      XtGetValues(w, args, ONE);
   } 
   else s = "?";
   XtSetArg(args[0], XtNlabel, s);
   XtSetValues(pagemediaButton, args, ONE);          
   ENDMESSAGE(set_pagemediaButton_label)
}

/*------------------------------------------------------------*/
/* set_new_pagemedia */
/*------------------------------------------------------------*/

static Boolean
set_new_pagemedia(pagenumber)
   int pagenumber;
{
   int new_llx,new_lly,new_urx,new_ury;
   Boolean changed = False;
   int from_doc = 0;
   Arg args[4];
   Widget w = NULL; 
   Pixmap bitmap;
   int num_doc_media;
   int nm;
   int media_bbox;

   BEGINMESSAGE(set_new_pagemedia)

   num_doc_media=0;
   if (doc) num_doc_media = doc->nummedia;
   media_bbox = num_doc_media;

   nm = MEDIA_ID_INVALID;
   if (gv_pagemedia != gv_pagemedia_old) {
     if (doc_mediaIsOk(doc,pagenumber,gv_pagemedia)) {
       nm = gv_pagemedia;
       INFIMESSAGE(forcing new pagemedia to be,nm)
       INFMESSAGE(disabling automatic pagemedia)
       gv_pagemedia_auto = 0;
     }
   } 
   else if (gv_pagemedia==media_bbox && !gv_pagemedia_auto) {
     if (doc_mediaIsOk(doc,pagenumber,gv_pagemedia))
       nm = gv_pagemedia;
   }
   else if (gv_pagemedia_auto) {
     nm = doc_preferredMediaOfPage(doc,pagenumber,&new_llx,&new_lly,&new_urx,&new_ury);
     if (nm != MEDIA_ID_INVALID) {
       INFIMESSAGE(using pagemedia preferred from doc, nm)
       from_doc = 1;
     }
   }
   if (nm==MEDIA_ID_INVALID && doc_mediaIsOk(doc,pagenumber,gv_pagemedia_old))
     nm = gv_pagemedia_old;
   if (nm==MEDIA_ID_INVALID) nm = gv_fallback_pagemedia;
   gv_pagemedia = nm;

   /* If pagemedia changed, remove the old marker. */
   IIMESSAGE(gv_pagemedia,gv_pagemedia_old)
   if (gv_pagemedia != gv_pagemedia_old) {
      if (gv_pagemedia_old>=0) {
         if (pagemediaEntry[gv_pagemedia_old]) w = pagemediaEntry[gv_pagemedia_old];
         else                                  w = pagemediaEntry[gv_pagemedia_old-1];
         widgets_setSelectedBitmap(w,0);
      }
      set_pagemediaButton_label(gv_pagemedia);
   }

   if (gv_pagemedia >= 0) {
      if (from_doc) bitmap = app_res.document_bitmap;
      else          bitmap = app_res.selected_bitmap;
      if (pagemediaEntry[gv_pagemedia]) w = pagemediaEntry[gv_pagemedia];
      else                              w = pagemediaEntry[gv_pagemedia-1];
      set_newBitmapIfChanged(w,bitmap);
   }

   if (gv_pagemedia_auto != gv_pagemedia_auto_old) widgets_setSelectedBitmap(autoMediaEntry,gv_pagemedia_auto);
 
   if (gv_pagemedia == num_doc_media) {
      doc_boundingBoxOfPage(doc,pagenumber,&new_llx,&new_lly,&new_urx,&new_ury);
   } else {
      new_llx = new_lly = 0;
      if (gv_pagemedia < num_doc_media) {
         new_urx = doc->media[gv_pagemedia].width-1;
	 new_ury = doc->media[gv_pagemedia].height-1;
      } else {
         new_urx = gv_medias[gv_pagemedia-num_doc_media]->width-1;
         new_ury = gv_medias[gv_pagemedia-num_doc_media]->height-1;
      }
   }

   /* If bounding box changed, setup for new size. */
   if ((new_llx != current_llx) || (new_lly != current_lly) ||
      (new_urx != current_urx) || (new_ury != current_ury)) {
      INFMESSAGE(bounding box changed)
      INFIIMESSAGE(lower left:,new_llx,new_lly)
      INFIIMESSAGE(upper right:,new_urx,new_ury)
      GhostviewDisableInterpreter(page);
      changed = True;
      current_llx = new_llx;
      current_lly = new_lly;
      current_urx = new_urx;
      current_ury = new_ury;
      XtSetArg(args[0], XtNllx, current_llx);
      XtSetArg(args[1], XtNlly, current_lly);
      XtSetArg(args[2], XtNurx, current_urx);
      XtSetArg(args[3], XtNury, current_ury);
      XtSetValues(page, args, FOUR);
   }

   gv_pagemedia_old = gv_pagemedia;
   gv_pagemedia_auto_old = gv_pagemedia_auto;

   ENDMESSAGE(set_new_pagemedia)
   return changed;
}

/*------------------------------------------------------------*/
/* same_document_media */
/*------------------------------------------------------------*/

static Boolean
same_document_media()
{
   int i;
   Boolean same = True;

   BEGINMESSAGE(same_document_media)
   if (olddoc == NULL && doc == NULL)          same=True;
   else if (olddoc == NULL || doc == NULL)     same=False;
   else if (olddoc->nummedia != doc->nummedia) same=False;
   else for (i = 0; i < doc->nummedia; i++) {
       if (strcmp(olddoc->media[i].name, doc->media[i].name)) {
          same=False;
          break;
       }
   }
   ENDMESSAGE(same_document_media)
   return(same);
}

/*############################################################*/
/* misc_buildPagemediaMenu */
/*############################################################*/

void misc_buildPagemediaMenu()
{
  Widget w;
  int i,num_doc_media;

  BEGINMESSAGE(misc_buildPagemediaMenu)
  if (pagemediaMenu && same_document_media()) {
    ENDMESSAGE(misc_buildPagemediaMenu)
    return;
  }
  if (pagemediaMenu) XtDestroyWidget(pagemediaMenu);
  pagemediaMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,pagemediaButton, NULL,(Cardinal)0);

  autoMediaEntry = XtCreateManagedWidget("automatic",smeBSBObjectClass,pagemediaMenu,NULL,(Cardinal)0);
  XtAddCallback(autoMediaEntry,XtNcallback,cb_setPagemedia,(XtPointer)MEDIA_ID_AUTO);
  widgets_setSelectedBitmap(autoMediaEntry,gv_pagemedia_auto);
  XtCreateManagedWidget("line",smeLineObjectClass,pagemediaMenu,NULL,(Cardinal)0);

  /* Build the Page Media menu */
  /* the Page media menu has three parts.
   *  - the automatic media detection entry.
   *  - the document defined page medias
   *  - the standard page medias
   */
  num_doc_media = 0;
  if (doc) num_doc_media = doc->nummedia;
  
  i = gv_num_std_pagemedia + num_doc_media;
  if (pagemediaEntry) GV_XtFree(pagemediaEntry);
  pagemediaEntry = (Widget *) GV_XtMalloc(i * sizeof(Widget));
  
  if (doc && doc->nummedia) {
    for (i = 0; i < doc->nummedia; i++) {
      pagemediaEntry[i] = XtCreateManagedWidget(doc->media[i].name,smeBSBObjectClass, pagemediaMenu,NULL,(Cardinal)0);
      XtAddCallback(pagemediaEntry[i], XtNcallback,cb_setPagemedia, (XtPointer)i);
    }
    w = XtCreateManagedWidget("line", smeLineObjectClass, pagemediaMenu,NULL,(Cardinal)0);
  }
  
  for (i = 0; gv_medias[i]; i++) {
    pagemediaEntry[i+num_doc_media] = NULL;
    if (!(gv_medias[i]->used)) continue;
    pagemediaEntry[i+num_doc_media] = 
      XtCreateManagedWidget(gv_medias[i]->name,smeBSBObjectClass, pagemediaMenu,NULL,(Cardinal)0);
    XtAddCallback(pagemediaEntry[i+num_doc_media], XtNcallback,cb_setPagemedia, (XtPointer)(i+num_doc_media));
  }
  {
    Boolean b = (doc_mediaIsOk(doc,current_page,num_doc_media) ? True : False);
    XtSetSensitive(pagemediaEntry[num_doc_media],b);
  }

  ENDMESSAGE(misc_buildPagemediaMenu)
}

/*------------------------------------------------------------*/
/* build_label_menu */
/*------------------------------------------------------------*/

static Widget
build_label_menu(parent, name, label, bitmap)
    Widget parent;
    String name, label;
    Pixmap bitmap;
{
    Arg args[5];
    Cardinal n;
    Widget menu, entry;

    BEGINMESSAGE(build_label_menu)
								n=0;
    menu = XtCreatePopupShell("menu", simpleMenuWidgetClass,parent, args, n);
								n=0;
    XtSetArg(args[n], XtNlabel, label);			        n++;
    if (bitmap) {
       XtSetArg(args[n], XtNleftMargin, 20);			n++;
       XtSetArg(args[n], XtNleftBitmap, bitmap);		n++;
    }
    XtSetArg(args[n], XtNjustify, XtJustifyCenter);	        n++;
    entry = XtCreateManagedWidget(name, smeBSBObjectClass,menu, args, n);
    ENDMESSAGE(build_label_menu)
    return menu;
}

/*############################################################*/
/* catch_Xerror */
/* Catch X errors die gracefully if one occurs */
/*############################################################*/

int
catch_Xerror(dpy, err)
    Display *dpy;
    XErrorEvent *err;
{
    BEGINMESSAGE(catch_Xerror)
    if (err->error_code == BadImplementation) {
	old_Xerror(dpy, err);
	return 0;
    }
    if (dying) return 0;
    dying = True;
    bomb = *err;
    XtDestroyWidget(toplevel);
    ENDMESSAGE(catch_Xerror)
    return 0;
}
