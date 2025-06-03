char copyright[]="xbmbrowser Version 2.0  (c) Copyright Ashley Roll, 1992.";
/*
*****************************************************************************
** xbmbrowser version 2.0  (c) Copyright Ashley Roll, 1992.
** FILE: xbmbrowser.c
**
** xbmbrowser is Public Domain. However it, and all the code still belong to me.
** I do, however grant permission for you to freely copy and distribute it on 
** the condition that this and all other copyright notices remain unchanged in 
** all distributions.
**
** This software comes with NO warranty whatsoever. I therefore take no
** responsibility for any damages, losses or problems that the program may 
** cause.
*****************************************************************************
*/

#define MAIN
#include "xbmbrowser.h"
#include "patchlevel.h"
#include "user-menu.h"
#include "icon.xbm"


extern void DoQuit();
extern void Ok();
extern Widget MakeMenu();
extern Widget AddMenuItem();
extern void Show_Help();
extern void Inform_User();
extern void SetWindowIcon();
extern void set_name();
extern void destroy_Callback();
extern void rescan();
extern FileList *get_files();
extern void change_dir();
extern void setup_dialog();
extern void dir_menu();
extern void pos_dir();

void fill_bw();

/* -------------------------- */
static Atom  wm_delete_window;      /* insure that delete window works */

static XtActionsRec  actions[] = {  /* declare possible actions */
/* action_name, routine */
  { "quit",   DoQuit  },
  { "CD", change_dir },
  { "Set_Name", set_name },
  { "Ok", Ok },
  { "Pos_Dir", pos_dir },
};

/* translation table for label widgets for the bitmaps */
static char Translations[] = 
  "<EnterWindow>:      Set_Name() \n\
   <BtnDown>:    XawPositionSimpleMenu(bitmapMenu) MenuPopup(bitmapMenu) \n\
   <BtnUp>:      MenuPopdown(bitmapMenu)";

/* translation table for the dialogWidget (directory name) */
static char text_trans[] = 
  "<Key>Return:  CD() \n\
   Ctrl<Key>M:   CD() \n\
   <Btn3Down>:  Pos_Dir() MenuPopup(DirPopup) ";

/*  translation table for the list widget */
static char list_trans[] =
   "<Enter>: Set() \n\
    <Leave>: Unset() \n\
    <BtnMotion>: Set() \n\
    <BtnUp>: MenuPopdown(DirPopup) Notify() Unset()"; 

/*  translation table for the transient shell containing the list */
/* this translation makes the list popdown if the button is released 
   outside the window */
static char tshell_trans[] = 
       "<BtnUp>: MenuPopdown(DirPopup)";


/* fallback resources  */
static char *fall_back[] = {
   ".xbmbrowser.width: 450", 
   ".xbmbrowser.height: 500", 
   ".xbmbrowser.iconName: browser",
   ".xbmbrowser.?.TransientShell.width: 350",
   ".xbmbrowser.main.buttons.Command.width: 70",
   NULL
}; 

/* -------------------------- */

