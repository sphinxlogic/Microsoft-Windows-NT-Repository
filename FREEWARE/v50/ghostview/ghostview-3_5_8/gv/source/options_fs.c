/*
**
** options_fs.c
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
#include <ctype.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include "Aaa.h"
#include "Button.h"
#include "FileSel.h"
#include "Switch.h"

#include "types.h"
#include "callbacks.h"
#include "d_memdebug.h"
#include "widgets_misc.h"
#include "main_resources.h"
#include "main_globals.h"
#include "misc.h"
#include "note.h"
#include "options.h"
#include "options_fs.h"

static Widget popup,optionControl;
static Widget filter,scratch_dir,default_save_dir,filters,dirs;

static void options_fs_create();
static void options_fs_setOptionsAtEntry();

/*------------------------------------------------------
  OptionPopupStruct
------------------------------------------------------*/

OptionPopupStruct gv_options_fs_struct = {
  NULL,
  options_fs_create,
  options_fs_setOptionsAtEntry,
  False
};

OptionPopup gv_options_fs = (OptionPopup) &gv_options_fs_struct;

/*------------------------------------------------------
   options_fs_setOptionsAtEntry
------------------------------------------------------*/

static void options_fs_setOptionsAtEntry()
{
  String s;

  BEGINMESSAGE(options_fs_setOptionsAtEntry)
  SMESSAGE(app_res.scratch_dir)
  widgets_setText(scratch_dir,  app_res.scratch_dir);
  SMESSAGE(app_res.default_save_dir)
  widgets_setText(default_save_dir,  app_res.default_save_dir);
  SMESSAGE(gv_filters)
  s = options_squeezeMultiline(gv_filters); widgets_setText(filters,s); GV_XtFree(s);
  SMESSAGE(gv_dirs)
  s = options_squeezeMultiline(gv_dirs); widgets_setText(dirs,s); GV_XtFree(s);
  SMESSAGE(gv_filter)
  widgets_setText(filter,gv_filter);
  ENDMESSAGE(options_fs_setOptionsAtEntry)
}

/*------------------------------------------------------
   options_fs_cb_apply
------------------------------------------------------*/

static void options_fs_cb_apply(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;
{
   BEGINMESSAGE(options_fs_cb_apply)
   Arg args[5];
   Cardinal n;
   static Boolean s_scratch_dir = False;
   static Boolean s_default_save_dir = False;

   BEGINMESSAGE(options_fs_cb_apply)

   options_textApply(scratch_dir,&s_scratch_dir,&app_res.scratch_dir);
   options_textApply(default_save_dir,&s_default_save_dir,&app_res.default_save_dir);
   options_textApply(filters,NULL,&gv_filters);
   options_textApply(dirs,NULL,&gv_dirs);
   options_textApply(filter,NULL,&gv_filter);
							n=0;
   XtSetArg(args[n], XtNtmpDir, app_res.scratch_dir);	n++;
   XtSetArg(args[n], XtNfilters, gv_filters);		n++;
   XtSetArg(args[n], XtNdirs, gv_dirs);			n++;
   XtSetArg(args[n], XtNfilter, gv_filter);		n++;
   XtSetValues(FileSel, args, n);

   ENDMESSAGE(options_fs_cb_apply)
}

/*------------------------------------------------------
   options_fs_cb_save
------------------------------------------------------*/

static
void options_fs_cb_save(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  int    argn = 0;
  String argi[30];
  String argv[30];

  BEGINMESSAGE(options_fs_cb_save)

  options_setArg(&(argi[argn]),&(argv[argn]),"*filter"              ,gv_class       ,widgets_getText(filter));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),"*filters"             ,gv_class       ,widgets_getText(filters));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),"*dirs"                ,gv_class       ,widgets_getText(dirs));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_scratchDir          ,gv_class       ,widgets_getText(scratch_dir));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_defaultSaveDir      ,gv_class       ,widgets_getText(default_save_dir));
       ++argn;

  options_save(argn,argi,argv);
  while (--argn >=0) {
    GV_XtFree(argi[argn]); 
    GV_XtFree(argv[argn]);
  }

  ENDMESSAGE(options_fs_cb_save)
}

/*------------------------------------------------------
   options_fs_create
------------------------------------------------------*/

static void options_fs_create()
{
   Arg          args[10];
   Cardinal     n;
   Widget       w;

   BEGINMESSAGE(options_fs_create)

        						n=0;
        XtSetArg(args[n], XtNallowShellResize, True);	n++;
   popup = XtCreatePopupShell("optionfsPopup",transientShellWidgetClass,toplevel, args, n);
   gv_options_fs->popup = popup;

         						n=0;
   optionControl = XtCreateManagedWidget("optionControl",aaaWidgetClass,popup,args,n);

        						n=0;
   w = XtCreateManagedWidget("apply", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_fs_cb_apply,NULL); 
   w = XtCreateManagedWidget("save", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_fs_cb_save,NULL);
   w = XtCreateManagedWidget("dismiss", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_cb_popdown,(XtPointer)gv_options_fs);
 
   scratch_dir      = widgets_createLabeledLineTextField("scratchDir",   optionControl);
   default_save_dir = widgets_createLabeledLineTextField("saveDir",      optionControl);
   filter           = widgets_createLabeledLineTextField("filter",       optionControl);
   filters          = widgets_createLabeledTextField("filters", optionControl);
   dirs             = widgets_createLabeledTextField("dirs", optionControl);

   options_fs_setOptionsAtEntry();
   options_realize(popup,optionControl);
                             
   ENDMESSAGE(options_fs_create)
}

