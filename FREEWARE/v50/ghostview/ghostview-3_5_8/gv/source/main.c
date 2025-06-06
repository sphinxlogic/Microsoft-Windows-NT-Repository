/*
**
** main.c
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

#define _GV_MAIN_C_

/*
#define MESSAGES
*/
#include "message.h"

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(cursorfont.h)
#include INC_X11(StringDefs.h)
#include INC_X11(Shell.h)
#include INC_XAW(Cardinals.h)
#include INC_XAW(SimpleMenu.h)
#include INC_XAW(SmeBSB.h)
#include INC_XAW(SmeLine.h)
#include INC_XAW(Label.h)
#include INC_XAW(Scrollbar.h)
#include INC_XAW(XawInit.h)
#include INC_XMU(Editres.h)
#include "Aaa.h"
#include "Button.h"
#include "Clip.h"
#include "FileSel.h"
#include "Frame.h"
#include "Ghostview.h"
#include "MButton.h"
#include "Vlist.h"

#ifdef VMS
#   include <unixlib.h>
#   include "strcasecmp.h"
#   include <stat.h>
#else
#   include <string.h>
#   include <sys/stat.h>
#   include <unistd.h>
#endif

#include "types.h"
#include "actions.h"
#include "callbacks.h"
#include "confirm.h"
#include "d_memdebug.h"
#include "dialog.h"
#include "error.h"
#include "file.h"
#include "gv_message.h"
#include "note.h"
#include "info.h"
#include "magmenu.h"
#include "main_resources.h"
#include "main_globals.h"
#include "media.h"
#include "misc.h"
#include "miscmenu.h"
#include "options.h"
#include "options_gs.h"
#include "options_gv.h"
#include "options_setup.h"
#include "popup.h"
#include "process.h"
#include "ps.h"
#include "resource.h"
#include "doc_misc.h"
#include "version.h"
#include "stdc.h"
#include "scale.h"

#if defined(USE_SIGNAL_HANDLER) && (XtSpecificationRelease < 6)
#   undef USE_SIGNAL_HANDLER
#endif
#ifdef USE_SIGNAL_HANDLER
#   include "signal.h"
#endif /* USE_SIGNAL_HANDLER */


#define BITMAP_ARGS(name)\
           (const char*)CONCAT(name,_bits),\
           CONCAT(name,_width),\
           CONCAT(name,_height)
 
#include FALLBACK_ICON_PIXMAP
#include FALLBACK_SELECTED_BITMAP
#include FALLBACK_DOCUMENT_BITMAP
#include FALLBACK_MARK_ODD_BITMAP
#include FALLBACK_MARK_EVEN_BITMAP
#include FALLBACK_MARK_CURRENT_BITMAP
#include FALLBACK_MARK_UNMARK_BITMAP
#include FALLBACK_MARK_EMPTY_BITMAP

typedef struct menu_entry {
  Widget		*widgetP;
  String		name;
  XtCallbackProc	callback;
  XtPointer		client_data;
} MenuEntry;

static XtActionsRec actions[] = {
 { "GV_Antialias"	, action_antialias		},
 { "GV_CheckFile"	, action_checkFile		},
 { "GV_ConfigureNotify"	, action_shellConfigureNotify	},
 { "GV_DeleteWindow"	, action_deleteWindow		},
 { "GV_DismissPopup"	, action_dismissPopup		},
 { "GV_EraseLocator"	, action_eraseLocator		},
 { "GV_HandleDSC"	, action_handleDSC		},
 { "GV_MagMenu"         , magmenu_a_magMenu           	},
 { "GV_MenuPopdown"     , action_menuPopdown           	},
 { "GV_MiscMenu"        , miscmenu_a_miscMenu          	},
 { "GV_MovePage"	, action_movePage		},
 { "GV_Open"		, action_open			},
 { "GV_OtherPage"	, action_otherPage		},
 { "GV_Page"		, action_page			},
 { "GV_Panner"		, action_panner			},
 { "GV_Print"		, action_print			},
 { "GV_Quit"		, action_quit			},
 { "GV_Reopen"		, action_reopen			},
 { "GV_Resizing"	, action_autoResize		},
 { "GV_Save"		, action_save			},
 { "GV_SetScale"	, action_setScale		},
 { "GV_SetOrientation"	, action_setOrientation		},
 { "GV_SetPageMark"	, action_setPageMark		},
 { "GV_SetPageMedia"	, action_setPagemedia		},
 { "GV_Toc"		, action_toc			},
 { "GV_TogDialPrefBut"	, action_preferDialogPopupButton },
 { "GV_TogConfPrefBut"	, action_preferConfirmPopupButton },
 { "GV_WatchFile"	, action_watchFile },
};

/*--------------------------------------------------------------
   dummyCvtStringToPixmap
   Dummy String to Pixmap converter. Used to suppress warnings
   about missing String to Pixmap converter.
   Background: on Motif displays 'xrdb -q' shows resource
   entries "*topShadowPixmap: unspecified_pixmap" and
   "*bottomShadowPixmap: unspecified_pixmap". Since the ThreeD
   widget of Xaw3d also uses these resources without
   installing a converter we get tons of warnings. ###jp###
--------------------------------------------------------------*/

static Boolean
dummyCvtStringToPixmap(dpy, args, num_args, fromVal, toVal,converter_data)
   Display   *dpy;
   XrmValue  *args;
   Cardinal  *num_args;
   XrmValue  *fromVal;
   XrmValue  *toVal;
   XtPointer *converter_data;
{   
   BEGINMESSAGE(dummyCvtStringToPixmap)
#  ifdef MESSAGES
   {
      char *name = (char*) fromVal->addr;
      INFSMESSAGE(will not convert,name)
   }
#  endif
   ENDMESSAGE(dummyCvtStringToPixmap)
   return(False);
}

