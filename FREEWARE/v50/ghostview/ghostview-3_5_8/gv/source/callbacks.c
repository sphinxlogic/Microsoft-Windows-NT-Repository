/*
**
** callbacks.c
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
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#ifndef BUFSIZ
#   define BUFSIZ 1024
#endif

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include INC_XAW(Cardinals.h)
#include INC_XAW(Scrollbar.h)
#include "Clip.h"
#include "FileSel.h"
#include "Ghostview.h"
#include "Vlist.h"
#include INC_X11(IntrinsicP.h)

#ifdef VMS
#   define unlink remove
#   include <stat.h>
#   include <types.h>
#else
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <unistd.h>
#endif

#include "types.h"
#include "actions.h"
#include "callbacks.h"
#include "confirm.h"
#include "d_memdebug.h"
#include "file.h"
#include "ps.h"
#include "doc_misc.h"
#include "info.h"
#include "popup.h"
#include "process.h"
#include "dialog.h"
#include "magmenu.h"
#include "main_resources.h"
#include "main_globals.h"
#include "media.h"
#include "misc.h"
#include "miscmenu.h"
#include "note.h"
#include "save.h"
#include "resource.h"
#include "scale.h"
#include "misc_private.h"
#include "version.h"
#include "widgets_misc.h"

static char* save_directory = NULL;
static char* open_directory = NULL;

/*############################################################*/
/* cb_showTitle */
/*############################################################*/

void
cb_showTitle(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  String t=NULL,s;
  Arg args[2];
  Cardinal n;

  BEGINMESSAGE(cb_showTitle)
  if (client_data) {
    app_res.show_title = app_res.show_title ? False : True;
  }
  if (app_res.show_title) {
    if (doc && doc->title) t = doc->title;
    else if (gv_filename) {
#   ifdef VMS
      t = strrchr(gv_filename,']');
      if (!t) t = strrchr(gv_filename,':');
      if (t) t++;
      else t = gv_filename;
#   else
      t = gv_filename;
#   endif
    }
    if (!t) t = s = GV_XtNewString(versionIdentification[0]);
    else {
      s = GV_XtMalloc((4+strlen(t)+1)*sizeof(char));
      sprintf(s,"gv: %s",t);
    }
  } else {
    t = s = GV_XtNewString(versionIdentification[0]);
  }
					n=0;
  XtSetArg(args[n], XtNtitle, s);	n++;
  XtSetArg(args[n], XtNiconName, t);	n++;
  XtSetValues(toplevel,args,n);
  GV_XtFree(s);
  ENDMESSAGE(cb_showTitle)
}

/*############################################################*/
/* cb_newtocScrollbar */
/*############################################################*/

void
cb_newtocScrollbar(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  int x,y;

  BEGINMESSAGE(cb_newtocScrollbar)
  x = (int) newtocControl->core.x;
  if (((int)client_data)==1) {
    int dy = (int)call_data;
    y = (int) newtocControl->core.y - dy;
  } else {
    float *percent = (float *) call_data;
    y = (int)(-*percent * newtocControl->core.height);
  }
  ClipWidgetSetCoordinates(newtocClip, x, y);
  ENDMESSAGE(cb_newtocScrollbar)
}

/*##################################################################*/
/* cb_newtocClipAdjust */
/*##################################################################*/

void cb_newtocClipAdjust(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_newtocClipAdjust)
  XawScrollbarSetThumb(newtocScroll,
		       -(float)newtocControl->core.y/(float)newtocControl->core.height,
		       (float)newtocClip->core.height/(float)newtocControl->core.height);
  ENDMESSAGE(cb_newtocClipAdjust)
}

/*############################################################*/
/* cb_adjustSlider */
/*############################################################*/

void
cb_adjustSlider(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    static Dimension opw=0,oph=0,opvw=0,opvh=0;
    static Position opvx=0,opvy=0;
    XawPannerReport *report = (XawPannerReport*) call_data; 
    Dimension pw  = (Dimension) (report->canvas_width);
    Dimension ph  = (Dimension) (report->canvas_height);
    Dimension pvw = (Dimension) (report->slider_width);
    Dimension pvh = (Dimension) (report->slider_height);
    Position  pvx = (Position)  (report->slider_x);
    Position  pvy = (Position)  (report->slider_y);

    BEGINMESSAGE(cb_adjustSlider)
    if (gv_scroll_mode == SCROLL_MODE_PANNER) {
       INFMESSAGE(aborting due to wrong scroll mode) ENDMESSAGE(view_cb_adjustSlider)
       return;
    }
    if (!show_panner) {INFMESSAGE(panner not used)ENDMESSAGE(cb_adjustSlider)return;}
    
    if ((pw!=opw)||(ph!=oph)||(pvw!=opvw)||(pvh!=opvh)||(pvx!=opvx)||(pvy!=opvy)) {
       Arg args[5];
       Dimension sw,sh,cw,ch,bw;
       Position  sx,sy;
       static Dimension osw=0,osh=0;
       static Position  osx=0,osy=0;

       INFMESSAGE(detected changes)
       XtSetArg(args[0], XtNwidth,&cw);
       XtSetArg(args[1], XtNheight,&ch);
       XtSetArg(args[2], XtNborderWidth,&bw);
       XtGetValues(panner, args, THREE);

       sw = (Dimension) ((cw*pvw+pw/2)/pw);
       sh = (Dimension) ((ch*pvh+ph/2)/ph);
       if (pw>pvw) sx = (Position) (((cw-sw)*pvx+(pw-pvw)/2)/(pw-pvw)); else sx = 0;
       if (ph>pvh) sy = (Position) (((ch-sh)*pvy+(ph-pvh)/2)/(ph-pvh)); else sy = 0;

       IIMESSAGE(cw,ch)
       IIMESSAGE(sw,sh) IIMESSAGE(sx,sy)
       IIMESSAGE(pw,ph) IIMESSAGE(pvw,pvh) IIMESSAGE(pvx,pvy)

       INFMESSAGE(redisplaying slider)
       XtConfigureWidget(slider,sx,sy,sw,sh,bw);
       osw=sw; osh=sh; osx=sx; osy=sy;
       opw=pw; oph=ph; opvw=pvw; opvh=pvh; opvx=pvx; opvy=pvy;
    }
    ENDMESSAGE(cb_adjustSlider)
}

