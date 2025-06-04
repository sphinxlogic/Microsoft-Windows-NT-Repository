/*
**
** options_setup.c
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
#include INC_XAW(SmeBSB.h)
#include "Aaa.h"
#include "Button.h"
#include "FileSel.h"
#include "Ghostview.h"
#include "Frame.h"
#include "MButton.h"
#include "Switch.h"

#include "types.h"
#include "callbacks.h"
#include "d_memdebug.h"
#include "widgets_misc.h"
#include "magmenu.h"
#include "main_resources.h"
#include "main_globals.h"
#include "media.h"
#include "misc.h"
#include "miscmenu.h"
#include "note.h"
#include "options.h"
#include "options_gv.h"
#include "options_setup.h"
#include "scale.h"
#include "ps.h"
#include "doc_misc.h"

static Widget   popup=NULL,optionControl;
static Widget   eyeGuideToggle,reverseScrollingToggle,confirmPrintToggle,autoCenterToggle;
static Widget   pixmapToggle,miscLabel;
static Widget   confirmLabel,confirmButton,confirmMenu,showTitleToggle;
static Widget   print_command,scales,uncompress,screenSize,medias,magmenu,miscmenu;

static String confirm_quit_styles[4] = { "Never","When processing","Always", NULL };

static void options_setup_setOptionsAtEntry();
static void options_setup_create();
extern void main_setResolutions();
extern void main_createScaleMenu();

/*------------------------------------------------------
  OptionPopupStruct
------------------------------------------------------*/

OptionPopupStruct gv_options_setup_struct = {
  NULL,
  options_setup_create,
  options_setup_setOptionsAtEntry,
  False
};

OptionPopup gv_options_setup = (OptionPopup) &gv_options_setup_struct;

/*------------------------------------------------------
   options_setup_setOptionsAtEntry
------------------------------------------------------*/

static void options_setup_setOptionsAtEntry()
{
  String s;
  Arg args[2];
  Cardinal n;
  char tmp[50];

  BEGINMESSAGE(options_setup_setOptionsAtEntry)

  widgets_setToggle(confirmPrintToggle, (app_res.confirm_print ? 1 : 0));
  widgets_setToggle(reverseScrollingToggle, (app_res.reverse_scrolling ? 1 : 0));
  widgets_setToggle(eyeGuideToggle, (app_res.scrolling_eye_guide ? 1 : 0));
  widgets_setToggle(showTitleToggle, (app_res.show_title ? 1 : 0));
  widgets_setToggle(pixmapToggle, (app_res.use_bpixmap ? 1 : 0));
  widgets_setToggle(autoCenterToggle, (app_res.auto_center ? 1 : 0));

  SMESSAGE(gv_print_command)
    widgets_setText(print_command, gv_print_command);
  SMESSAGE(gv_uncompress_command)
    widgets_setText(uncompress,gv_uncompress_command);
  SMESSAGE(gv_scales_res)
    s = options_squeezeMultiline(gv_scales_res);
    widgets_setText(scales,s);
    GV_XtFree(s);
  SMESSAGE(gv_medias_res)
    s = options_squeezeMultiline(gv_medias_res); 
    widgets_setText(medias,s);
    GV_XtFree(s);
  SMESSAGE(gv_magmenu_entries_res)
    s = options_squeezeMultiline(gv_magmenu_entries_res);
    widgets_setText(magmenu,s);
    GV_XtFree(s);
  SMESSAGE(gv_miscmenu_entries_res)
    s = options_squeezeMultiline(gv_miscmenu_entries_res);
    widgets_setText(miscmenu,s);
    GV_XtFree(s);
  sprintf(tmp,"%d x %d",gv_screen_width,gv_screen_height);
  widgets_setText(screenSize,tmp);

  s=confirm_quit_styles[app_res.confirm_quit];
                                   n=0;
  XtSetArg(args[n], XtNlabel, s);  n++;
  XtSetValues(confirmButton, args, n);

  ENDMESSAGE(options_setup_setOptionsAtEntry)
}