/*### Procedure and Macro Declarations ###########################################*/

static void  main_createMenu();
void main_setGhostscriptResources();
void main_setInternResource();
void main_setResolutions();
void main_createScaleMenu();

#ifdef max
#   undef max
#endif
#define max(a,b) ((a)>(b)?(a):(b))
#ifdef min
#   undef min
#endif
#define min(a,b) ((a)<(b)?(a):(b))

/*#################################################################################
   Main
#################################################################################*/

int main(argc, argv)
  int  argc;
  char *argv[];
{

  MAINBEGINMESSAGE(main)

  {
    Arg          args[20];
    Cardinal     n;
    int          number;
    Widget       cont_child[50];
    Cardinal     cont_child_num=0;
    Dimension    maximum_width,maximum_height;
    unsigned int gwidth=0,gheight=0;
    int          dim_forced;

/*###  initializing global variables ####################################*/

    INFMESSAGE(initializing global variables)
    gv_scroll_mode = SCROLL_MODE_NONE;
    gv_class = GV_CLASS;
    gv_pending_page_request=NO_CURRENT_PAGE;
    gv_gs_arguments  = NULL;
    gv_filename      = NULL;
    gv_filename_unc  = NULL;
    gv_filename_dsc  = NULL;
    gv_filename_old  = NULL;
    gv_filename_raw  = NULL;
    infopopup        = NULL;
    confirmpopup     = NULL;
    dialogpopup      = NULL;
    notepopup        = NULL;
    versionpopup     = NULL;
    FileSel_popup    = NULL;
    pagemediaEntry   = NULL;
#ifdef VMS
    gv_print_kills_file = 1;
#else
    gv_print_kills_file = 0;
#endif

/*###  initializing toolkit and the application context #################*/

    INFMESSAGE(initializing toolkit and the application context)
    XtToolkitInitialize();
    app_con = XtCreateApplicationContext();
    XtAppAddActions(app_con, actions, XtNumber(actions));

/*### opening display #######################################################*/

   INFMESSAGE(opening display)
   {
#ifndef VMS
     /*
       The following doesn't work for XFILESEARCHPATH since stupid
       XtResolvePathname, when passed a NULL path, checks XFILESEARCHPATH
       only on the first call. (jp)
     */
     const char *xufsp="XUSERFILESEARCHPATH";
     char *xuf;
     xuf = getenv(xufsp); if (xuf) xuf = GV_XtNewString(xuf);
     setenv(xufsp,"___",1);
#endif
     gv_display = XtOpenDisplay(app_con,NULL,NULL,gv_class,NULL,0,&argc,argv);
#ifndef VMS
     if (xuf) { setenv(xufsp,xuf,1); GV_XtFree(xuf); } else unsetenv(xufsp);
#endif
     if (!gv_display) {
       fprintf(stderr, "%s: Unable to open the display.\n", GV_APPLICATION_NAME);
       exit(EXIT_STATUS_ERROR);
     }
     XtGetApplicationNameAndClass(gv_display,&gv_name,&gv_class);
     SMESSAGE(gv_name) SMESSAGE(gv_class)
     gv_database=XtDatabase(gv_display);
   }

/*### getting resources ############################################*/

   {
     XrmDatabase db;
     INFMESSAGE(getting resources)
     db = resource_buildDatabase(gv_display,gv_class,gv_name,&argc,argv);
     XrmCombineDatabase(db,&gv_database,True);
   }

/*### show some help ?  ############################################*/
    {
      String s;
      s=resource_getResource(gv_database,gv_class,gv_name,"help",NULL);
      if (s) {
	if      (!strcasecmp(s,"?"))    s=message_usage;
	else if (!strcasecmp(s,"help")) s=message_help;
	else if (!strcasecmp(s,"version")) s=versionIdentification[0];
	else s = NULL;
      }
      if (s) {
	fprintf(stdout,"%s\n",s);
	XtDestroyApplicationContext(app_con);
	exit(EXIT_STATUS_ERROR);
      }
    }

/*### initializing widget set and creating application shell #########################*/

    INFMESSAGE(initializing widget set)
    XawInitializeWidgetSet();
    XtAppSetTypeConverter(app_con,XtRString,XtRPixmap,dummyCvtStringToPixmap,NULL,0,XtCacheNone,NULL);   
    old_Xerror = XSetErrorHandler(catch_Xerror);
    wm_delete_window = XInternAtom(gv_display, "WM_DELETE_WINDOW", False);
    dim_forced=resource_checkGeometryResource(&gv_database,gv_class,gv_name);

    INFMESSAGE(creating the application shell)
							     n=0;
      XtSetArg(args[n], XtNallowShellResize, (dim_forced?False:True));n++;
      XtSetArg(args[n], XtNtitle,versionIdentification[0]);  n++;
    toplevel = XtAppCreateShell(NULL,gv_class,applicationShellWidgetClass,gv_display,args,n);

    /* support for Editres ###jp### 06/18/95 */
    XtAddEventHandler(toplevel, (EventMask) 0, TRUE,_XEditResCheckMessages, (XtPointer)NULL);

/*### getting application resources ####################################*/

    INFMESSAGE(retrieving and analyzing application resources)
    XtGetApplicationResources(toplevel,(XtPointer) &app_res,resources,XtNumber(resources),NULL,ZERO);
    if (!resource_checkResources(gv_name,app_res.version,versionCompatibility)) {
      XtDestroyApplicationContext(app_con);
      exit(EXIT_STATUS_ERROR);
    }

/*### initialization of global variables based on resource ##################*/

    main_setGhostscriptResources(gv_database);
    main_setInternResource(gv_database,&gv_print_command,"printCommand");
    main_setInternResource(gv_database,&gv_uncompress_command,"uncompressCommand");

    gv_user_defaults_file = resource_userDefaultsFile();
    gv_screen = XtScreen(toplevel);
    gv_scanstyle = 0;
    gv_scanstyle |= (app_res.ignore_eof ? SCANSTYLE_IGNORE_EOF : 0);
    gv_scanstyle |= (app_res.respect_dsc ? 0 : SCANSTYLE_IGNORE_DSC);

    if (dim_forced) app_res.auto_resize=False;

    app_res.minimum_width  = app_res.minimum_width  < 300 ? 300 : app_res.minimum_width; 
    app_res.minimum_height = app_res.minimum_height < 300 ? 300 : app_res.minimum_height; 

    gv_medias_res = resource_getResource(gv_database,gv_class,gv_name,"medias","Medias");
    gv_medias_res = GV_XtNewString(gv_medias_res);
    gv_medias = media_parseMedias(gv_medias_res);
    gv_num_std_pagemedia = media_numMedias(gv_medias);

    gv_magmenu_entries_res = resource_getResource(gv_database,gv_class,gv_name,"magMenu","MagMenu");
    gv_magmenu_entries_res = GV_XtNewString(gv_magmenu_entries_res);
    gv_magmenu_entries = magmenu_parseMagMenuEntries(gv_magmenu_entries_res);

    gv_miscmenu_entries_res = resource_getResource(gv_database,gv_class,gv_name,"miscMenuEntries","MiscMenuEntries");
    gv_miscmenu_entries_res = GV_XtNewString(gv_miscmenu_entries_res);
    gv_miscmenu_entries = miscmenu_parseMiscMenuEntries(gv_miscmenu_entries_res);
    
    gv_scales_res = resource_getResource(gv_database,gv_class,gv_name,"scales","Scales");
    gv_scales_res = GV_XtNewString(gv_scales_res);
    gv_scales = scale_parseScales(gv_scales_res);
    gv_scale_current = gv_scale_base_current = -1;
    if (app_res.scale < 0) app_res.scale = (-app_res.scale)|SCALE_MIN;
    app_res.scale &= (SCALE_VAL|SCALE_MIN);
    gv_scale = scale_checkScaleNum(gv_scales,app_res.scale|SCALE_REL);
    if (gv_scale < 0) gv_scale = scale_checkScaleNum(gv_scales,0|SCALE_REL);
    gv_scale &= SCALE_VAL;
    if (app_res.scale_base<1) app_res.scale_base = 1;
    app_res.scale_base &= SCALE_VAL;
    gv_scale_base = scale_checkScaleNum(gv_scales,(app_res.scale_base-1)|SCALE_BAS);
    if (gv_scale_base < 0) gv_scale_base = 0;
    gv_scale_base &= SCALE_VAL;

    if      (app_res.confirm_quit < 0) app_res.confirm_quit = 0;
    else if (app_res.confirm_quit > 2) app_res.confirm_quit = 2;

    if (app_res.watch_file_frequency < 500) app_res.watch_file_frequency = 500;

    main_setResolutions(1);
    default_xdpi = gv_real_xdpi;
    default_ydpi = gv_real_ydpi;

/*### creating bitmaps #####################################################*/

    INFMESSAGE(setting the icon) 
    {
       Pixmap icon_pixmap;
       XtSetArg(args[0], XtNiconPixmap, &icon_pixmap);
       XtGetValues(toplevel, args, 1);
       if (icon_pixmap == None) {
          icon_pixmap = 
          XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                BITMAP_ARGS(FALLBACK_ICON_NAME));
          XtSetArg(args[0], XtNiconPixmap, icon_pixmap);
          XtSetValues(toplevel, args, 1);
       }
    }
       if (app_res.mark_odd_bitmap == None)
           app_res.mark_odd_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_MARK_ODD_NAME));
       if (app_res.mark_even_bitmap == None)
           app_res.mark_even_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_MARK_EVEN_NAME));
       if (app_res.mark_current_bitmap == None)
           app_res.mark_current_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_MARK_CURRENT_NAME));
       if (app_res.mark_unmark_bitmap == None)
           app_res.mark_unmark_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_MARK_UNMARK_NAME));
       if (app_res.mark_empty_bitmap == None)
           app_res.mark_empty_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_MARK_EMPTY_NAME));
       if (app_res.selected_bitmap == None)
           app_res.selected_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_SELECTED_NAME));
       if (app_res.document_bitmap == None)
           app_res.document_bitmap =
              XCreateBitmapFromData(gv_display, RootWindowOfScreen(gv_screen),
                                  BITMAP_ARGS(FALLBACK_DOCUMENT_NAME));