/*##################################################################*/
/* cb_antialias */
/*##################################################################*/

void cb_antialias(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_antialias)
  if (client_data) {
    app_res.antialias = app_res.antialias ? False : True;
    if (gv_filename) {
      cb_stopInterpreter(page,NULL,NULL);
      cb_reopen(page,NULL,NULL);
    }
  }
  widgets_setSelectedBitmap(antialiasEntry, app_res.antialias ? 1 : 0);
  ENDMESSAGE(cb_antialias)
}

/*##################################################################*/
/* cb_useBackingPixmap */
/*##################################################################*/

void cb_useBackingPixmap(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  int i = (int)client_data;

  BEGINMESSAGE(cb_useBackingPixmap)
  if (i&1) {
    app_res.use_bpixmap = app_res.use_bpixmap ? False : True;
  }
  if (i&2) {
    Arg args[1];
    XtSetArg(args[0],XtNuseBackingPixmap,app_res.use_bpixmap);
    XtSetValues(page,args,(Cardinal)1);
  }
  if (i&4 && gv_filename) cb_reopen(page,NULL,NULL);
  ENDMESSAGE(cb_useBackingPixmap)
}

/*##################################################################*/
/* cb_handleDSC */
/*##################################################################*/

void cb_handleDSC(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_handleDSC)
  if (client_data) {
    gv_scanstyle = (gv_scanstyle & SCANSTYLE_IGNORE_DSC) ?
      (gv_scanstyle & ~SCANSTYLE_IGNORE_DSC) :
      (gv_scanstyle |  SCANSTYLE_IGNORE_DSC);
    if (gv_filename) {
      cb_stopInterpreter(page,NULL,NULL);
      cb_reopen(page,NULL,NULL);
    }
  }
  widgets_setSelectedBitmap(dscEntry, (gv_scanstyle & SCANSTYLE_IGNORE_DSC) ? 0 : 1);
  ENDMESSAGE(cb_handleDSC)
}

/*##################################################################*/
/* cb_handleEOF */
/*##################################################################*/

void cb_handleEOF(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_handleEOF)
  if (client_data) {
    gv_scanstyle = (gv_scanstyle & SCANSTYLE_IGNORE_EOF) ?
      (gv_scanstyle & ~SCANSTYLE_IGNORE_EOF) :
      (gv_scanstyle |  SCANSTYLE_IGNORE_EOF);
    if (gv_filename) {
      cb_stopInterpreter(page,NULL,NULL);
      cb_reopen(page,NULL,NULL);
    }
  }
  widgets_setSelectedBitmap(eofEntry,(gv_scanstyle & SCANSTYLE_IGNORE_EOF));
  ENDMESSAGE(cb_handleEOF)
}

/*##################################################################*/
/* cb_stopInterpreter */
/*##################################################################*/

void cb_stopInterpreter(w, client_data, call_data)
   Widget w;
   XtPointer client_data, call_data;
{
   BEGINMESSAGE(cb_stopInterpreter)
   GhostviewDisableInterpreter(page);
   ENDMESSAGE(cb_stopInterpreter)
}

/*##################################################################*/
/* cb_pageAdjustNotify */
/*##################################################################*/

void
cb_pageAdjustNotify(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_pageAdjustNotify)
  if (gv_scroll_mode == SCROLL_MODE_GHOSTVIEW) {
    String params[2];
    Cardinal num_params=2;
    params[0]= "adjusted";
    params[1]= (char*) call_data;
    action_movePage(page,(XEvent*)NULL,params,&num_params);
  }
  ENDMESSAGE(cb_pageAdjustNotify)
}

/*##################################################################*/
/* cb_checkFile */
/*##################################################################*/

void
cb_checkFile(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    int changed;

    BEGINMESSAGE(cb_checkFile)
    changed = check_file(((int)client_data));
    if (changed==1) show_page(current_page,NULL);
    ENDMESSAGE(cb_checkFile)
}

/*##################################################################*/
/* cb_watchFile */
/*##################################################################*/

/*------------------------------------------------------------*/
/* watch_file */
/*------------------------------------------------------------*/