/*------------------------------------------------------
   options_setup_cb_apply
------------------------------------------------------*/

static void options_setup_cb_apply(w, client_data, call_data)
   Widget	w;
   XtPointer	client_data, call_data;
{
   Arg args[5];
   Cardinal n;
   Boolean b;
   int i,j;
   Boolean redisplay=False;
   Boolean reopen=False;
   char *l;

   BEGINMESSAGE(options_setup_cb_apply)

   app_res.confirm_print = SwitchIsSet(confirmPrintToggle) ? True : False ;
   app_res.reverse_scrolling = SwitchIsSet(reverseScrollingToggle) ? True : False ;
   app_res.scrolling_eye_guide = SwitchIsSet(eyeGuideToggle) ? True : False ;
   app_res.auto_center = SwitchIsSet(autoCenterToggle) ? True : False ;

   b = app_res.use_bpixmap;
   app_res.use_bpixmap = SwitchIsSet(pixmapToggle) ? True : False;
   cb_useBackingPixmap(NULL,(XtPointer)2,NULL);
   if (b != app_res.use_bpixmap) reopen=True;

   b = SwitchIsSet(showTitleToggle) ? True : False;
   if (b != app_res.show_title) {
     cb_showTitle(NULL,(XtPointer)1,NULL);
   }

   options_textApply(uncompress,NULL,&gv_uncompress_command);
   options_textApply(print_command,NULL,&gv_print_command);
   options_textApply(magmenu,NULL,&gv_magmenu_entries_res);
   magmenu_freeMagMenuEntries(gv_magmenu_entries);
   gv_magmenu_entries = magmenu_parseMagMenuEntries(gv_magmenu_entries_res);
   options_textApply(miscmenu,NULL,&gv_miscmenu_entries_res);
   miscmenu_freeMiscMenuEntries(gv_miscmenu_entries);
   gv_miscmenu_entries = miscmenu_parseMiscMenuEntries(gv_miscmenu_entries_res);
   l = widgets_getText(screenSize);
   i=j=0;
   sscanf(l," %d x %d ",&i,&j);
   if (i>0 && j>0 && (i != gv_screen_width || j != gv_screen_height)) {
     gv_screen_width = i;
     gv_screen_height = j;
     main_setResolutions(0);
     default_xdpi=default_ydpi=0.0;
     redisplay=True;
   }
							n=0;
   XtSetArg(args[n], XtNlabel, &l);                     n++;
   XtGetValues(confirmButton, args, n);
   i=0;
   while (i<3) {
     if (!strcmp(confirm_quit_styles[i],l)) app_res.confirm_quit = i;
     ++i;
   }

   {
     char *s,*sr;
     l = widgets_getText(scales);
     sr = options_squeezeMultiline(gv_scales_res);
     s  = options_squeezeMultiline(l);
     if (strcmp(s,sr)) {
       GV_XtFree(sr);
       GV_XtFree(gv_scales_res);
       gv_scales_res=s;
       scale_freeScales(gv_scales);
       gv_scales = scale_parseScales(gv_scales_res);
       XtDestroyWidget(scaleMenu);
       GV_XtFree(scaleEntry);
       main_createScaleMenu();
       gv_scale = scale_checkScaleNum(gv_scales,app_res.scale|SCALE_REL);
       if (gv_scale < 0) gv_scale = scale_checkScaleNum(gv_scales,0|SCALE_REL);
       gv_scale &= SCALE_VAL;
       gv_scale_base = scale_checkScaleNum(gv_scales,(app_res.scale_base-1)|SCALE_BAS);
       if (gv_scale_base < 0) gv_scale_base = 0;
       gv_scale_base &= SCALE_VAL;
       gv_scale_base_current = gv_scale_base;
       gv_scale_current = gv_scale;
       options_gv_createScaleMenus();
       gv_scale_current = gv_scale_base_current = -1;
       redisplay=True;
     } else {
       GV_XtFree(s);
       GV_XtFree(sr);
     }
   }

   {
     char *s,*sr;
     l = widgets_getText(medias);
     sr = options_squeezeMultiline(gv_medias_res);
     s  = options_squeezeMultiline(l);
     if (strcmp(s,sr)) {
       GV_XtFree(sr);
       GV_XtFree(gv_medias_res);
       gv_medias_res=s;
       media_freeMedias(gv_medias);
       gv_medias = media_parseMedias(gv_medias_res);
       gv_num_std_pagemedia = media_numMedias(gv_medias);
       if (pagemediaMenu) {
	 XtDestroyWidget(pagemediaMenu);
	 pagemediaMenu=NULL;
       }
       gv_fallback_pagemedia   = doc_convStringToPageMedia(NULL,app_res.fallback_pagemedia);
       if (gv_fallback_pagemedia == MEDIA_ID_INVALID) gv_fallback_pagemedia = doc_convStringToPageMedia(NULL,"A4");
       if (gv_fallback_pagemedia == MEDIA_ID_INVALID) {
	 gv_fallback_pagemedia=1;
	 while (!gv_medias[gv_fallback_pagemedia]->used) gv_fallback_pagemedia++;
       }
       gv_pagemedia = gv_pagemedia_old = MEDIA_ID_INVALID;
       gv_pagemedia_auto_old = -1;
       misc_buildPagemediaMenu();
       options_gv_createMediaMenus();
       redisplay=True;
     } else {
       GV_XtFree(s);
       GV_XtFree(sr);
     }
   }

							n=0;
   XtSetArg(args[n], XtNreverseScrolling,app_res.reverse_scrolling);n++;
   XtSetValues(FileSel, args, n);

   if (reopen && gv_filename) {
     cb_stopInterpreter(page,NULL,NULL);
     show_page(REQUEST_REOPEN,NULL);
   } else if (redisplay) show_page(REQUEST_OPTION_CHANGE,NULL);

   ENDMESSAGE(options_setup_cb_apply)
}