/*### Parsing maximum width, maximum height resources, creating control ####################*/

    {
       char *pos;
       int width,height;
       char* max_size_screen = "screen";

       INFMESSAGE(parsing maximum size resources)

       pos= strstr(app_res.maximum_width,max_size_screen);
       if (pos) { width=WidthOfScreen(gv_screen)+atoi(pos+strlen(max_size_screen)); }
       else     { width=atoi(app_res.maximum_width); }
       maximum_width = (width > 0 ? (Dimension) width : 0);
       maximum_width = max(maximum_width,(Dimension)app_res.minimum_width);
       if (maximum_width<(Dimension)gwidth) maximum_width=(Dimension)gwidth;
       pos= strstr(app_res.maximum_height,max_size_screen);
       if (pos) { height=HeightOfScreen(gv_screen)+atoi(pos+strlen(max_size_screen)); }
       else     { height=atoi(app_res.maximum_height); }
       maximum_height = (height > 0 ? (Dimension) height : 0);
       maximum_height = max(maximum_height,(Dimension)app_res.minimum_height);
       if (maximum_height<(Dimension)gheight) maximum_height=(Dimension)gheight;
       IIMESSAGE(maximum_width,maximum_height)

       INFMESSAGE(creating control)
                               				n=0;
       if (gwidth) {
          app_res.auto_resize=False;
          XtSetArg(args[n], XtNresizeWidth, False);	n++;
          XtSetArg(args[n], XtNwidth, (Dimension)gwidth);n++;
          INFIMESSAGE(forcing width for control:,gwidth)
       } else {
          XtSetArg(args[n], XtNresizeWidth, True);	n++;
       }
       if (gheight) {
          app_res.auto_resize=False;
          XtSetArg(args[n], XtNresizeHeight, False);	n++;
          XtSetArg(args[n], XtNheight, (Dimension)gheight);n++;
          INFIMESSAGE(forcing height for control:,gheight)
       } else {
          XtSetArg(args[n], XtNresizeHeight, True);	n++;
       }
       XtSetArg(args[n], XtNmaximumWidth, maximum_width); n++;
       XtSetArg(args[n], XtNmaximumHeight,maximum_height);n++; 
       XtSetArg(args[n], XtNminimumWidth, (Dimension)app_res.minimum_width); n++;
       XtSetArg(args[n], XtNminimumHeight,(Dimension)app_res.minimum_height);n++; 
       control = XtCreateWidget("control",aaaWidgetClass,toplevel,args,n);
    }