static void watch_file (client_data, idp)
  XtPointer client_data;
  XtIntervalId *idp;
{
  static XtIntervalId timer = (XtIntervalId) 0;
  unsigned long t = (unsigned long) app_res.watch_file_frequency;

  BEGINMESSAGE(watch_file)
  /* notification after timeout */
  if ((int)client_data && app_res.watch_file) {
    if (!file_fileIsNotUseful(gv_filename)) {
      int error;
      String s;
      struct stat sbuf;
      INFMESSAGE(checking file)
      s = GV_XtNewString(gv_filename);
#     ifdef VMS
        { char *c; c = strrchr(s,';'); if (c) *c='\0'; }
#     endif
      error = stat(s, &sbuf);
      if (!error && mtime != sbuf.st_mtime && sbuf.st_mtime < time(NULL))
         cb_checkFile(NULL,(XtPointer)(CHECK_FILE_VERSION|CHECK_FILE_DATE),NULL);
      GV_XtFree(s);
    }
  }
  if (timer) {
    XtRemoveTimeOut(timer);
    timer = (XtIntervalId) 0;
  }
  if (app_res.watch_file) {
    INFMESSAGE(adding timeout)
    timer = XtAppAddTimeOut(app_con,t,watch_file,(XtPointer)1);
  }
  ENDMESSAGE(watch_file)
}

void
cb_watchFile(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_watchFile)
  if (client_data) {
    app_res.watch_file = app_res.watch_file ? False : True;
  }
  watch_file(NULL,NULL);
  widgets_setSelectedBitmap(watchFileEntry,(app_res.watch_file ? 1 : 0));
  ENDMESSAGE(cb_watchFile)
}

/*##################################################################*/
/* cb_print */
/*##################################################################*/

static char *make_pagelist(mode)
   int mode;
{
   Boolean mode_valid=False;
   char *pagelist=NULL;

   BEGINMESSAGE(make_pagelist)
   if (toc_text && (mode&(PAGE_MODE_CURRENT|PAGE_MODE_MARKED))) {
      char *tmp;
      pagelist = GV_XtNewString(VlistVlist(newtoc));
      if (mode&PAGE_MODE_MARKED) {
	tmp = pagelist;
        while (*tmp) if (*tmp++=='*') { mode_valid=True; break; }
	if (!mode_valid && (mode&PAGE_MODE_CURRENT)) mode=PAGE_MODE_CURRENT;
      }
      if (mode==PAGE_MODE_CURRENT) {
	tmp = pagelist;
        while (*tmp) { *tmp=' '; tmp++; }
	pagelist[current_page]='*';
	mode_valid=True;
      }
   }
   if (!mode_valid) {
      GV_XtFree(pagelist);
      pagelist=NULL;
   }
   ENDMESSAGE(make_pagelist)
   return pagelist;
}

static char *get_pagelist(modep)
   int *modep;
{
   char *pagelist=NULL;
   int mode= *modep;

   BEGINMESSAGE(get_pagelist)
   if (toc_text && (mode&(PAGE_MODE_CURRENT|PAGE_MODE_MARKED))) {
      if (mode&PAGE_MODE_MARKED) {
         pagelist=make_pagelist(PAGE_MODE_MARKED);
         if (pagelist) mode=PAGE_MODE_MARKED;
      }
      if (!pagelist && (mode&PAGE_MODE_CURRENT)) {
         pagelist=make_pagelist(PAGE_MODE_CURRENT);
         if (pagelist) mode=PAGE_MODE_CURRENT;
      }
      if (!pagelist) mode=PAGE_MODE_INVALID;
   } else if (mode==PAGE_MODE_ALL) {
      pagelist=NULL; /* all pages */
   } else {
      mode=PAGE_MODE_INVALID;
   }
   *modep=mode;
   ENDMESSAGE(get_pagelist)
   return pagelist;
}

void
cb_print(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    char *prompt=GV_PRINT_MESSAGE;
    char *buttonlabel=GV_PRINT_BUTTON_LABEL;
    char *message;
    char *pagelist=NULL;

    BEGINMESSAGE(cb_print)

    if (!gv_filename) {
       INFMESSAGE(no file)
       ENDMESSAGE(cb_print)
       return;
    }

    gv_print_mode = (int)client_data;
    pagelist=get_pagelist(&gv_print_mode);
    if (pagelist) GV_XtFree(pagelist);
    if (gv_print_mode==PAGE_MODE_INVALID) {
       INFMESSAGE(invalid print mode)
       ENDMESSAGE(cb_print)
       return;
    }

    if (app_res.confirm_print) {
       if        (gv_print_mode==PAGE_MODE_MARKED) {
          message=GV_PRINT_MARKED_MESSAGE; INFMESSAGE(printing marked pages)
       } else if (gv_print_mode == PAGE_MODE_CURRENT) {
          message=GV_PRINT_PAGE_MESSAGE;   INFMESSAGE(printing current page)
       } else {
          message=GV_PRINT_ALL_MESSAGE;    INFMESSAGE(printing document)
       }
       DialogPopupSetPrompt(prompt);
       DialogPopupSetMessage(message);
       DialogPopupSetButton(DIALOG_BUTTON_DONE,buttonlabel,cb_doPrint);
       DialogPopupSetButton(DIALOG_BUTTON_CANCEL,NULL,cb_cancelPrint);
       DialogPopupSetText(gv_print_command);
       cb_popupDialogPopup((Widget)NULL,NULL,NULL);
       ENDMESSAGE(cb_print)
       return;
    }   
    cb_doPrint((Widget)NULL,NULL,(XtPointer)gv_print_command);
    ENDMESSAGE(cb_print)
}

/*##################################################################*/
/* cb_doPrint */
/*##################################################################*/