/*------------------------------------------------------
   options_setup_cb_save
------------------------------------------------------*/

static
void options_setup_cb_save(w, client_data, call_data)
  Widget	w;
  XtPointer	client_data, call_data;
{
  int    argn = 0;
  String argi[30];
  String argv[30];
  char tmp[20];
  Arg args[1];
  Cardinal n;
  int i;
  String l,t = "True",f = "False";

  BEGINMESSAGE(options_setup_cb_save)

  options_setArg(&(argi[argn]),&(argv[argn]),s_miscMenuEntries     ,gv_class       ,widgets_getText(miscmenu));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_magMenu             ,gv_class       ,widgets_getText(magmenu));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_medias              ,gv_class       ,widgets_getText(medias));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_scales              ,gv_class       ,widgets_getText(scales));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_printCommand        ,gv_class,widgets_getText(print_command));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_uncompressCommand   ,gv_class,widgets_getText(uncompress));
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_confirmPrint        ,gv_class       ,SwitchIsSet(confirmPrintToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_reverseScrolling    ,gv_class       ,SwitchIsSet(reverseScrollingToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_scrollingEyeGuide   ,gv_class       ,SwitchIsSet(eyeGuideToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_showTitle           ,gv_class       ,SwitchIsSet(showTitleToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_autoCenter          ,gv_class       ,SwitchIsSet(autoCenterToggle) ? t : f);
       ++argn;
  options_setArg(&(argi[argn]),&(argv[argn]),s_useBackingPixmap    ,gv_class       ,SwitchIsSet(pixmapToggle) ? t : f);
       ++argn;
	         	       	        n=0;
  XtSetArg(args[n], XtNlabel, &l);      n++;
  XtGetValues(confirmButton, args, n);
  i=0; while (i<3) {
     if (!strcmp(confirm_quit_styles[i],l)) sprintf(tmp,"%d",i);
     ++i;
   }
  options_setArg(&(argi[argn]),&(argv[argn]),s_confirmQuit        ,gv_class        ,tmp);
       ++argn;
#ifndef VMS
  {
    char name[255];
    char val[255];
    scale_getScreenResourceName(gv_display,val);
    sprintf(name,"%s",val);
    options_setArg(&(argi[argn]),&(argv[argn]),name                  ,gv_class       ,widgets_getText(screenSize));
       ++argn;
  }
#endif

  options_save(argn,argi,argv);
  while (--argn >=0) {
    GV_XtFree(argi[argn]); 
    GV_XtFree(argv[argn]);
  }

  ENDMESSAGE(options_setup_cb_save)
}

/*------------------------------------------------------
   options_setup_create
------------------------------------------------------*/

void options_setup_create()
{
   Arg          args[10];
   Cardinal     n;
   Widget       w;
   int i;

   BEGINMESSAGE(options_setup_create)

   if (popup) {
     INFMESSAGE(popup exists)
     ENDMESSAGE(options_setup_create)
     return;
   }

        						n=0;
        XtSetArg(args[n], XtNallowShellResize, True);	n++;
   popup = XtCreatePopupShell("optionsetupPopup",transientShellWidgetClass,toplevel, args, n);
   gv_options_setup->popup=popup;
         						n=0;
   optionControl = XtCreateManagedWidget("optionControl",aaaWidgetClass,popup,args,n);

   miscLabel              = XtCreateManagedWidget("misc",labelWidgetClass,optionControl,NULL,(Cardinal)0);
   confirmPrintToggle     = XtCreateManagedWidget("confirmPrint",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   reverseScrollingToggle = XtCreateManagedWidget("scrolling",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   eyeGuideToggle         = XtCreateManagedWidget("eyeGuide",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   autoCenterToggle       = XtCreateManagedWidget("autoCenter",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   showTitleToggle        = XtCreateManagedWidget("showTitle",switchWidgetClass,optionControl,NULL,(Cardinal)0);
   pixmapToggle           = XtCreateManagedWidget("pixmap",switchWidgetClass,optionControl,NULL,(Cardinal)0);

   options_createLabeledMenu("confirm",optionControl,&confirmLabel,&confirmButton,&confirmMenu);
     for (i = 0; confirm_quit_styles[i]; i++) {
       w = XtCreateManagedWidget(confirm_quit_styles[i],smeBSBObjectClass, confirmMenu,NULL,(Cardinal)0);
       XtAddCallback(w, XtNcallback,options_cb_changeMenuLabel,NULL); 
     }

        						n=0;
   w = XtCreateManagedWidget("apply", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_setup_cb_apply,NULL); 
   w = XtCreateManagedWidget("save", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_setup_cb_save,NULL);
   w = XtCreateManagedWidget("dismiss", buttonWidgetClass,optionControl, args, n);
         XtAddCallback(w, XtNcallback, options_cb_popdown,(XtPointer)gv_options_setup);
 
   miscmenu         = widgets_createLabeledTextField("miscmenu", optionControl);
   magmenu          = widgets_createLabeledTextField("magmenu", optionControl);
   medias           = widgets_createLabeledTextField("medias", optionControl);
   uncompress       = widgets_createLabeledLineTextField("uncompress",   optionControl);
   print_command    = widgets_createLabeledLineTextField("printCommand", optionControl);
   scales           = widgets_createLabeledTextField("scales", optionControl);
   screenSize       = widgets_createLabeledLineTextField("screenSize", optionControl);

   options_setup_setOptionsAtEntry();
   options_realize(popup,optionControl);
                                
   ENDMESSAGE(options_setup_create)
}