/*### Creating the Menus ###############################################################*/

    INFMESSAGE(menus)

   {
     MenuEntry m[] = {
       { &fileButton,       "fileButton", NULL , NULL},
       { &fileMenu,         "menu", NULL ,NULL},
       { &openEntry,        "open", cb_openFile, NULL},
       { &reopenEntry,      "reopen", cb_reopen, NULL},
       { &updateEntry,      "update", cb_checkFile, (XtPointer)CHECK_FILE_VERSION },
       { NULL,              "line", NULL, NULL },
       { &printAllEntry,    "printAllPages", cb_print, (XtPointer)PAGE_MODE_ALL},
       { &printMarkedEntry, "printMarkedPages",cb_print , (XtPointer)(PAGE_MODE_MARKED|PAGE_MODE_CURRENT)},
       { &saveAllEntry,     "saveAllPages", cb_save, (XtPointer)PAGE_MODE_ALL},
       { &saveMarkedEntry,  "saveMarkedPages",cb_save , (XtPointer)(PAGE_MODE_MARKED|PAGE_MODE_CURRENT)},
       { NULL,              "line", NULL, NULL },
       { &copyrightEntry,   "copyright", cb_popupVersionPopup, NULL},
       { NULL,              "line", NULL, NULL },
       { &quitEntry,        "quit", cb_quitGhostview, NULL},
       { NULL, NULL, NULL, NULL },
     };
     main_createMenu(m,cont_child,&cont_child_num);
   }

   {
     MenuEntry m[] = {
       { &stateButton,     "stateButton",NULL ,NULL},
       { &stateMenu,       "menu", NULL, NULL},
       { &stopEntry,       "stop", cb_stopInterpreter, NULL},
       { &dscEntry,        "dsc", cb_handleDSC, (XtPointer)1},
       { &eofEntry,        "eof", cb_handleEOF, (XtPointer)1},
       { &antialiasEntry,  "antialias", cb_antialias, (XtPointer)1},
       { &watchFileEntry,  "watchFile", cb_watchFile, (XtPointer)1},
       { &sizeEntry,       "size", cb_autoResize, (XtPointer)1},
       { NULL,             "line", NULL, NULL },
       { &optiongvEntry,   "optionsgv", NULL, NULL},
       { &optiongsEntry,   "optionsgs", NULL, NULL},
       { &optionfsEntry,   "optionsfs", NULL, NULL},
       { &optionsetupEntry,"optionssetup", NULL, NULL},
       { NULL, NULL, NULL, NULL },
     };
     main_createMenu(m,cont_child,&cont_child_num);
     XtAddCallback(optionfsEntry, XtNcallback,options_cb_popup,(XtPointer)gv_options_fs);
     XtAddCallback(optiongvEntry, XtNcallback,options_cb_popup,(XtPointer)gv_options_gv);
     XtAddCallback(optiongsEntry, XtNcallback,options_cb_popup,(XtPointer)gv_options_gs);
     XtAddCallback(optionsetupEntry, XtNcallback,options_cb_popup,(XtPointer)gv_options_setup);
     cb_handleDSC(dscEntry,NULL,NULL);
     cb_handleEOF(eofEntry,NULL,NULL);
     cb_antialias(dscEntry,NULL,NULL);
   }

   {
     MenuEntry m[] = {
       { &pageButton,     "pageButton", NULL, NULL},
       { &pageMenu,       "menu", NULL, NULL},
       { &nextEntry,      "next", cb_page, "+1"},
       { &redisplayEntry, "redisplay", cb_redisplay, NULL},
       { &prevEntry,      "prev", cb_page, "-1"},
       { NULL,            "line", NULL, NULL },
       { &centerEntry,    "center", cb_positionPage, (XtPointer)1},
       { NULL,            "line", NULL, NULL },
       { &currentEntry,   "current" , cb_setPageMark, (XtPointer)(SPM_CURRENT|SPM_TOGGLE) },
       { &oddEntry    ,   "odd"     , cb_setPageMark, (XtPointer)(SPM_ODD|SPM_TOGGLE)     },
       { &evenEntry   ,   "even"    , cb_setPageMark, (XtPointer)(SPM_EVEN|SPM_TOGGLE)    },
       { &unmarkEntry ,   "unmark"  , cb_setPageMark, (XtPointer)(SPM_ALL|SPM_UNMARK)     },
       { NULL, NULL, NULL, NULL },
     };
     main_createMenu(m,cont_child,&cont_child_num);
   }

   {
     MenuEntry m[] = {
       { &orientationButton, "orientationButton", NULL, NULL},
       { &orientationMenu,   "menu", NULL, NULL},
       { &autoOrientEntry,   "automatic", cb_setOrientation, (XtPointer)O_AUTOMATIC},
       { NULL,               "line", NULL, NULL },
       { &portraitEntry,     "portrait", cb_setOrientation, (XtPointer)O_PORTRAIT},
       { &landscapeEntry,    "landscape", cb_setOrientation, (XtPointer)O_LANDSCAPE},
       { &upsidedownEntry,   "upsidedown",cb_setOrientation , (XtPointer)O_UPSIDEDOWN},
       { &seascapeEntry,     "seascape", cb_setOrientation, (XtPointer)O_SEASCAPE},
       { NULL,               "line", NULL, NULL },
       { &swapEntry,         "swap", cb_setOrientation, (XtPointer)O_SWAP_LANDSCAPE},
       { NULL, NULL, NULL, NULL },
     };
     main_createMenu(m,cont_child,&cont_child_num);
   }

   {
							n=0;
     XtSetArg(args[n], XtNresize, True);		n++;
     pagemediaButton = XtCreateWidget("pagemediaButton",mbuttonWidgetClass,control,args,n);
     cont_child[cont_child_num] = pagemediaButton; cont_child_num++;
   }

   {
           					n=0;
     XtSetArg(args[n], XtNresize, False);	n++;
     processButton = XtCreateWidget("processButton", mbuttonWidgetClass,control,args,n);
     cont_child[cont_child_num] = processButton; cont_child_num++;
     processMenu=NULL;
   }

							n=0;
     scaleButton = XtCreateWidget("scaleButton",mbuttonWidgetClass,control,args,n);
     cont_child[cont_child_num] =scaleButton; cont_child_num++;
     main_createScaleMenu();