void
cb_doPrint(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    String print_command;
    String error=NULL;
    char *pagelist=NULL;

    BEGINMESSAGE(cb_doPrint)

    if (call_data) print_command = (String)(call_data);  /* dialog was not used */  
    else           print_command = DialogPopupGetText(); /* dialog was used */  
    if (!print_command) print_command="";
    SMESSAGE(print_command)

    cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);

    pagelist=get_pagelist(&gv_print_mode);
    if (gv_print_mode != PAGE_MODE_INVALID) {
       SaveData sd          = save_allocSaveData();
       sd->save_fn          = NULL;
       sd->src_fn           = gv_filename_unc ?
	                        GV_XtNewString(gv_filename_unc) :
	                        GV_XtNewString(gv_filename);
       sd->conv_fn          = NULL;
       sd->pagelist         = pagelist ? GV_XtNewString(pagelist) : NULL;
       sd->print_cmd        = print_command ? GV_XtNewString(print_command) : NULL;
       sd->convert          = gv_filename_dsc ? 1 : 0;
       sd->save_to_file     = (gv_print_kills_file || pagelist) ? 1 : 0;
       sd->save_to_printer  = 1;
       sd->print_kills_file = gv_print_kills_file;
       sd->scanstyle        = gv_scanstyle;
       error = save_saveFile(sd);
    }
    if (error) {
       NotePopupShowMessage(error);
       GV_XtFree(error);
    } else {
       cb_popdownDialogPopup((Widget)NULL,(XtPointer)NULL,NULL);
    }
    if (pagelist) GV_XtFree(pagelist);

    ENDMESSAGE(cb_doPrint)
}

/*##################################################################*/
/* cb_cancelPrint */
/*##################################################################*/

void
cb_cancelPrint(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_cancelPrint)
    cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
    cb_popdownDialogPopup((Widget)NULL,(XtPointer)NULL,NULL);
    ENDMESSAGE(cb_cancelPrint)
}

/*##################################################################*/
/* cb_save */
/*##################################################################*/

void
cb_save(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    Arg args[10];
    Cardinal n;  
    char *title="Save";
    char *buttonlabel;
    Widget button = XtNameToWidget(FileSel,"button2");
    char *pagelist;
    char *name;
    char *path;
    char ext[20];
    size_t pathlen;
    char default_path[GV_MAX_FILENAME_LENGTH];

    BEGINMESSAGE(cb_save)

    gv_save_mode = (int)client_data;
    pagelist=get_pagelist(&gv_save_mode);
    if (pagelist) GV_XtFree(pagelist);
    if (gv_save_mode==PAGE_MODE_INVALID) {
       INFMESSAGE(invalid save mode)
       ENDMESSAGE(cb_save)
       return;
    }

    path=".";
    if (!save_directory && app_res.default_save_dir) path=app_res.default_save_dir;
    else if (save_directory)                         path=save_directory;
    XawFileSelectionSetPath(FileSel,path);
    XawFileSelectionScan(FileSel,XawFileSelectionRescan);
    path = XawFileSelectionGetPath(FileSel);
    name = file_locateFilename(gv_filename_raw);
    ext[0]='\0';

    if        (gv_save_mode==PAGE_MODE_MARKED) {
       XawFileSelectionRemoveButton(FileSel, 3);
       buttonlabel="Save Marked Pages";  INFMESSAGE(saving marked pages)
       strcpy(ext,"_pages");
    } else if (gv_save_mode==PAGE_MODE_CURRENT) {
       XawFileSelectionRemoveButton(FileSel, 3);
       buttonlabel="Save Current Page";  INFMESSAGE(saving current page)
       if (0<=current_page && current_page <= 9998) sprintf(ext,"_page_%d",(current_page+1));
       else strcpy(ext,"_page");
    } else {
       buttonlabel="Save Document";      INFMESSAGE(saving all pages)
       if (gv_filename_dsc) {
          Widget button3;
          XawFileSelectionAddButton(FileSel, 3, cb_doSave, (XtPointer)FILE_TYPE_PDF);
          button3 = XtNameToWidget(FileSel,"button3");
          n=0;
          XtSetArg(args[n], XtNlabel, "Save as PDF"); ++n;
          XtSetValues(button3,args,n);
       }
    }

    /*  We assume the if ext was filled, then we'll definitely write PDF. */
    name = GV_XtNewString(name);
    if (gv_filename_dsc && *ext) name=file_pdfname2psname(name);

    pathlen = strlen(path)+strlen(name)+strlen(ext);
    if (pathlen<GV_MAX_FILENAME_LENGTH-1) {
       sprintf(default_path,"%s%s",path,name);
       file_stripVersionNumber(default_path);
       strcat(default_path,ext);
       XawFileSelectionSetPath(FileSel,default_path);
    }
    GV_XtFree(name);

    n=0;
    XtSetArg(args[n], XtNtitle,title); ++n;
    XtSetValues(FileSel_popup, args, n);
    n=0;
    XtSetArg(args[n], XtNlabel, buttonlabel); ++n;
    XtSetValues(button,args,n);

    XtRemoveAllCallbacks(button, XtNcallback);
    XtAddCallback(button, XtNcallback,cb_doSave,NULL);

    XawFileSelectionPreferButton(FileSel,2);

    popup_positionPopup(FileSel_popup,viewFrame,POPUP_POSITION_POS,4,4);
    cb_popupPopup(w, (XtPointer)FileSel_popup, call_data);
    ENDMESSAGE(cb_save)
}

/*##################################################################*/
/* cb_doSave */
/*##################################################################*/