main(argc, argv)
  int argc;
  char **argv;
{
XtAppContext appcon;
Widget button;

ac = argc;
av = argv;
bw = NULL;
bname = NULL;
dialogs_made = FALSE;
file_list = NULL;
widgetList = NULL;
directory = dname;

  /* Setup the toplevel window */
  {
   Pixmap icon;
   char title[80];
        
      sprintf(title,"XbmBrowser Version 2.0 %s",PATCHLEVEL);

      toplevel = XtVaAppInitialize(
                 &appcon, "XbmBrowser",    /* app context, ClassName */
                 NULL, 0,                  /* app command line options */
                 &argc, argv,              /* command line */
                 fall_back,                /* Fall back resources */
                 XtNtitle,(XtArgVal)title,
                 NULL);                    /* End Va resource list */

      icon = XCreateBitmapFromData(
                 XtDisplay(toplevel), RootWindowOfScreen(XtScreen(toplevel)),
                 (char *)icon_bits, icon_width, icon_height);

      XtVaSetValues(toplevel,XtNiconPixmap, icon,NULL);

  }
  XtAppAddActions(appcon, actions, XtNumber(actions));


  if( argc > 2 )  useage();
  if(argc == 2) {
    if(chdir(argv[1]) != 0) {
      fprintf(stderr,"xbmbrowser: couldn't chdir to '%s'\n",argv[1]);
      exit(0);
    }
  }

  (void) getcwd(dname,253);

/* create the cursors */
  normalCursor = XCreateFontCursor(XtDisplay(toplevel),XC_left_ptr);
  waitCursor = XCreateFontCursor(XtDisplay(toplevel),XC_watch);
 
/* create a paned widget to put everything into */
  mainpw  = XtVaCreateManagedWidget("main",panedWidgetClass,toplevel,
            XtNcursor,(XtArgVal)normalCursor,
            NULL);

/* create the form widget to put the buttons in */
  bfw = XtVaCreateManagedWidget("buttons",formWidgetClass,mainpw,
              XtNshowGrip,(XtArgVal)False,
              XtNskipAdjust,(XtArgVal)True,NULL);

/* create the Buttons */
  button = XtVaCreateManagedWidget("quit",commandWidgetClass,bfw,
                  XtNlabel,(XtArgVal)"Quit",NULL);
  XtAddCallback(button,"callback",DoQuit,NULL);
  button = XtVaCreateManagedWidget("help",commandWidgetClass,bfw,
                  XtNfromHoriz,(XtArgVal)button,
                  XtNlabel,(XtArgVal)"Help",NULL);
  XtAddCallback(button,"callback",Show_Help,NULL);
  button = XtVaCreateManagedWidget("rescan",commandWidgetClass,bfw,
                  XtNfromHoriz,(XtArgVal)button,
                  XtNlabel,(XtArgVal)"Rescan",NULL);
  XtAddCallback(button,"callback",rescan,NULL);

/* dialogWidget to hold and get the directory to read from */

  atw = XtVaCreateManagedWidget("directory", dialogWidgetClass, mainpw,
           XtNlabel,(XtArgVal)"Current Directory",
           XtNvalue,(XtArgVal)dname,
           XtNshowGrip,(XtArgVal)False,
           XtNskipAdjust,(XtArgVal)True,
           NULL);

  XtOverrideTranslations(XtNameToWidget(atw,"value"),
                         XtParseTranslationTable(text_trans));

/* create the dirMenu list widget in a popup shell. */
  dirPopup =  XtVaCreatePopupShell("DirPopup",transientShellWidgetClass,atw,
              XtNoverrideRedirect,(XtArgVal)True,
              XtNallowShellResize,(XtArgVal)True,
              NULL);

  XtOverrideTranslations(dirPopup,XtParseTranslationTable(tshell_trans));

  dirList = XtVaCreateManagedWidget("DirList",listWidgetClass,dirPopup,
                  NULL);

  XtAddCallback(dirList,"callback",dir_menu,NULL);

  XtOverrideTranslations(dirList,XtParseTranslationTable(list_trans));


/* label widget to hold the name of the current bitmap */
  lw = XtVaCreateManagedWidget("label",labelWidgetClass,mainpw,
                  XtNlabel,(XtArgVal)"No Bitmap Selected",
                  XtNshowGrip,(XtArgVal)False,
                  XtNskipAdjust,(XtArgVal)True,NULL);

/* create a viewport widget to stick the bitmaps in */
{
int width;

  XtVaGetValues(toplevel,XtNwidth,&width,NULL);

  phw = XtVaCreateManagedWidget("viewport",viewportWidgetClass,mainpw,
                  XtNwidth,(XtArgVal)width,
                  XtNallowHoriz,(XtArgVal)False,
                  XtNallowVert,(XtArgVal)True,
                  XtNshowGrip,(XtArgVal)False,
                  XtNskipAdjust,(XtArgVal)True,NULL);

}

/* setup the user menu */
{
char rcfile[256];
FILE *f;

  /* try to find the HOME_DIR_FILE */
  sprintf(rcfile,"~/%s",HOME_DIR_FILE);
  expand_tilder(rcfile);

  if((f = fopen(rcfile,"r")) == (FILE *) NULL) {
    /* try the DEFAULT_RC_FILE */
    strcpy(rcfile,DEFAULT_RC_FILE);

    if((f = fopen(rcfile,"r")) == (FILE *) NULL) {
      fprintf(stderr,"Xbmbrowser: Can not find a menu definition file\n"); 
      fprintf(stderr,"  looking for: ~/%s \n  or %s\n",HOME_DIR_FILE,DEFAULT_RC_FILE);
      DoQuit();
    } 
  }
  fclose(f); 
  Create_user_menu(phw ,"bitmapMenu",rcfile);
}
 setup();
 XtRealizeWidget(toplevel);

  /* Set the window to call quit() action if `deleted' */
  XtOverrideTranslations(toplevel,     
          XtParseTranslationTable("<Message>WM_PROTOCOLS:quit()") );
  wm_delete_window = XInternAtom(XtDisplay(toplevel),"WM_DELETE_WINDOW",False);
  (void) XSetWMProtocols(XtDisplay(toplevel), XtWindow(toplevel),
                                                 &wm_delete_window, 1);
  XtAppMainLoop(appcon);
exit(0);
}

/*
** this (re)initializes the boxWidget that contains the menubuttons
** by calling the fillbw() function
*/
setup()
{
int width,fore,back,depth;

  if(bw == NULL) {
    XtVaGetValues(toplevel,XtNwidth,&width,NULL);
 
    /* create the box widget to put all the bitmaps in */
    bw = XtVaCreateManagedWidget("bitmaps",boxWidgetClass,phw,
                    XtNwidth,width,
                    XtNorientation,(XtArgVal)XtorientVertical,
                    XtNfromVert,(XtArgVal)lw,
                    NULL);

    XtVaGetValues(bw, XtNborderColor, &fore,
                      XtNbackground,  &back,
                      XtNdepth,       &depth, NULL);

    XtVaSetValues(bw, XtNbackgroundPixmap,
          (XtArgVal)XmuCreateStippledPixmap(XtScreen(phw), fore, back, depth),
                    NULL);

  }

/* create all the bitmaps (each in a menubutton) */
  /* destory the old list and bitmaps */
  while(file_list != NULL) {
    FileList *t;

    t = file_list->next;
    free(file_list);
    file_list = t;
  }
  file_list = get_files(".");

  build_dirMenu();

  fill_bw();

  XtVaSetValues(lw,XtNlabel,(XtArgVal)"No Bitmap Selected",NULL);
}