/*### Optional Widgets ##############################################################*/

    {
       char *layout;
       char *widgetname;

       layout = resource_getResource(gv_database,gv_class,gv_name,"control.layout","Aaa.Layout");
#if 0
       if (!layout) {
          fprintf(stderr,"  %s: Error, layout resource not found\n",versionIdentification[0]);
          exit(EXIT_STATUS_ERROR);
       }
#endif

       {
          int i=0;
          struct { Widget		*widget;
                   Bool 		*show;
                   String		name;
                   XtCallbackProc	callback;
                   XtPointer		client_data;
          } b[] = {
            { &w_nextPage          , &show_nextPage          , "nextPage"      , NULL             , (XtPointer)NULL },
            { &w_prevPage          , &show_prevPage          , "prevPage"      , NULL             , (XtPointer)NULL },
            { &w_toggleCurrentPage , &show_toggleCurrentPage , "toggleCurrent" , cb_setPageMark   , (XtPointer)(SPM_CURRENT|SPM_TOGGLE) },
            { &w_toggleEvenPages   , &show_toggleEvenPages   , "toggleEven"    , cb_setPageMark   , (XtPointer)(SPM_EVEN|SPM_TOGGLE)    },
            { &w_toggleOddPages    , &show_toggleOddPages    , "toggleOdd"     , cb_setPageMark   , (XtPointer)(SPM_ODD|SPM_TOGGLE)     },
            { &w_unmarkAllPages    , &show_unmarkAllPages    , "unmarkAll"     , cb_setPageMark   , (XtPointer)(SPM_ALL|SPM_UNMARK)     },
            { &w_printMarkedPages  , &show_printMarkedPages  , "printMarked"   , cb_print         , (XtPointer)(PAGE_MODE_MARKED|PAGE_MODE_CURRENT)},
            { &w_printAllPages     , &show_printAllPages     , "printAll"      , cb_print         , (XtPointer)PAGE_MODE_ALL           },
            { &w_saveMarkedPages   , &show_saveMarkedPages   , "saveMarked"    , cb_save          , (XtPointer)(PAGE_MODE_MARKED|PAGE_MODE_CURRENT)},
            { &w_saveAllPages      , &show_saveAllPages      , "saveAll"       , cb_save          , (XtPointer)PAGE_MODE_ALL            },
            { &w_openFile          , &show_openFile          , "openFile"      , cb_openFile      , (XtPointer)NULL },
            { &w_autoResize        , &show_autoResize        , "autoResize"    , cb_autoResize    , (XtPointer)1 },
            { &w_showThisPage      , &show_showThisPage      , "redisplay"     , cb_redisplay     , (XtPointer)NULL },
            { &w_updateFile        , &show_updateFile        , "updateFile"    , cb_checkFile     , (XtPointer)CHECK_FILE_VERSION },
            { &w_checkFile         , &show_checkFile         , "checkFile"     , cb_checkFile     , (XtPointer)CHECK_FILE_DATE },
            { NULL                 , NULL                    , NULL            , NULL             ,  NULL },
          };
          INFMESSAGE(optional widgets: buttons)
						n=0;
          XtSetArg(args[n], XtNresize, True);	n++;
          while (b[i].widget) {
             INFSMESSAGE(creating widget,b[i].name)
             *(b[i].show) = strstr(layout,b[i].name) ? True : False;
             if (*(b[i].show)) {
                *(b[i].widget) = XtCreateWidget(b[i].name,buttonWidgetClass,control,args,n);
                if (b[i].callback) XtAddCallback(*(b[i].widget),XtNcallback,b[i].callback,b[i].client_data);
                cont_child[cont_child_num] = *(b[i].widget); cont_child_num++;
             }
             ++i;
          }
#define   SET_BITMAP(w,s,b)\
	    if ((s) && (b)!= None) {\
							n=0;\
              XtSetArg(args[n], XtNbitmap, (b));	n++;\
	      XtSetValues((w),args,n);\
	    }
          SET_BITMAP(w_toggleCurrentPage,show_toggleCurrentPage,app_res.mark_current_bitmap)
          SET_BITMAP(w_toggleEvenPages,show_toggleEvenPages,app_res.mark_even_bitmap)
          SET_BITMAP(w_toggleOddPages,show_toggleOddPages,app_res.mark_odd_bitmap)
          SET_BITMAP(w_unmarkAllPages,show_unmarkAllPages,app_res.mark_unmark_bitmap)
#undef SET_BITMAP
          cb_autoResize(sizeEntry,NULL,NULL);
       }


       INFMESSAGE(optional widgets: informational widgets)
#      define _mw_(widget,show,name)						\
         widgetname=name;							\
         show = strstr(layout,widgetname) ? True : False;			\
         if (show) {								\
            widget = XtCreateWidget(widgetname,mbuttonWidgetClass,control,args,n);\
            cont_child[cont_child_num] = widget; cont_child_num++;		\
         }

	      						n=0;
	XtSetArg(args[n], XtNresize, True);		n++;
       _mw_( titlebutton , show_title   , "titleButton" );
       _mw_( datebutton  , show_date    , "dateButton"  );

       widgetname="locator";
       show_locator = strstr(layout,widgetname) ? True : False;
       if (show_locator) {
          char buf[MAX_LOCATOR_LENGTH];
          sprintf(buf,app_res.locator_format,9999,9999);
          XtSetArg(args[n], XtNlabel,buf);		n++;
          locator = XtCreateWidget(widgetname,labelWidgetClass,control,args,n);
          cont_child[cont_child_num] =locator; cont_child_num++;
       }
#      undef _mw_


       INFMESSAGE(panner)
       widgetname="pannerFrame";
       show_panner = strstr(layout,widgetname) ? True : False;
       if (show_panner) {

  									n=0;
          pannerFrame = XtCreateWidget("pannerFrame",frameWidgetClass,control,args,n);
          cont_child[cont_child_num] = pannerFrame; cont_child_num++;
  									n=0;
          panner = XtCreateManagedWidget("panner", compositeWidgetClass,pannerFrame, args, n);
         	      							n=0;
          XtSetArg(args[n], XtNresize,False);				n++;
          XtSetArg(args[n], XtNlabel,"");			      	n++;
          slider = XtCreateManagedWidget("slider", labelWidgetClass,panner, args, n);
       }

    } /* end of optional widgets */