void
cb_doSave(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    String name;
    String error=NULL;
    char *pagelist;
    int type;

    BEGINMESSAGE(cb_doSave)
    if (client_data) type = (int)client_data;
    else type = FILE_TYPE_PS;

    name = XawFileSelectionGetPath(FileSel);
    if (file_fileIsDir(name)) {
      XawFileSelectionScan(FileSel,XawFileSelectionRescan);
      ENDMESSAGE(cb_doSave)
      return;
    }
    cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
    if (save_directory) GV_XtFree(save_directory);
    save_directory= file_getDirOfPath(name);
    SMESSAGE(name)
    pagelist=get_pagelist(&gv_save_mode);
    if (gv_save_mode != PAGE_MODE_INVALID) {
       SaveData sd          = save_allocSaveData();
       sd->save_fn          = name ? GV_XtNewString(name) : NULL;
       sd->src_fn           = gv_filename_unc ?
	                        GV_XtNewString(gv_filename_unc) :
	                        GV_XtNewString(gv_filename);
       sd->conv_fn          = NULL;
       sd->pagelist         = pagelist ? GV_XtNewString(pagelist) : NULL;
       sd->print_cmd        = NULL;
       sd->convert          = (gv_filename_dsc && type==FILE_TYPE_PS) ? 1 : 0;
       sd->save_to_file     = 1;
       sd->save_to_printer  = 0;
       sd->print_kills_file = gv_print_kills_file;
       sd->scanstyle        = gv_scanstyle;
       error = save_saveFile(sd);
    }
    if (error) {
       NotePopupShowMessage(error);
       GV_XtFree(error);
    } else {
       XtPopdown(FileSel_popup);
    }    
    if (pagelist) GV_XtFree(pagelist);
    ENDMESSAGE(cb_doSave)
}

/*##################################################################*/
/* cb_openFile */
/*##################################################################*/

void
cb_openFile(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    Arg args[1];
    Cardinal n;
    Widget button = XtNameToWidget(FileSel,"button2");

    BEGINMESSAGE(cb_openFile)

    XawFileSelectionRemoveButton(FileSel, 3);
    n=0;
    XtSetArg(args[n], XtNtitle, "Open File"); ++n;
    XtSetValues(FileSel_popup, args, n);
    n=0;
    XtSetArg(args[n], XtNlabel, "Open File"); ++n;
    XtSetValues(button, args, n);
    XtRemoveAllCallbacks(button, XtNcallback);
    XtAddCallback(button, XtNcallback,cb_doOpenFile,NULL);

    {
       char *path=".";
       if (open_directory) path=open_directory;
       XawFileSelectionSetPath(FileSel,path);
    }
    XawFileSelectionScan(FileSel,XawFileSelectionRescan);
    XawFileSelectionPreferButton(FileSel,2);
    popup_positionPopup(FileSel_popup,viewFrame,POPUP_POSITION_POS,4,4);
    cb_popupPopup(w, (XtPointer)FileSel_popup, call_data);
    ENDMESSAGE(cb_openFile)
}   

/*##################################################################*/
/* cb_doOpenFile */
/*##################################################################*/

void
cb_doOpenFile(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    String name,error;

    BEGINMESSAGE(cb_doOpenFile)
    name = XawFileSelectionGetPath(FileSel);
    SMESSAGE(name)
    if (open_directory) GV_XtFree(open_directory);
    open_directory=file_getDirOfPath(name);
    SMESSAGE(open_directory)
    if ((error = misc_testFile(name))) {
      XawFileSelectionScan(FileSel,XawFileSelectionRescan);
      GV_XtFree(error);
    } else {
      cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
      XtPopdown(FileSel_popup);
      show_page(REQUEST_NEW_FILE,(XtPointer)name);
    }
    ENDMESSAGE(cb_doOpenFile)
}

/*##################################################################*/
/* cb_reopen */
/* Explicitly reopen the file. */
/*##################################################################*/

void
cb_reopen(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    char *error=NULL;
    BEGINMESSAGE(reopen_file)

    if ((error = misc_testFile(gv_filename))) {
       NotePopupShowMessage(error);
       GV_XtFree(error);
    } else {
       cb_popdownNotePopup((Widget)NULL,(XtPointer)NULL,NULL);
       show_page(REQUEST_REOPEN,NULL);
    }
    ENDMESSAGE(reopen_file)
}

/*##################################################################*/
/* cb_redisplay */
/*##################################################################*/

void
cb_redisplay(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_redisplay)
    if (w && (XtClass(w) == ghostviewWidgetClass) && (w != page)) {
       INFMESSAGE(redisplay on zoom widget not enabled)
       ENDMESSAGE(cb_redisplay)
       return;
    }    
    show_page(REQUEST_REDISPLAY,NULL);
    ENDMESSAGE(cb_redisplay)
}

/*##################################################################*/
/* cb_page */
/*##################################################################*/

void
cb_page(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  int np;
  int cp;
  int k = 0;
  char *s;

  BEGINMESSAGE(cb_page)
  if (gv_psfile && client_data) {
    s = (char*)client_data;
    if (*s=='-' || *s=='+') {  
      k = 1;
      np = atoi(s);
    } else if (*s=='=' && *(s+1)=='h') {  
      np = VlistHighlighted(newtoc);
      if (np >=0) { 
	k=2;
	VlistChangeHighlighted(newtoc,np,XawVlistUnset);
      }
    } else {
      k = 2;
      if (!isdigit(*s)) s++;
      np = atoi(s)-1;
    }
    
    if ((k==1 && np) || k==2) {
      if (toc_text) {
	if (gv_pending_page_request>NO_CURRENT_PAGE) cp=gv_pending_page_request;
	else cp = VlistSelected(newtoc);
	np = np + ((k==1) ? cp : 0);
	np=doc_putPageInRange(doc,np);
	IIMESSAGE(np,current_page)
	misc_setPageMarker(np,0);
	if (np != current_page) show_page(np,NULL);
      } else {
	if (k==1 && np > 0) show_page(np,NULL);
      }
    }
  }
  ENDMESSAGE(cb_page)
}
         