/* 
 * build the dirMenu by going through the list and searching for directories
 */
build_dirMenu()
{
FileList *this;
static char *list[256];
int list_offset;

  list_offset = 0;
  list[list_offset++] = "/";
  list[list_offset++] = "~/";
 
  /* add to the menu any name in the file list that is a directory */
  for(this = file_list;this != NULL; this = this->next)
  {
    if(this->is_dir) {
         list[list_offset++] = this->fname;
    }
  }
  list[list_offset] = NULL;
  XtVaSetValues(dirList,XtNdefaultColumns,(XtArgVal)(list_offset/25)+1,NULL);
  XawListChange(dirList,list,0,0,True);
}

useage()
{
fprintf(stderr,"\nxbmbrowser: Useage: \nxbmbrowser [toolkit options] [<Dir>]\n");
fprintf(stderr,"\nWhere <Dir> is a directory name. \n");
exit(1);
}

/* some defines to make adding a widget to a WidgetList eaiser */
#define ADDTOLIST(w,l) bcopy((char *)(w),(char *)l,(int)sizeof(Widget)); l++;

/*
** this creates all the menubuttons that contain the bitmaps
*/
void fill_bw()
{
FileList *x;
WidgetLst *wl;
WidgetList manList,unmanList;
WidgetList m,u;
char b_name[255],name[10];
Pixmap bitmap = NULL;
int show = TRUE;
int wcount = 0,fcount = 0,uc,mc;
static int wnum = 0;

  /* count the number of widgets */
  for(wl = widgetList; wl != NULL; wcount++, wl = wl->next);

  /* count the number of bitmap files */
  for(x = file_list; x != NULL; x = x->next)  
    if(x->bmOK == BitmapSuccess)
      fcount++;
 
  /* set wcount to be the higher of the two */
  wcount = (wcount < fcount) ? fcount:wcount;

  /* allocate memory for the WidgetLists for Manageing and Unmanageing */
  manList = (WidgetList) malloc((wcount+1)*sizeof(Widget));
  unmanList = (WidgetList) malloc((wcount+1)*sizeof(Widget));
  m = manList;
  u = unmanList;
  uc = mc = 0;

  if(widgetList == NULL) {
    widgetList = (WidgetLst *) malloc(sizeof(WidgetLst));
    widgetList->w = NULL;
    widgetList->next = NULL;
  }

  /* add all the widgets to the unmanageList */
  wl = widgetList;
  while(wl != NULL) {
    if(wl->w != NULL) {
      ADDTOLIST(wl->w,u); uc++;
    }

    wl = wl->next;
  }

  /* unmanage the unmanList  - This stops alot of geometry req's from the 
     children when they change bitmaps, and makes things faster */
  if(uc > 0)
    XtUnmanageChildren(unmanList,uc);


  for(x = file_list, wl = widgetList; x != NULL; x = x->next)
  {
  int t; 

    if(! x->is_dir) {
      if(x->bmOK == BitmapSuccess) {

        /* if there is a precreated widget to hold the bitmap, then useit */
        if(wl->w != NULL) {

          /* get the old bitmap so we can destroy it */
          XtVaGetValues(wl->w,XtNbitmap,(XtArgVal)&bitmap,NULL);

          /* set the bitmap */
          XtVaSetValues(wl->w,XtNbitmap,(XtArgVal)x->bitmap,NULL);

          /* Destroy the old bitmap */
          XFreePixmap(XtDisplay(toplevel), bitmap);

          /* set the fname field */
          strcpy(wl->fname,x->fname);

          /* add the widget to the manage list */
          ADDTOLIST(wl->w,m); mc++;

        } else {
          /* create the label widget for it and set the bitmap */
          sprintf(name,"%d",wnum); 
          wnum++;
          wl->w = XtVaCreateWidget(name,labelWidgetClass,bw,
                                   XtNbitmap,(XtArgVal)x->bitmap,
                                   NULL);

          XtOverrideTranslations(wl->w,XtParseTranslationTable(Translations));

          /* set the fname field */
          strcpy(wl->fname,x->fname);

          /* add the widget to the manage list */
          ADDTOLIST(wl->w,m); mc++;

        } 
          
        if(wl->next == NULL) {
          /* allocate another WidgetLst element */
          wl->next = (WidgetLst *) malloc(sizeof(WidgetLst)); 
          wl = wl->next;
          wl->next = NULL;
          wl->w = NULL;

        } else {
          wl = wl->next;
        }
      }
    }
  }
 

  /* manage the manList */
  if(mc > 0)
    XtManageChildren(manList,mc);

  /* free the WidgetLists */
  free(manList); 
  free(unmanList); 

}