/*### Table of Contents ###########################################################*/

    INFMESSAGE(table of contents)


  									n=0;
          newtocFrame = XtCreateWidget("newtocFrame",frameWidgetClass,control,args,n);
          cont_child[cont_child_num] = newtocFrame; cont_child_num++;
  									n=0;
          newtocClip = XtCreateManagedWidget("newtocClip", clipWidgetClass,newtocFrame, args, n);
	     XtAddCallback(newtocClip, XtNreportCallback,cb_newtocClipAdjust, (XtPointer)NULL);
  									n=0;
          newtocControl = XtCreateManagedWidget("newtocControl", aaaWidgetClass,newtocClip, args, n);
         	      							n=0;
          newtoc = XtCreateManagedWidget("newtoc", vlistWidgetClass,newtocControl, args, n);
									n=0;
          newtocScroll = XtCreateWidget("newtocScroll", scrollbarWidgetClass,control, args, n);
	     XtAddCallback(newtocScroll, XtNscrollProc,cb_newtocScrollbar, (XtPointer)1);
             XtAddCallback(newtocScroll, XtNjumpProc,cb_newtocScrollbar, (XtPointer)2);

          cont_child[cont_child_num] = newtocScroll; cont_child_num++;

/*### The Page View ###########################################################*/

   INFMESSAGE(viewport)

									n=0;
   viewFrame = XtCreateWidget("viewFrame", frameWidgetClass,control,args,n);
   cont_child[cont_child_num] = viewFrame; cont_child_num++;

									n=0;
   viewClip = XtCreateManagedWidget("viewClip", clipWidgetClass,viewFrame,args,n);
           if (show_panner) XtAddCallback(viewClip, XtNreportCallback,cb_adjustSlider,(XtPointer)NULL);
            XtAddCallback(viewClip, XtNreportCallback,cb_pageAdjustNotify,(XtPointer)NULL);

									n=0;
   viewControl = XtCreateManagedWidget("viewControl", aaaWidgetClass,viewClip,args,n);

   {
      Boolean b;
									n=0;
            XtSetArg(args[n], XtNinterpreter,gv_gs_interpreter);	n++;
            b = gv_gs_safer ? True : False;
            XtSetArg(args[n], XtNsafer,b);                              n++;
            b = gv_gs_quiet ? True : False;
            XtSetArg(args[n], XtNquiet,b);                              n++;
            b = app_res.use_bpixmap ? True : False;
            XtSetArg(args[n], XtNuseBackingPixmap,b);                   n++;
            XtSetArg(args[n], XtNarguments,gv_gs_arguments);            n++;
            XtSetArg(args[n], XtNlxdpi, (1000*default_xdpi));		n++;
            XtSetArg(args[n], XtNlydpi, (1000*default_ydpi));		n++;
     page = XtCreateManagedWidget("page", ghostviewWidgetClass,viewControl, args,n);
            num_ghosts++;

            XtAddCallback(page, XtNcallback, cb_track, (XtPointer)NULL);
            XtAddCallback(page, XtNdestroyCallback, cb_destroyGhost, (XtPointer)page);
            XtAddCallback(page, XtNmessageCallback, cb_message, (XtPointer)page);
            XtAddCallback(page, XtNoutputCallback,cb_appendInfoPopup, (XtPointer)NULL);
   }
/*### checking gv_filename and opening psfile #############################*/

    INFMESSAGE(checking gv_filename and opening psfile)
    if (argc > 1) gv_filename=GV_XtNewString(argv[1]);
    if (gv_filename && strcmp(gv_filename, "-")) {
      if (misc_changeFile(gv_filename)) {
	open_fail_error(errno,GV_ERROR_OPEN_FAIL,gv_filename,1);
	exit(EXIT_STATUS_ERROR);
      } else {
        GV_XtFree(gv_filename_old);
        gv_filename_old = NULL;
      }
    }