/*##################################################################*/
/* cb_positionPage */
/*##################################################################*/

void
cb_positionPage(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  Widget clip,control,gvw;
  int clw,clh,cow,coh,px=0,py=0,x=0,y=0;
  Boolean center=False;
  Boolean scroll=False;
  Boolean have_pagepos=False;

  BEGINMESSAGE(cb_positionPage)

  if (w && (XtClass(w) == ghostviewWidgetClass)) {
    gvw     = w;
    control = XtParent(w);
    clip    = XtParent(control);
  } else {
    gvw     = page;
    control = viewControl;
    clip    = viewClip;
  }
  clw = (int)clip->core.width;
  clh = (int)clip->core.height;
  cow = (int)control->core.width;
  coh = (int)control->core.height;

  if ((int)client_data) center = True;
  if (!center) have_pagepos=misc_restorePagePosition(&px,&py);
  if (app_res.auto_center == True) center = True;

  if (have_pagepos) {
    GhostviewReturnStruct grs;
    Position ocx,ocy,cx,cy;

    INFMESSAGE(using saved page position)
    cx = ((Position)clip->core.width)/2  - control->core.x - page->core.x;
    cy = ((Position)clip->core.height)/2 - control->core.y - page->core.y;
    
    GhostviewGetAreaOfBB (gvw,px,py,px,py,&grs);
    ocx = (int) grs.psx;
    ocy = (int) grs.psy;
    x = (int)control->core.x - (int)(ocx - cx);
    y = (int)control->core.y - (int)(ocy - cy);
    scroll=True;
  } else if (center) {
    INFMESSAGE(centering)
    x = -(cow - clw)/2;
    y = -(coh - clh)/2;
    scroll=True;
  }
  if (scroll) {
    INFIIMESSAGE(setting position to,x,y)
    ClipWidgetSetCoordinates(clip,x,y);
  }
  ENDMESSAGE(cb_positionPage)
}

/*##################################################################*/
/* cb_setPageMark */
/* Set/unset the 'page marked' property */
/*##################################################################*/

void
cb_setPageMark(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    int r=(int)client_data;
    int entry=XawVlistInvalid,change=XawVlistInvalid;

    BEGINMESSAGE(cb_setPageMark)
    if (!toc_text) {
      INFMESSAGE(no toc) ENDMESSAGE(cb_setPageMark)
      return;
    }
    if      (r & SPM_ALL)     entry =  XawVlistAll;
    else if (r & SPM_EVEN)    entry =  XawVlistEven;
    else if (r & SPM_ODD)     entry =  XawVlistOdd;
    else if (r & SPM_CURRENT) entry =  XawVlistCurrent;
    if      (r & SPM_MARK)    change = XawVlistSet;
    else if (r & SPM_UNMARK)  change = XawVlistUnset;
    else if (r & SPM_TOGGLE)  change = XawVlistToggle;
    VlistChangeMark(newtoc,entry,change);
    ENDMESSAGE(cb_setPageMark)
}

/*##################################################################*/
/* cb_autoResize */
/*##################################################################*/

void
cb_autoResize(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
  Arg args[1];

  BEGINMESSAGE(cb_autoResize)
  if (client_data) {
    app_res.auto_resize = !(app_res.auto_resize);
    show_page(REQUEST_TOGGLE_RESIZE,NULL);
  }
  if (show_autoResize) {
    if (app_res.auto_resize) XtSetArg(args[0], XtNlabel,GV_AUTO_RESIZE_YES);
    else                     XtSetArg(args[0], XtNlabel,GV_AUTO_RESIZE_NO);
    XtSetValues(w_autoResize, args,ONE);
  }
  widgets_setSelectedBitmap(sizeEntry, app_res.auto_resize ? 1 : 0);
  ENDMESSAGE(cb_autoResize)
}

/*##################################################################*/
/* cb_setScale */
/*##################################################################*/

void
cb_setScale(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  int i=(int)client_data;

  BEGINMESSAGE(cb_setScale)
  i = scale_checkScaleNum(gv_scales,i);
  if (i>=0) {
    if (i&SCALE_BAS) gv_scale_base = i&SCALE_VAL;
    else             gv_scale = i&SCALE_VAL;
  }
  if (i>=0) {
    misc_savePagePosition();
    show_page(REQUEST_NEW_SCALE,NULL);
    misc_resetPagePosition();
  }
  ENDMESSAGE(cb_setScale)
}

/*##################################################################*/
/* cb_setOrientation */
/*##################################################################*/

void
cb_setOrientation(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    int o = (int) client_data;
    int changed = 1;

    BEGINMESSAGE(cb_setOrientation)
    switch (o) {
       case O_AUTOMATIC:
            INFMESSAGE(swapping auto orientation)
            if (gv_orientation_auto != 0) gv_orientation_auto = 0;
            else gv_orientation_auto = 1;
            break;
       case O_PORTRAIT:
       case O_SEASCAPE:
       case O_LANDSCAPE:
       case O_UPSIDEDOWN:
            gv_orientation = o;
            INFIMESSAGE(new orientation,gv_orientation)
            break;
       case O_SWAP_LANDSCAPE:
            INFMESSAGE(swapping landscape)
            if (gv_swap_landscape != 0) gv_swap_landscape = 0;
            else gv_swap_landscape = 1;
            break;
       default:
            INFMESSAGE(unknown orientation)
            changed = 0;
            break;
    }
    if (!call_data) {
      if (changed) {
	misc_savePagePosition();
	show_page(REQUEST_NEW_ORIENTATION,NULL);
	misc_resetPagePosition();
      }
    }
    ENDMESSAGE(cb_setOrientation)
}

/*##################################################################*/
/* cb_setPagemedia */
/*##################################################################*/

void
cb_setPagemedia(w, client_data, call_data)
   Widget w;
   XtPointer client_data, call_data;
{
   int media = (int)client_data;
   int media_bbox = doc ? doc->nummedia : 0;

   BEGINMESSAGE(cb_setPagemedia)
   if (media==MEDIA_ID_AUTO) {
     INFMESSAGE(toggling automatic media detection)
     gv_pagemedia_auto = gv_pagemedia_auto ? 0 : 1;
   } else if (media==media_bbox) {
     INFMESSAGE(changing to bounding box)
     if (doc_mediaIsOk(doc,current_page,media)) gv_pagemedia = media;
     else call_data = (XtPointer)1;
   } else {
     INFIMESSAGE(changing to pagemedia,media)
     gv_pagemedia = media;
   }
   if (!call_data) {
     show_page(REQUEST_NEW_PAGEMEDIA,NULL);
   }
   ENDMESSAGE(cb_setPagemedia)
}

/*##################################################################*/
/* cb_track */
/* track mouse pointer */
/*##################################################################*/

void
cb_track(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    GhostviewReturnStruct *p = (GhostviewReturnStruct *)call_data;

    BEGINMESSAGE1(cb_track)
    /* locator events have zero width and height */
    if ((p->width == 0)&&(p->height == 0)) {
        if (show_locator) {
           static char buf[MAX_LOCATOR_LENGTH];
           static int x,y;
           if ((x != p->psx) || (y != p->psy) || (buf[0]='\0')) {
    	      sprintf(buf, app_res.locator_format, p->psx, p->psy);
              update_label(locator,buf);
           }
           x=p->psx; y=p->psy;
        }
        ENDMESSAGE1(cb_track)
	return;
    }


   ENDMESSAGE1(cb_track)
}

/*##################################################################*/
/* cb_message */
/* Process messages from ghostscript */
/* Refresh occurs when window was resized unexpectedly */
/*##################################################################*/

void
cb_message(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    int i;
    char *error;

    BEGINMESSAGE(cb_message)
    if (!strcmp((char *) call_data, "Failed")) {
        INFMESSAGE(Failed)
	if ((Widget)client_data == page) {
            error = "\nError: PostScript interpreter failed in main window.\n\n";
	} else {
            error = "\nError: PostScript interpreter failed in zoom window.\n\n";
	}
	cb_appendInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)error);
    } else if (!strcmp((char *) call_data, "BadAlloc")) {
        INFMESSAGE(BadAlloc)
	if ((Widget)client_data == page) {
	    error = "\nWarning: Could not allocate backing pixmap in main window.\n\n";
	} else {
	    error = "\nWarning: Could not allocate backing pixmap in zoom window.\n\n";
	}
	cb_appendInfoPopup((Widget)NULL,(XtPointer)NULL,(XtPointer)error);
    } else if (!strcmp((char *) call_data, "Refresh")) {
        INFMESSAGE(Refresh)
	if (toc_text) {
	    GhostviewSendPS(w, gv_psfile, doc->beginprolog,
			    doc->lenprolog, False);
	    GhostviewSendPS(w, gv_psfile, doc->beginsetup,
			    doc->lensetup, False);
	    if (doc->pageorder == DESCEND)
		i = (doc->numpages - 1) - current_page;
	    else
		i = current_page;
	    GhostviewSendPS(w, gv_psfile, doc->pages[i].begin,
			    doc->pages[i].len, False);
	}
    } else if (!strcmp((char *) call_data, "Page")) {
        INFMESSAGE(completed page)
	if (w && (XtClass(w) == ghostviewWidgetClass) && (w != page)) {
	  INFMESSAGE(killing interpreter in zoom window)
	  GhostviewDisableInterpreter(w);
	}
	if ((gv_pending_page_request!=NO_CURRENT_PAGE) && (toc_text) && ((Widget)client_data == page)) {
           INFIMESSAGE(pending request for, gv_pending_page_request)
           show_page(gv_pending_page_request,NULL);
	}
    }
    ENDMESSAGE(cb_message)
}

/*##################################################################*/
/* cb_destroy */
/* Destroy popup window */
/*##################################################################*/

void
cb_destroy(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_destroy)
    XtDestroyWidget((Widget)client_data);
    ENDMESSAGE(cb_destroy)
}

/*------------------------------------------------------------------*/
/* cb_shutdown */
/*------------------------------------------------------------------*/