/*### remaining initialization #####################################################*/

    INFMESSAGE(remaining initialization)

#ifdef USE_SIGNAL_HANDLER
    signal_setSignalHandlers(1);
#endif

    GhostviewDisableInterpreter(page);

    gv_pagemedia = MEDIA_ID_INVALID;
    setup_ghostview();

    {
       int o;
       gv_orientation_old         = O_UNSPECIFIED;
       o = doc_convStringToDocOrient(app_res.default_orientation);
       if (o == O_AUTOMATIC) {
          gv_orientation_auto     = 1;
          gv_orientation_auto_old = 0;
          gv_orientation          = O_UNSPECIFIED;
       } else {
          gv_orientation_auto     = 0;
          gv_orientation_auto_old = 1;
          gv_orientation          = o;
       }
    }

    gv_fallback_orientation = doc_convStringToDocOrient(app_res.fallback_orientation);
    if (gv_fallback_orientation != O_PORTRAIT   && gv_fallback_orientation != O_LANDSCAPE &&
        gv_fallback_orientation != O_UPSIDEDOWN && gv_fallback_orientation != O_SEASCAPE)
        gv_fallback_orientation = O_PORTRAIT;

    gv_fallback_pagemedia   = doc_convStringToPageMedia(NULL,app_res.fallback_pagemedia);
    if (gv_fallback_pagemedia == MEDIA_ID_INVALID) gv_fallback_pagemedia = doc_convStringToPageMedia(NULL,"A4");
    if (gv_fallback_pagemedia == MEDIA_ID_INVALID) {
      gv_fallback_pagemedia=1;
      while (!gv_medias[gv_fallback_pagemedia]->used) gv_fallback_pagemedia++;
    }

    gv_swap_landscape_old = -1;
    if (app_res.swap_landscape) gv_swap_landscape = 1;
    else                        gv_swap_landscape = 0;

    gv_exiting = 0; 

    number = doc_convStringToPage(doc,app_res.page);
    number = doc_putPageInRange(doc,number);


/*### managing the children ######################################################*/

    INFMESSAGE(managing children of control)
    XtManageChildren((WidgetList)cont_child,cont_child_num);
    INFMESSAGE(managing control)
    XtManageChild(control);
    XtSetMappedWhenManaged(toplevel, False);
    INFMESSAGE(realizing toplevel)
    XtRealizeWidget(toplevel);
    XSetWMProtocols(gv_display, XtWindow(toplevel), &wm_delete_window, 1);

/*### Creating the File Selection Popup ###########################################*/

    INFMESSAGE(creating file selection popup)
                              				n=0;
            XtSetArg(args[n], XtNallowShellResize,True);n++;
    FileSel_popup = XtCreatePopupShell("fileSelPopup",transientShellWidgetClass,toplevel,args,n);
                              				n=0;
            XtSetArg(args[n], XtNbuttons,  2);		n++;
            XtSetArg(args[n], XtNpreferredButton, 2);	n++;
            XtSetArg(args[n], XtNreverseScrolling,app_res.reverse_scrolling);n++;
            if (app_res.scratch_dir) {
               XtSetArg(args[n], XtNtmpDir, app_res.scratch_dir); n++;
            }
    FileSel = XtCreateManagedWidget("fileSel",file_selectionWidgetClass,FileSel_popup,args,n);
	XtAddCallback(XtNameToWidget(FileSel,"button1"), XtNcallback,cb_popdownPopup,FileSel_popup);
	XtAddCallback(XtNameToWidget(FileSel,"button1"), XtNcallback,cb_popdownNotePopup,(XtPointer)NULL);

    XtRealizeWidget(FileSel_popup);
    XSetWMProtocols(gv_display,XtWindow(FileSel_popup),&wm_delete_window,1);

                                                n=0;
    XtSetArg(args[n], XtNfilters, &gv_filters);	n++;
    XtSetArg(args[n], XtNdirs,    &gv_dirs);	n++;
    XtSetArg(args[n], XtNfilter,  &gv_filter);	n++;
    XtGetValues(FileSel,args,n);
    gv_filters = GV_XtNewString(gv_filters);
    gv_dirs    = GV_XtNewString(gv_dirs);
    gv_filter  = GV_XtNewString(gv_filter);

/*### now we become visible ######################################################*/

    INFMESSAGE(switching off resize for buttons and labels)
							n=0;
	XtSetArg(args[0], XtNresize, False);		n++;
    if (show_nextPage)	        XtSetValues(w_nextPage,args,n);
    if (show_prevPage)	        XtSetValues(w_prevPage,args,n);
    if (show_toggleCurrentPage)	XtSetValues(w_toggleCurrentPage,args,n);
    if (show_toggleEvenPages)	XtSetValues(w_toggleEvenPages,args,n);
    if (show_toggleOddPages)	XtSetValues(w_toggleOddPages,args,n);
    if (show_unmarkAllPages)	XtSetValues(w_unmarkAllPages,args,n);
    if (show_saveMarkedPages)	XtSetValues(w_saveMarkedPages,args,n);
    if (show_printMarkedPages)	XtSetValues(w_printMarkedPages,args,n);
    if (show_printAllPages)	XtSetValues(w_printAllPages,args,n);
    if (show_openFile)		XtSetValues(w_openFile,args,n);
    if (show_autoResize)	XtSetValues(w_autoResize,args,n);
    if (show_showThisPage)	XtSetValues(w_showThisPage,args,n);
    if (show_updateFile)	XtSetValues(w_updateFile,args,n);
    if (show_checkFile)		XtSetValues(w_checkFile,args,n);
    if (show_locator) {
       				XtSetArg(args[n], XtNlabel,"");		n++;
				XtSetValues(locator,args,n);
    }

    process_menu(NULL,PROCESS_MENU_HIDE); /* hide the process button */

    setup_layout_ghostview();

    if (gv_filename) current_page=number;
    show_page(REQUEST_SETUP,NULL);
    cb_watchFile(watchFileEntry,NULL,NULL);

    /* must allow control to resize */
    AaaWidgetAllowResize((AaaWidget)control,True,True);

    INFMESSAGE(mapping toplevel)
    XtMapWidget(toplevel);
  }

  INFMESSAGE(waiting for events now)
  XtAppMainLoop(app_con);

  /* should never get here */
  return 1;
}