static void
cb_shutdown(w, client_data, call_data)
   Widget w;
   XtPointer client_data;
   XtPointer call_data;
{
   BEGINMESSAGE(cb_shutdown)

   if (gv_psfile) fclose(gv_psfile);
   if (gv_filename_dsc) unlink(gv_filename_dsc);
   if (gv_filename_unc) unlink(gv_filename_unc);
   process_kill_all_processes();

#if defined(DUMP_XTMEM) || defined(DUMP_MEM) 
   if (gv_filename_dsc)       GV_XtFree(gv_filename_dsc);
   if (gv_filename_unc)       GV_XtFree(gv_filename_unc);
   if (gv_filename_old)       GV_XtFree(gv_filename_old);
   if (gv_filename_raw)       GV_XtFree(gv_filename_raw);
   if (gv_filename)           GV_XtFree(gv_filename);
   if (doc)                   psfree(doc);
   if (olddoc)                psfree(olddoc);
   if (gv_scales_res)         GV_XtFree(gv_scales_res);
   if (gv_magmenu_entries)    magmenu_freeMagMenuEntries(gv_magmenu_entries);
   if (gv_miscmenu_entries)   miscmenu_freeMiscMenuEntries(gv_miscmenu_entries);
   if (gv_scales)             scale_freeScales(gv_scales);
   if (gv_medias_res)         GV_XtFree(gv_medias_res);
   if (gv_medias)             media_freeMedias(gv_medias);
   if (gv_user_defaults_file) GV_XtFree(gv_user_defaults_file);
   if (scaleEntry)            GV_XtFree(scaleEntry);
   if (open_directory)        GV_XtFree(open_directory);
   if (save_directory)        GV_XtFree(save_directory);
   if (toc_text)              GV_XtFree(toc_text);
   if (pagemediaEntry)        GV_XtFree(pagemediaEntry);
   if (gv_dirs)               GV_XtFree(gv_dirs);
   if (gv_filters)            GV_XtFree(gv_filters);
   if (gv_filter)             GV_XtFree(gv_filter);
   GV_XtFree(gv_magmenu_entries_res);
   GV_XtFree(gv_miscmenu_entries_res);
   GV_XtFree(gv_print_command);
   GV_XtFree(gv_uncompress_command);
   GV_XtFree(gv_gs_interpreter);
   GV_XtFree(gv_gs_cmd_scan_pdf);
   GV_XtFree(gv_gs_cmd_conv_pdf);
   GV_XtFree(gv_gs_x11_device);
   GV_XtFree(gv_gs_x11_alpha_device);
   GV_XtFree(gv_gs_arguments);
   resource_freeData();
   GV_MemoryDUMP
   GV_XtMemoryDUMP
#endif
   XtDestroyApplicationContext(app_con);
   ENDMESSAGE(cb_shutdown)
   ENDMESSAGE(exiting gv)
   exit(EXIT_STATUS_NORMAL);
}

/*##################################################################*/
/* cb_destroyGhost */
/* destroy callback for Ghostview widgets. */
/* The disable interpreter call ensures that ghostscript is killed. */
/* Once the count goes to 0, we are sure that all forked processes have */
/* been killed and that we can safely exit. */
/*##################################################################*/

void
cb_destroyGhost(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    BEGINMESSAGE(cb_destroyGhost)
    GhostviewDisableInterpreter((Widget) client_data);
    num_ghosts--;
    if (num_ghosts) {
       ENDMESSAGE(cb_destroyGhost)
       return;
    }
    gv_exiting=1;
    if (dying) old_Xerror(XtDisplay(w), &bomb);
    /* Okay, okay, I'm a little pedantic. But I want to see the line
           MemDebug:   Stack is CLEAN !
           XtMemDebug: Stack is CLEAN !
       when activating the memory debug routines. For this we have to ensure
       that all destroy routines of all widgets are executed before leaving
       the application. So we just create a new shell, hook a destroy
       callback to it and destroy it immediately. The trick is that the
       creation of the shell is delayed until the main loop next becomes idle;
       and this will be after the dust of the above destruction has settled down.
    */
    toplevel = XtAppCreateShell("shutdown",gv_class,applicationShellWidgetClass,gv_display,NULL,0);
    XtAddCallback(toplevel,XtNdestroyCallback,cb_shutdown,(XtPointer)NULL);
    XtDestroyWidget(toplevel);
}

/*##################################################################*/
/* cb_quitGhostview */
/* Start application folding up by Destroying the top level widget. */
/* The application exits when the last interpreter is killed during */
/* a destroy callback from ghostview widgets. */
/*##################################################################*/

void
cb_quitGhostview(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  char *message=NULL;
 
  BEGINMESSAGE(cb_quitGhostview)
  if (app_res.confirm_quit>=1) {
    message = process_disallow_quit();
    if (message || app_res.confirm_quit>=2) {
      ConfirmPopupSetMessage("2", message);
      ConfirmPopupSetMessage("3", "Do you really want to quit ?");
      ConfirmPopupSetButton(CONFIRM_BUTTON_DONE,cb_doQuit);
      ConfirmPopupSetButton(CONFIRM_BUTTON_CANCEL,cb_cancelQuit);
      ConfirmPopupSetInitialButton(CONFIRM_BUTTON_CANCEL);
      cb_popupConfirmPopup((Widget)NULL,NULL,NULL);
      ENDMESSAGE(cb_quitGhostview)
      return;
    }
  }
  cb_doQuit((Widget)NULL,NULL,(XtPointer)NULL);
  ENDMESSAGE(cb_quitGhostview)
}

/*##################################################################*/
/* cb_cancelQuit */
/*##################################################################*/

void
cb_cancelQuit(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_cancelQuit)
  cb_popdownConfirmPopup((Widget)NULL,(XtPointer)NULL,NULL);
  ENDMESSAGE(cb_cancelQuit)
}

/*##################################################################*/
/* cb_doQuit */
/*##################################################################*/

void
cb_doQuit(w, client_data, call_data)
  Widget w;
  XtPointer client_data, call_data;
{
  BEGINMESSAGE(cb_doQuit)
  cb_popdownConfirmPopup((Widget)NULL,(XtPointer)NULL,NULL);
  XtUnmapWidget(toplevel);   
  XtDestroyWidget(toplevel);
  ENDMESSAGE(cb_doQuit)
}