/*--------------------------------------------------
    main_createMenu
--------------------------------------------------*/

static void main_createMenu(m,list,numP)
  MenuEntry *m;
  Widget   *list;
  Cardinal *numP;  
{
  Arg args[1];
  Cardinal n;
  int i;

  BEGINMESSAGE(main_createMenu)
  SMESSAGE(m[0].name)

							n=0;
  *(m[0].widgetP) = XtCreateManagedWidget(m[0].name,mbuttonWidgetClass,control,args,n);
    list[*numP] = *(m[0].widgetP); (*numP)++;

							n=0;
  *(m[1].widgetP) = XtCreatePopupShell(m[1].name, simpleMenuWidgetClass,*(m[0].widgetP),args,n);

  i=2;
  while (m[i].name) {
    if (!strcmp(m[i].name,"line")) {
      XtCreateManagedWidget("line", smeLineObjectClass,*(m[1].widgetP),args,n);
    } else {
      *(m[i].widgetP) = XtCreateManagedWidget(m[i].name, smeBSBObjectClass,*(m[1].widgetP),args,n);
      if (m[i].callback) XtAddCallback(*(m[i].widgetP), XtNcallback,m[i].callback,m[i].client_data);
    }
    ++i;
  }
  ENDMESSAGE(main_createMenu)
}

/*##################################################
    main_setInternResource
##################################################*/

void main_setInternResource(db,sP,name)
  XrmDatabase db;
  String *sP;
  char *name;
{
  BEGINMESSAGE(main_setInternResource)
  *sP = resource_getResource(db,gv_class,gv_class,name,NULL);
  if (!*sP) *sP="";
  *sP = GV_XtNewString(*sP);
  ENDMESSAGE(main_setInternResource)
}

/*##################################################
    main_setGhostscriptResources
##################################################*/

void main_setGhostscriptResources(db)
  XrmDatabase db;
{
  char *s;

  BEGINMESSAGE(main_setGhostscriptResources)
  main_setInternResource(db,&gv_gs_interpreter,"gsInterpreter");
  main_setInternResource(db,&gv_gs_cmd_scan_pdf,"gsCmdScanPDF");
  main_setInternResource(db,&gv_gs_cmd_conv_pdf,"gsCmdConvPDF");
  main_setInternResource(db,&gv_gs_x11_device,"gsX11Device");
  main_setInternResource(db,&gv_gs_x11_alpha_device,"gsX11AlphaDevice");
  main_setInternResource(db,&gv_gs_arguments,"gsArguments");
  s = resource_getResource(db,gv_class,gv_class,"gsSafer",NULL);
  if (!strcasecmp(s,"true"))  gv_gs_safer = 1; else gv_gs_safer = 0;
  s = resource_getResource(db,gv_class,gv_class,"gsQuiet",NULL);
  if (!strcasecmp(s,"true"))  gv_gs_quiet = 1; else gv_gs_quiet = 0;
  ENDMESSAGE(main_setGhostscriptResources)
}

/*##################################################
   main_setResolutions
##################################################*/

void main_setResolutions(query)
  int query;
{
  BEGINMESSAGE(main_setResolutions)
  if (query) scale_getScreenSize(gv_display,gv_screen,gv_database,gv_class,gv_name,&gv_screen_width,&gv_screen_height);
  gv_real_xdpi = 72.0 * 72.0 * (float)gv_screen_width  / (25.4 * WidthOfScreen(gv_screen));
  gv_real_ydpi = 72.0 * 72.0 * (float)gv_screen_height / (25.4 * HeightOfScreen(gv_screen));
  gv_pixel_xdpi = 72.0;
  gv_pixel_ydpi = 72.0;
  IIMESSAGE(gv_screen_width,gv_screen_height)
  FMESSAGE(gv_real_xdpi) FMESSAGE(gv_real_ydpi) 
  FMESSAGE(gv_pixel_xdpi) FMESSAGE(gv_pixel_ydpi) 
  ENDMESSAGE(main_setResolutions)
}

/*##################################################
   main_createScaleMenu
##################################################*/

void main_createScaleMenu()
{
  Arg args[1];
  Cardinal n;
  int i;
  Boolean have_line = False;

  BEGINMESSAGE(main_createScaleMenu)
							n=0;
  scaleMenu = XtCreatePopupShell("menu", simpleMenuWidgetClass,scaleButton,args,n);
  for (i=0; gv_scales[i]; i++);
  scaleEntry = (Widget*) GV_XtMalloc(i*sizeof(Widget));
  for (i=0; gv_scales[i]; i++) {
    if (!have_line && !gv_scales[i]->is_base) {
      XtCreateManagedWidget("line", smeLineObjectClass,scaleMenu,NULL,(Cardinal)0);
      have_line=True;
    }
    scaleEntry[i] = XtCreateManagedWidget(gv_scales[i]->name, smeBSBObjectClass,scaleMenu,args,n);
    if (gv_scales[i]->is_base) XtAddCallback(scaleEntry[i], XtNcallback,cb_setScale,(XtPointer)(i|SCALE_BAS));
    else XtAddCallback(scaleEntry[i], XtNcallback,cb_setScale,(XtPointer)(i|SCALE_ABS));
  }
  ENDMESSAGE(main_createScaleMenu)
}







